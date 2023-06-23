#pragma once

#include<Aurora/EngineAPI/RenderPass.h>

#include<Aurora/Effect/BloomEffect.h>

class PostProcessPass :public RenderPass
{
public:

	RenderTexture* originTexture;

	BloomEffect effect;

	PostProcessPass(RenderTexture* originTexture) :
		originTexture(new RenderTexture(*originTexture)),
		effect(context, Graphics::getWidth(), Graphics::getHeight())
	{
	}

	~PostProcessPass()
	{
		delete originTexture;
	}

	void recordCommand()
	{
		//pixel compute shader占用第一个槽位来获取跟时间相关的变量
		context->PSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);
		context->CSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);

		//vertex geometry hull domain shader占用前两个槽位来获取矩阵信息或者摄像头的信息
		context->VSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->HSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->DSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->GSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);

		context->OMSetDepthStencilState(States::defDepthStencilState, 0);
		context->RSSetState(States::rasterCullNone);

		ShaderResourceView* bloomSRV = effect.process(originTexture);

		context->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->OMSetBlendState(nullptr);

		context->ClearDefRTV(DirectX::Colors::Black);
		context->OMSetDefRTV(nullptr);

		context->PSSetSRV({ bloomSRV }, 0);
		context->PSSetSampler({ States::linearClampSampler }, 0);

		context->BindShader(Shader::fullScreenVS);
		context->BindShader(Shader::fullScreenPS);

		context->DrawQuad();
	}

};