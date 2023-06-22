#include<Aurora/Core/GraphicsStates.h>

void GraphicsStates::resetStates()
{
	memset(curRTV, 0, sizeof(RenderTargetView*) * D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	memset(curVSRV, 0, sizeof(ShaderResourceView*) * D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
	memset(curHSRV, 0, sizeof(ShaderResourceView*) * D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
	memset(curDSRV, 0, sizeof(ShaderResourceView*) * D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
	memset(curGSRV, 0, sizeof(ShaderResourceView*) * D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
	memset(curPSRV, 0, sizeof(ShaderResourceView*) * D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
	memset(curCSRV, 0, sizeof(ShaderResourceView*) * D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
	memset(curCUAV, 0, sizeof(UnorderedAccessView*) * D3D11_PS_CS_UAV_REGISTER_COUNT);
	memset(curPUAV, 0, sizeof(UnorderedAccessView*) * D3D11_PS_CS_UAV_REGISTER_COUNT);
	memset(curBuffer, 0, sizeof(VertexBuffer*) * D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
}