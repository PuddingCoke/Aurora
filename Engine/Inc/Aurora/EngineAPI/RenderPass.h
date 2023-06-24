#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<future>

#include<Aurora/Core/GraphicsContext.h>

class RenderPass
{
public:

	RenderPass();

	~RenderPass();

	std::future<ID3D11CommandList*> GetPassResult();

protected:

	virtual void recordCommand() = 0;

	ID3D11CommandList* finishRecord();

	GraphicsContext* context;

};

#endif // !_RENDERPASS_H_
