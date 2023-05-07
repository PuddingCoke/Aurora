#include<Aurora/Core/RenderAPI.h>

RenderAPI* RenderAPI::instance = nullptr;

Shader* RenderAPI::fullScreenVS = nullptr;
Shader* RenderAPI::fullScreenPS = nullptr;
Shader* RenderAPI::skyboxVS = nullptr;
Shader* RenderAPI::shadowVS = nullptr;
Shader* RenderAPI::randNoiseCS = nullptr;

RenderAPI::RenderAPI(const unsigned int& msaaLevel, ID3D11Texture2D* const renderTexture):
	tempBuffer{}, tempStartConstants{}, tempNumConstants{}, tempRTV{}, tempUAV{}, tempSRV{}, tempOffsets{}, tempStrides{}, tempSampler{}
{
	instance = this;

	if (msaaLevel == 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		defRenderTargetView = new CustomedRTV(renderTexture, rtvDesc);
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

		defRenderTargetView = new CustomedRTV(Renderer::instance->msaaTexture.Get(), msaaViewDesc);
	}

	std::cout << "fullScreenVS ";
	fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes), ShaderType::Vertex);
	std::cout << "fullScreenPS ";
	fullScreenPS = new Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes), ShaderType::Pixel);
	std::cout << "skyboxVS ";
	skyboxVS = new Shader(g_SkyboxVSBytes, sizeof(g_SkyboxVSBytes), ShaderType::Vertex);
	std::cout << "shadowVS ";
	shadowVS = new Shader(g_ShadowVSBytes, sizeof(g_ShadowVSBytes), ShaderType::Vertex);
	std::cout << "randNoiseCS ";
	randNoiseCS = new Shader(g_RandNoiseCSBytes, sizeof(g_RandNoiseCSBytes), ShaderType::Compute);
}

RenderAPI* RenderAPI::get()
{
	return instance;
}

RenderAPI::~RenderAPI()
{
	delete defRenderTargetView;
	delete fullScreenVS;
	delete fullScreenPS;
	delete skyboxVS;
	delete shadowVS;
	delete randNoiseCS;
}

void RenderAPI::ClearDefRTV(const float* const color) const
{
	defRenderTargetView->clearRTV(color);
}

void RenderAPI::OMSetDefRTV(DepthStencilView* const dsv)
{
	OMSetRTV({ defRenderTargetView }, dsv);
}

void RenderAPI::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv)
{
	if (UnorderedAccessView::curPUAV[0])
	{
		UnorderedAccessView::unbindPUAV();
	}

	for (unsigned int i = 0; RenderTargetView::curRTV[i]; i++)
	{
		RenderTargetView::curRTV[i]->boundOnRTV = false;
		RenderTargetView::curRTV[i] = nullptr;
	}

	if (rtvs.size())
	{
		std::initializer_list<RenderTargetView*>::iterator it = rtvs.begin();

		for (unsigned int i = 0; i < rtvs.size(); i++, it++)
		{
			RenderTargetView::curRTV[i] = it[0];

			tempRTV[i] = it[0]->renderTargetView.Get();

			it[0]->bindRTV();

			it[0]->boundOnRTV = true;
		}

		if (dsv)
		{
			dsv->bindDSV();
			Renderer::getContext()->OMSetRenderTargets((unsigned int)rtvs.size(), tempRTV, dsv->getDSV());
		}
		else
		{
			Renderer::getContext()->OMSetRenderTargets((unsigned int)rtvs.size(), tempRTV, nullptr);
		}
	}
	else
	{
		dsv->bindDSV();
		Renderer::getContext()->OMSetRenderTargets(0, nullptr, dsv->getDSV());
	}

}

void RenderAPI::OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs)
{
	if (UnorderedAccessView::curPUAV[0])
	{
		UnorderedAccessView::unbindPUAV();
	}
	if (RenderTargetView::curRTV[0])
	{
		RenderTargetView::unbindRTV();
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();

	for (unsigned int i = 0; i < uavs.size(); i++, it++)
	{
		UnorderedAccessView::curPUAV[i] = it[0];

		tempUAV[i] = it[0]->unorderedAccessView.Get();

		it[0]->bindPUAV();

		it[0]->boundOnRTV = true;
	}

	Renderer::getContext()->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, (unsigned int)uavs.size(), tempUAV, nullptr);
}

