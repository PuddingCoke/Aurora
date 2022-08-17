#pragma once

#ifndef _NVIDIAENCODER_H_
#define _NVIDIAENCODER_H_

#include<iostream>
#include<fstream>
#include<chrono>

#include<NvEnc/nvEncodeAPI.h>

#include<Aurora/Graphics.h>

class NvidiaEncoder
{
public:

	NvidiaEncoder() = delete;

	NvidiaEncoder(const NvidiaEncoder&) = delete;

	bool encode(ID3D11Texture2D* const encodeTexture);

	NvidiaEncoder(const UINT& frameToEncode, const UINT& frameRate,bool& initializeStatus);

	~NvidiaEncoder();

private:

	UINT frameEncoded;

	const UINT frameToEncode;

	bool encoding;

	void* encoder;

	NV_ENCODE_API_FUNCTION_LIST nvencAPI;

	NV_ENC_CREATE_BITSTREAM_BUFFER bitstream;

	FILE* stream;

	HMODULE moduleNvEncAPI;

	const GUID codec = NV_ENC_CODEC_H264_GUID;

	const GUID preset = NV_ENC_PRESET_P2_GUID;

	const GUID profile = NV_ENC_H264_PROFILE_HIGH_GUID;

	static constexpr NV_ENC_BUFFER_FORMAT pixelFormat = NV_ENC_BUFFER_FORMAT::NV_ENC_BUFFER_FORMAT_ARGB;

	const NV_ENC_TUNING_INFO tuningInfo = NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	float encodeTime;

};

#endif // !_NVIDIAENCODER_H_
