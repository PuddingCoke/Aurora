#include<Aurora/EngineAPI/RenderAPI.h>

RenderAPI* RenderAPI::instance = nullptr;

Shader* RenderAPI::fullScreenVS = nullptr;
Shader* RenderAPI::fullScreenPS = nullptr;
Shader* RenderAPI::skyboxVS = nullptr;
Shader* RenderAPI::shadowVS = nullptr;
Shader* RenderAPI::randNoiseCS = nullptr;

RenderAPI::RenderAPI(const UINT& msaaLevel, ID3D11Texture2D* const renderTexture) :
	tempBuffer{}, tempStartConstants{}, tempNumConstants{}, tempRTV{}, tempUAV{}, tempSRV{}, tempOffsets{}, tempStrides{}, tempSampler{}
{
	instance = this;

	if (msaaLevel == 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		defRenderTargetView = new RenderOnlyRTV(renderTexture, rtvDesc);
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

		defRenderTargetView = new RenderOnlyRTV(Renderer::instance->msaaTexture.Get(), msaaViewDesc);
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

	for (UINT i = 0; RenderTargetView::curRTV[i]; i++)
	{
		RenderTargetView::curRTV[i]->boundOnRTV = false;
		RenderTargetView::curRTV[i] = nullptr;
	}

	if (rtvs.size())
	{
		std::initializer_list<RenderTargetView*>::iterator it = rtvs.begin();

		for (UINT i = 0; i < rtvs.size(); i++, it++)
		{
			RenderTargetView::curRTV[i] = it[0];

			tempRTV[i] = it[0]->renderTargetView.Get();

			it[0]->bindRTV();

			it[0]->boundOnRTV = true;
		}

		if (dsv)
		{
			dsv->bindDSV();
			Renderer::getContext()->OMSetRenderTargets((UINT)rtvs.size(), tempRTV, dsv->getDSV());
		}
		else
		{
			Renderer::getContext()->OMSetRenderTargets((UINT)rtvs.size(), tempRTV, nullptr);
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

	for (UINT i = 0; i < uavs.size(); i++, it++)
	{
		UnorderedAccessView::curPUAV[i] = it[0];

		tempUAV[i] = it[0]->unorderedAccessView.Get();

		it[0]->bindPUAV();

		it[0]->boundOnRTV = true;
	}

	Renderer::getContext()->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, (UINT)uavs.size(), tempUAV, nullptr);
}

void RenderAPI::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const UINT& slot)
{
	for (UINT i = slot; i < slot + uavs.size(); i++)
	{
		if (UnorderedAccessView::curCUAV[i])
		{
			UnorderedAccessView::curCUAV[i]->CUAVSlot = -1;
			UnorderedAccessView::curCUAV[i] = nullptr;
		}
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();
	for (UINT i = slot; i < slot + uavs.size(); i++, it++)
	{
		UnorderedAccessView::curCUAV[i] = it[0];
		tempUAV[i - slot] = it[0]->unorderedAccessView.Get();

		if (!it[0]->unbindFromCUAV() || !it[0]->unbindFromPUAV())
		{
			it[0]->bindCUAV();
		}

		it[0]->CUAVSlot = i;
	}

	Renderer::getContext()->CSSetUnorderedAccessViews(slot, (UINT)uavs.size(), tempUAV, nullptr);
}

void RenderAPI::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curVSRV[i])
		{
			ShaderResourceView::curVSRV[i]->VSSlot = -1;
			ShaderResourceView::curVSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
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

	Renderer::getContext()->VSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void RenderAPI::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curHSRV[i])
		{
			ShaderResourceView::curHSRV[i]->HSSlot = -1;
			ShaderResourceView::curHSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
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

	Renderer::getContext()->HSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void RenderAPI::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curDSRV[i])
		{
			ShaderResourceView::curDSRV[i]->DSSlot = -1;
			ShaderResourceView::curDSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
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

	Renderer::getContext()->DSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void RenderAPI::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curGSRV[i])
		{
			ShaderResourceView::curGSRV[i]->GSSlot = -1;
			ShaderResourceView::curGSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
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

	Renderer::getContext()->GSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void RenderAPI::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curPSRV[i])
		{
			ShaderResourceView::curPSRV[i]->PSSlot = -1;
			ShaderResourceView::curPSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
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

	Renderer::getContext()->PSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void RenderAPI::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (ShaderResourceView::curCSRV[i])
		{
			ShaderResourceView::curCSRV[i]->CSSlot = -1;
			ShaderResourceView::curCSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
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

	Renderer::getContext()->CSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void RenderAPI::IASetVertexBuffer(const UINT& slot, const std::initializer_list<VertexBuffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
{
	for (UINT i = slot; i < slot + (UINT)buffers.size(); i++)
	{
		if (VertexBuffer::curBuffer[i])
		{
			VertexBuffer::curBuffer[i]->IASlot = -1;
			VertexBuffer::curBuffer[i] = nullptr;
		}
	}

	std::initializer_list<VertexBuffer*>::iterator it = buffers.begin();
	std::initializer_list<UINT>::iterator itStride = strides.begin();
	std::initializer_list<UINT>::iterator itOffset = offsets.begin();

	for (UINT i = slot; i < slot + (UINT)buffers.size(); i++, it++, itStride++, itOffset++)
	{
		VertexBuffer::curBuffer[i] = it[0];
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStrides[i - slot] = itStride[0];
		tempOffsets[i - slot] = itOffset[0];

		if (!it[0]->unbindFromVertexBuffer())
		{
			it[0]->bindVertexBuffer();
		}

		it[0]->IASlot = i;
	}

	Renderer::getContext()->IASetVertexBuffers(slot, (UINT)buffers.size(), tempBuffer, tempStrides, tempOffsets);
}

void RenderAPI::VSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->VSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::HSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->HSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::DSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->DSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::GSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->GSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::PSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->PSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::CSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::getContext()->CSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void RenderAPI::VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->VSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void RenderAPI::HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->HSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void RenderAPI::DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->DSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void RenderAPI::GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->GSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void RenderAPI::PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->PSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void RenderAPI::CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::getContext()->CSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
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

void RenderAPI::RSSetViewport(const UINT& width, const UINT& height) const
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

void RenderAPI::ResolveSubresource(const Resource* const dstResource, const UINT& dstSubresource, const Resource* const srcResource, const UINT& srcSubresource, const DXGI_FORMAT& format)
{
	Renderer::getContext()->ResolveSubresource(dstResource->getResource(), dstSubresource, srcResource->getResource(), srcSubresource, format);
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

void RenderAPI::VSUnbindShader() const
{
	Renderer::getContext()->VSSetShader(nullptr, nullptr, 0);
}

void RenderAPI::HSUnbindShader() const
{
	Renderer::getContext()->HSSetShader(nullptr, nullptr, 0);
}

void RenderAPI::DSUnbindShader() const
{
	Renderer::getContext()->DSSetShader(nullptr, nullptr, 0);
}

void RenderAPI::GSUnbindShader() const
{
	Renderer::getContext()->GSSetShader(nullptr, nullptr, 0);
}

void RenderAPI::PSUnbindShader() const
{
	Renderer::getContext()->PSSetShader(nullptr, nullptr, 0);
}

void RenderAPI::CSUnbindShader() const
{
	Renderer::getContext()->CSSetShader(nullptr, nullptr, 0);
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

void RenderAPI::GenNoise(UnorderedAccessView* const uav, const UINT& textureWidth, const UINT& textureHeight)
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
	DSUnbindShader();
	HSUnbindShader();
	GSUnbindShader();

	RenderAPI::get()->DrawQuad();
}
