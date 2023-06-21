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

	virtual void unbindResource() = 0;

	void finishRecord();

	ID3D11CommandList* getCommandList() const;

protected:

	GraphicsContext* context;

	ComPtr<ID3D11CommandList> commandList;

};

#endif // !_RENDERPASS_H_