void RenderAPI::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + uavs.size(); i++)
	{
		if (UnorderedAccessView::curCUAV[i])
		{
			UnorderedAccessView::curCUAV[i]->CUAVSlot = -1;
			UnorderedAccessView::curCUAV[i] = nullptr;
		}
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();
	for (unsigned int i = slot; i < slot + uavs.size(); i++, it++)
	{
		UnorderedAccessView::curCUAV[i] = it[0];
		tempUAV[i - slot] = it[0]->unorderedAccessView.Get();

		if (!it[0]->unbindFromCUAV() || !it[0]->unbindFromPUAV())
		{
			it[0]->bindCUAV();
		}

		it[0]->CUAVSlot = i;
	}

	Renderer::getContext()->CSSetUnorderedAccessViews(slot, (unsigned int)uavs.size(), tempUAV, nullptr);
}

void RenderAPI::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curVSRV[i])
		{
			ShaderResourceView::curVSRV[i]->VSSlot = -1;
			ShaderResourceView::curVSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (unsigned int i = slot; i < slot + srvs.size(); i++, it++)
	{
		ShaderResourceView::curVSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->VSSlot != -1)
		{
			it[0]->unbindVSRV();
		}
		else
		{
			it[0]->bindSRV();
		}

		it[0]->VSSlot = i;
	}

	Renderer::getContext()->VSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void RenderAPI::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curHSRV[i])
		{
			ShaderResourceView::curHSRV[i]->HSSlot = -1;
			ShaderResourceView::curHSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (unsigned int i = slot; i < slot + srvs.size(); i++, it++)
	{
		ShaderResourceView::curHSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->HSSlot != -1)
		{
			it[0]->unbindHSRV();
		}
		else
		{
			it[0]->bindSRV();
		}

		it[0]->HSSlot = i;
	}

	Renderer::getContext()->HSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void RenderAPI::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curDSRV[i])
		{
			ShaderResourceView::curDSRV[i]->DSSlot = -1;
			ShaderResourceView::curDSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (unsigned int i = slot; i < slot + srvs.size(); i++, it++)
	{
		ShaderResourceView::curDSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->DSSlot != -1)
		{
			it[0]->unbindDSRV();
		}
		else
		{
			it[0]->bindSRV();
		}

		it[0]->DSSlot = i;
	}

	Renderer::getContext()->DSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void RenderAPI::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curGSRV[i])
		{
			ShaderResourceView::curGSRV[i]->GSSlot = -1;
			ShaderResourceView::curGSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (unsigned int i = slot; i < slot + srvs.size(); i++, it++)
	{
		ShaderResourceView::curGSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->GSSlot != -1)
		{
			it[0]->unbindGSRV();
		}
		else
		{
			it[0]->bindSRV();
		}

		it[0]->GSSlot = i;
	}

	Renderer::getContext()->GSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void RenderAPI::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curPSRV[i])
		{
			ShaderResourceView::curPSRV[i]->PSSlot = -1;
			ShaderResourceView::curPSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (unsigned int i = slot; i < slot + srvs.size(); i++, it++)
	{
		ShaderResourceView::curPSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->PSSlot != -1)
		{
			it[0]->unbindPSRV();
		}
		else
		{
			it[0]->bindSRV();
		}

		it[0]->PSSlot = i;
	}

	Renderer::getContext()->PSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void RenderAPI::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	for (unsigned int i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curCSRV[i])
		{
			ShaderResourceView::curCSRV[i]->CSSlot = -1;
			ShaderResourceView::curCSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (unsigned int i = slot; i < slot + srvs.size(); i++, it++)
	{
		ShaderResourceView::curCSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->CSSlot != -1)
		{
			it[0]->unbindCSRV();
		}
		else
		{
			it[0]->bindSRV();
		}

		it[0]->CSSlot = i;
	}

	Renderer::getContext()->CSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void RenderAPI::IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
{
	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++)
	{
		if (Buffer::curBuffer[i])
		{
			Buffer::curBuffer[i]->IASlot = -1;
			Buffer::curBuffer[i] = nullptr;
		}
	}

	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	std::initializer_list<UINT>::iterator itStride = strides.begin();
	std::initializer_list<UINT>::iterator itOffset = offsets.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++, itStride++, itOffset++)
	{
		Buffer::curBuffer[i] = it[0];
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStrides[i - slot] = itStride[0];
		tempOffsets[i - slot] = itOffset[0];

		if (!it[0]->unbindFromVertexBuffer())
		{
			it[0]->bindVertexBuffer();
		}

		it[0]->IASlot = i;
	}

	Renderer::getContext()->IASetVertexBuffers(slot, (unsigned int)buffers.size(), tempBuffer, tempStrides, tempOffsets);
}

