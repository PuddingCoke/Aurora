#include<Aurora/VideoEncoder/NvidiaEncoder.h>

bool NvidiaEncoder::encode()
{
	timeStart = timer.now();

	const D3D11_VIDEO_PROCESSOR_STREAM processorStream = { TRUE,0,0,0,0,NULL,inputView.Get(),NULL };

	videoContext->VideoProcessorBlt(videoProcessor.Get(), outputView.Get(), 0, 1, &processorStream);

	NV_ENC_REGISTER_RESOURCE registerResource = { NV_ENC_REGISTER_RESOURCE_VER };

	registerResource.bufferFormat = NV_ENC_BUFFER_FORMAT_NV12;

	registerResource.bufferUsage = NV_ENC_INPUT_IMAGE;

	registerResource.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;

	registerResource.resourceToRegister = nv12Texture->getResource();

	registerResource.width = width;

	registerResource.height = height;

	registerResource.pitch = 0;

	registerResource.pInputFencePoint = nullptr;

	registerResource.pOutputFencePoint = nullptr;

	nvencAPI.nvEncRegisterResource(encoder, &registerResource);

	NV_ENC_MAP_INPUT_RESOURCE mapResource = { NV_ENC_MAP_INPUT_RESOURCE_VER };

	mapResource.registeredResource = registerResource.registeredResource;

	nvencAPI.nvEncMapInputResource(encoder, &mapResource);

	NV_ENC_PIC_PARAMS picParams = { NV_ENC_PIC_PARAMS_VER };

	if (frameEncoded == frameToEncode)
	{
		picParams.encodePicFlags = NV_ENC_PIC_FLAG_EOS;

		nvencAPI.nvEncEncodePicture(encoder, &picParams);

		encoding = false;

		std::cout << "[class NvidiaEncoder] encode complete!\n";

		std::cout << "[class NvidiaEncoder] frame encode avg speed " << frameToEncode / encodeTime << "\n";
	}
	else
	{
		picParams.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;

		picParams.inputBuffer = mapResource.mappedResource;

		picParams.bufferFmt = NV_ENC_BUFFER_FORMAT_NV12;

		picParams.inputWidth = width;

		picParams.inputHeight = height;

		picParams.outputBitstream = bitstream.bitstreamBuffer;

		const NVENCSTATUS status = nvencAPI.nvEncEncodePicture(encoder, &picParams);

		if (status == NV_ENC_SUCCESS)
		{
			NV_ENC_LOCK_BITSTREAM lockBitstream = { NV_ENC_LOCK_BITSTREAM_VER };

			lockBitstream.outputBitstream = bitstream.bitstreamBuffer;

			lockBitstream.doNotWait = 0;

			nvencAPI.nvEncLockBitstream(encoder, &lockBitstream);

			pkt->pts = av_rescale_q(frameEncoded, AVRational{ 1,(int)frameRate }, outStream->time_base);
			pkt->dts = pkt->pts;
			pkt->stream_index = outStream->index;
			pkt->data = (uint8_t*)lockBitstream.bitstreamBufferPtr;
			pkt->size = lockBitstream.bitstreamSizeInBytes;

			if (!memcmp(lockBitstream.bitstreamBufferPtr, "\x00\x00\x00\x01\x67", 5))
			{
				pkt->flags |= AV_PKT_FLAG_KEY;
			}

			av_write_frame(outCtx, pkt);

			av_write_frame(outCtx, nullptr);

			nvencAPI.nvEncUnlockBitstream(encoder, lockBitstream.outputBitstream);
		}

		std::cout << "Encoding... " << frameEncoded + 1 << "\n";
	}

	nvencAPI.nvEncUnmapInputResource(encoder, mapResource.mappedResource);

	nvencAPI.nvEncUnregisterResource(encoder, registerResource.registeredResource);

	frameEncoded++;

	timeEnd = timer.now();

	const float frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;

	encodeTime += frameTime;

	return encoding;
}

