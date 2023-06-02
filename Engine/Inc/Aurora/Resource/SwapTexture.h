#pragma once

#ifndef _SWAPTEXTURE_H_
#define _SWAPTEXTURE_H_

#include<functional>

template<typename ResourceType>
class SwapTexture
{
private:

	ResourceType* texture0;

	ResourceType* texture1;

public:

	SwapTexture(const std::function<ResourceType* (void)>& factoryMethod);

	~SwapTexture();

	ResourceType* read() const;

	ResourceType* write() const;

	void swap();

	const UINT width;

	const UINT height;

};

template<typename ResourceType>
inline SwapTexture<ResourceType>::SwapTexture(const std::function<ResourceType* (void)>& factoryMethod) :
	texture0(factoryMethod()), texture1(factoryMethod()), width(texture0->getWidth()), height(texture0->getHeight())
{
}

template<typename ResourceType>
inline SwapTexture<ResourceType>::~SwapTexture()
{
	delete texture0;
	delete texture1;
}

template<typename ResourceType>
inline ResourceType* SwapTexture<ResourceType>::read() const
{
	return texture0;
}

template<typename ResourceType>
inline ResourceType* SwapTexture<ResourceType>::write() const
{
	return texture1;
}

template<typename ResourceType>
inline void SwapTexture<ResourceType>::swap()
{
	ResourceType* const temp = texture0;
	texture0 = texture1;
	texture1 = temp;
}

#endif // !_SWAPTEXTURE_H_
