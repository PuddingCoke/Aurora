#pragma once

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include<Aurora/Renderer.h>

class Resource
{
public:

	virtual ID3D11Resource* getResource() const = 0;

};

#endif // !_RESOURCE_H_
