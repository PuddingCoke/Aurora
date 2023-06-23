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
		//pixel compute shaderռ�õ�һ����λ����ȡ��ʱ����صı���
		context->PSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);
		context->CSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);

		//vertex geometry hull domain shaderռ��ǰ������λ����ȡ������Ϣ��������ͷ����Ϣ
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