NvidiaEncoder::NvidiaEncoder(const UINT& width, const UINT& height, const UINT& frameToEncode, const UINT& frameRate, ID3D11Resource* const inputTexture2D, bool& initializeStatus) :
	frameToEncode(frameToEncode), frameEncoded(0u), encoding(true), encodeTime(0), width(width), height(height), frameRate(frameRate), encoder(nullptr),
	nv12Texture(new Texture2D(width, height, 1, 1, FMT::NV12, D3D11_BIND_RENDER_TARGET, 0)), outCtx(nullptr), outStream(nullptr),
	nvencAPI{ NV_ENCODE_API_FUNCTION_LIST_VER },
	bitstream{ NV_ENC_CREATE_BITSTREAM_BUFFER_VER }
{
	moduleNvEncAPI = LoadLibraryA("nvEncodeAPI64.dll");

	if (moduleNvEncAPI == 0)
	{
		std::cout << "[class NvidiaEncoder] load nvEncodeAPI64.dll failed\n";
		initializeStatus = false;
		return;
	}

	initializeStatus = true;

	NVENCSTATUS(*ApiCreateInstance)(NV_ENCODE_API_FUNCTION_LIST*) = (NVENCSTATUS(*)(NV_ENCODE_API_FUNCTION_LIST*))GetProcAddress(moduleNvEncAPI, "NvEncodeAPICreateInstance");

	std::cout << "[class NvidiaEncoder] api instance create status " << ApiCreateInstance(&nvencAPI) << "\n";

	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS sessionParams = { NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER };
	sessionParams.device = Renderer::getDevice();
	sessionParams.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
	sessionParams.apiVersion = NVENCAPI_VERSION;

	std::cout << "[class NvidiaEncoder] open encode session status " << nvencAPI.nvEncOpenEncodeSessionEx(&sessionParams, &encoder) << "\n";

	NV_ENC_PRESET_CONFIG presetConfig = { NV_ENC_PRESET_CONFIG_VER,{NV_ENC_CONFIG_VER} };

	std::cout << "[class NvidiaEncoder] get preset config status " << nvencAPI.nvEncGetEncodePresetConfigEx(encoder, codec, preset, tuningInfo, &presetConfig) << "\n";

	NV_ENC_CONFIG config;
	memcpy(&config, &presetConfig.presetCfg, sizeof(NV_ENC_CONFIG));
	config.version = NV_ENC_CONFIG_VER;
	config.profileGUID = profile;
	config.rcParams.rateControlMode = NV_ENC_PARAMS_RC_VBR;
	config.rcParams.averageBitRate = 12000000U;
	config.rcParams.maxBitRate = 14000000U;

	//高质量编码
	config.gopLength = 120;
	config.frameIntervalP = 1;
	config.encodeCodecConfig.hevcConfig.idrPeriod = config.gopLength;
	config.rcParams.multiPass = NV_ENC_TWO_PASS_FULL_RESOLUTION;
	config.rcParams.vbvBufferSize = config.rcParams.maxBitRate * 4;
	config.rcParams.vbvInitialDelay = 0;
	config.rcParams.enableAQ = 1;

	NV_ENC_INITIALIZE_PARAMS encoderParams = { NV_ENC_INITIALIZE_PARAMS_VER };
	encoderParams.encodeConfig = &config;
	encoderParams.encodeGUID = codec;
	encoderParams.presetGUID = preset;
	encoderParams.tuningInfo = tuningInfo;
	encoderParams.encodeWidth = width;
	encoderParams.encodeHeight = height;
	encoderParams.darWidth = width;
	encoderParams.darHeight = height;
	encoderParams.maxEncodeWidth = width;
	encoderParams.maxEncodeHeight = height;
	encoderParams.frameRateNum = frameRate;
	encoderParams.frameRateDen = 1;
	encoderParams.enablePTD = 1;
	encoderParams.enableEncodeAsync = 0;

	std::cout << "[class NvidiaEncoder] ini encoder status " << nvencAPI.nvEncInitializeEncoder(encoder, &encoderParams) << "\n";

	std::cout << "[class NvidiaEncoder] create bitstream status " << nvencAPI.nvEncCreateBitstreamBuffer(encoder, &bitstream) << "\n";

	Renderer::getDevice()->QueryInterface(IID_ID3D11VideoDevice2, (void**)videoDevice.ReleaseAndGetAddressOf());
	ImCtx::GetContext()->QueryInterface(IID_ID3D11VideoContext3, (void**)videoContext.ReleaseAndGetAddressOf());

	D3D11_VIDEO_PROCESSOR_CONTENT_DESC contentDesc =
	{
		D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE,
		{frameRate,1},width,height,
		{frameRate,1},width,height,
		D3D11_VIDEO_USAGE_OPTIMAL_QUALITY
	};

	videoDevice->CreateVideoProcessorEnumerator(&contentDesc, videoProcessEnumerator.ReleaseAndGetAddressOf());

	videoDevice->CreateVideoProcessor(videoProcessEnumerator.Get(), 0, &videoProcessor);

	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC inputViewDesc = { 0,D3D11_VPIV_DIMENSION_TEXTURE2D,{0,0} };

	videoDevice->CreateVideoProcessorInputView(inputTexture2D, videoProcessEnumerator.Get(), &inputViewDesc, inputView.ReleaseAndGetAddressOf());

	D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC outputViewDesc = { D3D11_VPOV_DIMENSION_TEXTURE2D };

	videoDevice->CreateVideoProcessorOutputView(nv12Texture->getResource(), videoProcessEnumerator.Get(), &outputViewDesc, outputView.ReleaseAndGetAddressOf());

	std::cout << "[class NvidiaEncoder] render at " << width << " x " << height << "\n";
	std::cout << "[class NvidiaEncoder] frameRate " << frameRate << "\n";
	std::cout << "[class NvidiaEncoder] frameToEncode " << frameToEncode << "\n";
	std::cout << "[class NvidiaEncoder] start encoding\n";

	avformat_network_init();

	avformat_alloc_output_context2(&outCtx, nullptr, "mp4", nullptr);

	outCtx->url = av_strdup("output.mp4");

	outStream = avformat_new_stream(outCtx, nullptr);

	outStream->id = 0;

	AVCodecParameters* vpar = outStream->codecpar;
	vpar->codec_id = AV_CODEC_ID_HEVC;
	vpar->codec_type = AVMEDIA_TYPE_VIDEO;
	vpar->width = width;
	vpar->height = height;

	avio_open(&outCtx->pb, outCtx->url, AVIO_FLAG_WRITE);

	avformat_write_header(outCtx, nullptr);

	pkt = av_packet_alloc();
}

NvidiaEncoder::~NvidiaEncoder()
{
	if (moduleNvEncAPI)
	{
		nvencAPI.nvEncDestroyBitstreamBuffer(encoder, bitstream.bitstreamBuffer);
		nvencAPI.nvEncDestroyEncoder(encoder);
		delete nv12Texture;
		FreeLibrary(moduleNvEncAPI);

		av_packet_free(&pkt);
		av_write_trailer(outCtx);
		avio_close(outCtx->pb);
		avformat_free_context(outCtx);
	}
}