void RenderAPI::VSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->VSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::HSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->HSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::DSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->DSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::GSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->GSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::PSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->PSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::CSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->CSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->VSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void RenderAPI::HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->HSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void RenderAPI::DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->DSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void RenderAPI::GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->GSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void RenderAPI::PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->PSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void RenderAPI::CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->CSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void RenderAPI::IASetInputLayout(ID3D11InputLayout* const layout) const
{
	Renderer::getContext()->IASetInputLayout(layout);
}

void RenderAPI::IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset) const
{
	Renderer::getContext()->IASetIndexBuffer(buffer->getBuffer(), format, offset);
}

void RenderAPI::RSSetViewport(const float& width, const float& height) const
{
	Renderer::instance->vp.Width = width;
	Renderer::instance->vp.Height = height;
	Renderer::getContext()->RSSetViewports(1, &Renderer::instance->vp);
}

void RenderAPI::RSSetViewport(const unsigned int& width, const unsigned int& height) const
{
	RSSetViewport((float)width, (float)height);
}

void RenderAPI::RSSetViewport(const int& width, const int& height) const
{
	RSSetViewport((float)width, (float)height);
}

void RenderAPI::IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology) const
{
	Renderer::getContext()->IASetPrimitiveTopology(topology);
}

void RenderAPI::OMSetBlendState(ID3D11BlendState* const state) const
{
	Renderer::getContext()->OMSetBlendState(state, nullptr, 0xFFFFFFFF);
}

void RenderAPI::RSSetState(ID3D11RasterizerState* const state) const
{
	Renderer::getContext()->RSSetState(state);
}

void RenderAPI::OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef) const
{
	Renderer::getContext()->OMSetDepthStencilState(state, stencilRef);
}

void RenderAPI::DrawQuad() const
{
	Renderer::getContext()->Draw(3, 0);
}

void RenderAPI::DrawCube() const
{
	Renderer::getContext()->Draw(36, 0);
}

void RenderAPI::Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ) const
{
	Renderer::getContext()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void RenderAPI::Draw(const UINT& vertexCount, const UINT& startVertexLocation) const
{
	Renderer::getContext()->Draw(vertexCount, startVertexLocation);
}

void RenderAPI::DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation) const
{
	Renderer::getContext()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void RenderAPI::DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation) const
{
	Renderer::getContext()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void RenderAPI::VSSetShader(ID3D11VertexShader* const shader) const
{
	Renderer::getContext()->VSSetShader(shader, nullptr, 0);
}

void RenderAPI::HSSetShader(ID3D11HullShader* const shader) const
{
	Renderer::getContext()->HSSetShader(shader, nullptr, 0);
}

void RenderAPI::DSSetShader(ID3D11DomainShader* const shader) const
{
	Renderer::getContext()->DSSetShader(shader, nullptr, 0);
}

void RenderAPI::GSSetShader(ID3D11GeometryShader* const shader) const
{
	Renderer::getContext()->GSSetShader(shader, nullptr, 0);
}

void RenderAPI::PSSetShader(ID3D11PixelShader* const shader) const
{
	Renderer::getContext()->PSSetShader(shader, nullptr, 0);
}

void RenderAPI::CSSetShader(ID3D11ComputeShader* const shader) const
{
	Renderer::getContext()->CSSetShader(shader, nullptr, 0);
}

void RenderAPI::UnbindRTV() const
{
	RenderTargetView::unbindRTV();
}

void RenderAPI::UnbindCSUAV() const
{
	UnorderedAccessView::unbindCUAV();
}

void RenderAPI::UnbindPSUAV() const
{
	UnorderedAccessView::unbindPUAV();
}

void RenderAPI::GenNoise(UnorderedAccessView* const uav, const unsigned int& textureWidth, const unsigned int& textureHeight)
{
	randNoiseCS->use();
	CSSetUAV({ uav }, 0);
	RenderAPI::get()->Dispatch(textureWidth / 32, textureHeight / 18, 1);
}

void RenderAPI::DebugDraw(ShaderResourceView* const srv)
{
	IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	OMSetBlendState(nullptr);

	ClearDefRTV(DirectX::Colors::Black);
	OMSetDefRTV(nullptr);
	RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	PSSetSRV({ srv }, 0);
	PSSetSampler({ States::pointClampSampler }, 0);

	fullScreenVS->use();
	fullScreenPS->use();
	DSSetShader(nullptr);
	HSSetShader(nullptr);
	GSSetShader(nullptr);

	RenderAPI::get()->DrawQuad();
}
