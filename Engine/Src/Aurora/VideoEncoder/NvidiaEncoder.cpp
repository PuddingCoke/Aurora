﻿#include<Aurora/VideoEncoder/NvidiaEncoder.h>

bool NvidiaEncoder::encode()
{
	timeStart = timer.now();

	D3D11_VIDEO_PROCESSOR_STREAM processorStream = { TRUE,0,0,0,0,NULL,inputView.Get(),NULL };

	videoContext->VideoProcessorBlt(videoProcessor.Get(), outputView.Get(), 0, 1, &processorStream);

	NV_ENC_REGISTER_RESOURCE registerResource = { NV_ENC_REGISTER_RESOURCE_VER };

	registerResource.bufferFormat = pixelFormat;

	registerResource.bufferUsage = NV_ENC_INPUT_IMAGE;

	registerResource.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;

	registerResource.resourceToRegister = nv12Texture.Get();

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

		_pclose(stream);

		std::cout << "[class NvidiaEncoder] encode complete!\n";

		std::cout << "[class NvidiaEncoder] frame encode avg speed " << frameToEncode / encodeTime << "\n";
	}
	else
	{
		picParams.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;

		picParams.inputBuffer = mapResource.mappedResource;

		picParams.bufferFmt = pixelFormat;

		picParams.inputWidth = width;

		picParams.inputHeight = height;

		picParams.outputBitstream = bitstream.bitstreamBuffer;

		nvencAPI.nvEncEncodePicture(encoder, &picParams);

		NV_ENC_LOCK_BITSTREAM lockBitstream = { NV_ENC_LOCK_BITSTREAM_VER };

		lockBitstream.outputBitstream = bitstream.bitstreamBuffer;

		lockBitstream.doNotWait = 0;

		nvencAPI.nvEncLockBitstream(encoder, &lockBitstream);

		fwrite(lockBitstream.bitstreamBufferPtr, lockBitstream.bitstreamSizeInBytes, 1, stream);

		nvencAPI.nvEncUnlockBitstream(encoder, lockBitstream.outputBitstream);
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
	frameToEncode(frameToEncode), frameEncoded(0u), encoding(true), encodeTime(0), width(width), height(height)
{
	nvencAPI = { NV_ENCODE_API_FUNCTION_LIST_VER };

	moduleNvEncAPI = LoadLibraryA("nvEncodeAPI64.dll");

	if (moduleNvEncAPI == 0)
	{
		std::cout << "[class NvidiaEncoder] load nvEncodeAPI64.dll failed\n";
		initializeStatus = false;
		return;
	}

	initializeStatus = true;

	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = width;
		tDesc.Height = height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT_NV12;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;

		Renderer::device->CreateTexture2D(&tDesc, nullptr, nv12Texture.ReleaseAndGetAddressOf());
	}

	typedef NVENCSTATUS(*APICreateInstance)(NV_ENCODE_API_FUNCTION_LIST*);

	APICreateInstance apiCreateInstance;

	apiCreateInstance = (APICreateInstance)GetProcAddress(moduleNvEncAPI, "NvEncodeAPICreateInstance");

	std::cout << "[class NvidiaEncoder] api instance create status " << apiCreateInstance(&nvencAPI) << "\n";

	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS sessionParams = { NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER };
	sessionParams.device = Renderer::device;
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
	config.rcParams.averageBitRate = 30000000U;
	config.rcParams.maxBitRate = 40000000U;
	config.gopLength = NVENC_INFINITE_GOPLENGTH;
	config.frameIntervalP = 1;

	config.encodeCodecConfig.h264Config.idrPeriod = NVENC_INFINITE_GOPLENGTH;
	config.rcParams.multiPass = NV_ENC_MULTI_PASS_DISABLED;
	config.rcParams.vbvBufferSize = config.rcParams.averageBitRate / frameRate * 5;
	config.rcParams.vbvInitialDelay = config.rcParams.vbvBufferSize;

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

	bitstream = { NV_ENC_CREATE_BITSTREAM_BUFFER_VER };

	std::cout << "[class NvidiaEncoder] create bitstream status " << nvencAPI.nvEncCreateBitstreamBuffer(encoder, &bitstream) << "\n";

	Renderer::device->QueryInterface(IID_ID3D11VideoDevice2, (void**)videoDevice.ReleaseAndGetAddressOf());
	Renderer::getContext()->QueryInterface(IID_ID3D11VideoContext3, (void**)videoContext.ReleaseAndGetAddressOf());

	D3D11_VIDEO_PROCESSOR_CONTENT_DESC contentDesc =
	{
		D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE,
		{1,1},width,height,
		{1,1},width,height,
		D3D11_VIDEO_USAGE_PLAYBACK_NORMAL
	};

	videoDevice->CreateVideoProcessorEnumerator(&contentDesc, videoProcessEnumerator.ReleaseAndGetAddressOf());

	videoDevice->CreateVideoProcessor(videoProcessEnumerator.Get(), 0, &videoProcessor);

	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC inputViewDesc = { 0,D3D11_VPIV_DIMENSION_TEXTURE2D,{0,0} };

	videoDevice->CreateVideoProcessorInputView(inputTexture2D, videoProcessEnumerator.Get(), &inputViewDesc, inputView.ReleaseAndGetAddressOf());

	D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC outputViewDesc = { D3D11_VPOV_DIMENSION_TEXTURE2D };

	videoDevice->CreateVideoProcessorOutputView(nv12Texture.Get(), videoProcessEnumerator.Get(), &outputViewDesc, outputView.ReleaseAndGetAddressOf());

	std::cout << "[class NvidiaEncoder] render at " << width << " x " << height << "\n";
	std::cout << "[class NvidiaEncoder] frameRate " << frameRate << "\n";
	std::cout << "[class NvidiaEncoder] frameToEncode " << frameToEncode << "\n";
	std::cout << "[class NvidiaEncoder] start encoding\n";

	stream = _popen("ffmpeg -y -f h264 -i pipe: -c copy output.mp4", "wb");

}

NvidiaEncoder::~NvidiaEncoder()
{
	if (moduleNvEncAPI)
	{
		nvencAPI.nvEncDestroyBitstreamBuffer(encoder, bitstream.bitstreamBuffer);
		nvencAPI.nvEncDestroyEncoder(encoder);
		FreeLibrary(moduleNvEncAPI);
	}
}
