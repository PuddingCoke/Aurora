#pragma once

#ifndef _RESOURCEFORMAT_H_
#define _RESOURCEFORMAT_H_

#include<d3d11_4.h>
#include<unordered_map>

class ResourceFormat
{
public:

	ResourceFormat() = delete;

	ResourceFormat(const ResourceFormat&) = delete;

	void operator=(const ResourceFormat&) = delete;

	enum FMT
	{
		//Int
		R8I,
		RG8I,
		RGBA8I,

		R16I,
		RG16I,
		RGBA16I,

		R32I,
		RG32I,
		RGBA32I,

		//Uint
		R8UI,
		RG8UI,
		RGBA8UI,

		R16UI,
		RG16UI,
		RGBA16UI,

		R32UI,
		RG32UI,
		RGBA32UI,

		//Unorm
		R8,
		RG8,
		RGBA8,

		R16,
		RG16,
		RGBA16,

		//Float
		R16F,
		RG16F,
		RGBA16F,

		R32F,
		RG32F,
		RGBA32F,

		RG11B10F,

		//Type Less
		R8TL,
		RG8TL,
		RGBA8TL,

		R16TL,
		RG16TL,
		RGBA16TL,

		R32TL,
		RG32TL,
		RGBA32TL,

		//Depth
		D16,
		D32F,

		//Special
		NV12,
	};

	static DXGI_FORMAT cast(const FMT& fmt)
	{
		return formatMap.at(fmt);
	}

private:

	static const std::unordered_map<FMT, DXGI_FORMAT> formatMap;

};

using FMT = ResourceFormat::FMT;

#define FMTCAST(f) ResourceFormat::cast(f)

#endif // !_RESOURCEFORMAT_H_
