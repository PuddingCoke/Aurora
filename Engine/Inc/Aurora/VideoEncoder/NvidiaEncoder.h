#pragma once

#ifndef _NVIDIAENCODER_H_
#define _NVIDIAENCODER_H_

#include<iostream>
#include<chrono>

#include<NvEnc/nvEncodeAPI.h>
#include<Aurora/Core/Renderer.h>
#include<Aurora/Core/DX/Resource/Texture2D.h>

extern "C"
{
#include<libavutil/avutil.h>
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
}

class NvidiaEncoder
{
public:

	NvidiaEncoder() = delete;

	NvidiaEncoder(const NvidiaEncoder&) = delete;

	bool encode();

	NvidiaEncoder(const UINT& width, const UINT& height, const UINT& frameToEncode, const UINT& frameRate, ID3D11Resource* const inputTexture2D, bool& initializeStatus);

	~NvidiaEncoder();

private:

	const UINT width;

	const UINT height;

	UINT frameEncoded;

	const UINT frameToEncode;

	const UINT frameRate;

	bool encoding;

	void* encoder;

	ComPtr<ID3D11VideoDevice2> videoDevice;

	ComPtr<ID3D11VideoContext3> videoContext;

	ComPtr<ID3D11VideoProcessor> videoProcessor;

	ComPtr<ID3D11VideoProcessorInputView> inputView;

	ComPtr<ID3D11VideoProcessorOutputView> outputView;

	ComPtr<ID3D11VideoProcessorEnumerator> videoProcessEnumerator;

	NV_ENCODE_API_FUNCTION_LIST nvencAPI;

	NV_ENC_CREATE_BITSTREAM_BUFFER bitstream;

	Texture2D* nv12Texture;

	HMODULE moduleNvEncAPI;

	const GUID codec = NV_ENC_CODEC_HEVC_GUID;

	const GUID preset = NV_ENC_PRESET_P7_GUID;

	const GUID profile = NV_ENC_HEVC_PROFILE_MAIN_GUID;

	const NV_ENC_TUNING_INFO tuningInfo = NV_ENC_TUNING_INFO_HIGH_QUALITY;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	float encodeTime;

	AVFormatContext* outCtx;

	AVStream* outStream;

	AVPacket* pkt;
};

#endif // !_NVIDIAENCODER_H_
