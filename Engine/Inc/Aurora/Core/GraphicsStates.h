#pragma once

#include<Aurora/Core/DX/View/RenderTargetView.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>
#include<Aurora/Core/DX/View/UnorderedAccessView.h>
#include<Aurora/Resource/VertexBuffer.h>

class RenderTargetView;
class ShaderResourceView;
class UnorderedAccessView;
class VertexBuffer;

struct GraphicsStates
{
	RenderTargetView* curRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ShaderResourceView* curVSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ShaderResourceView* curHSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ShaderResourceView* curDSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ShaderResourceView* curGSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ShaderResourceView* curPSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ShaderResourceView* curCSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	UnorderedAccessView* curCUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	UnorderedAccessView* curPUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	VertexBuffer* curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	std::list<ShaderResourceView*> managedSRV;

	void resetStates();

};