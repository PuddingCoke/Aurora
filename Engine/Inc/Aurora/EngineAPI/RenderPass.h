#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Aurora/Core/GraphicsContext.h>

class RenderPass
{
public:

	RenderPass();

	~RenderPass();

	virtual void recordCommand() = 0;

	ID3D11CommandList* finishRecord();

protected:

	GraphicsContext* context;

};

#endif // !_RENDERPASS_H_
