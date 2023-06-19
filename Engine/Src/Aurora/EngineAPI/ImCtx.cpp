#include<Aurora/EngineAPI/ImCtx.h>

ImCtx* ImCtx::instance = nullptr;

Shader* ImCtx::fullScreenVS = nullptr;
Shader* ImCtx::fullScreenPS = nullptr;
Shader* ImCtx::skyboxVS = nullptr;
Shader* ImCtx::shadowVS = nullptr;
Shader* ImCtx::randNoiseCS = nullptr;

ImCtx::ImCtx(const UINT& msaaLevel, ID3D11Texture2D* const renderTexture) :
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

ImCtx* ImCtx::get()
{
	return instance;
}

ImCtx::~ImCtx()
{
	delete defRenderTargetView;
	delete fullScreenVS;
	delete fullScreenPS;
	delete skyboxVS;
	delete shadowVS;
	delete randNoiseCS;
}

void ImCtx::ClearDefRTV(const float* const color) const
{
	ClearRTV(defRenderTargetView, color);
}

void ImCtx::OMSetDefRTV(DepthStencilView* const dsv)
{
	OMSetRTV({ defRenderTargetView }, dsv);
}

void ImCtx::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv)
{
	if (UnorderedAccessView::curPUAV[0])
	{
		UnorderedAccessView::unbindPUAV(GetContext());
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

			it[0]->bindRTV(GetContext());

			it[0]->boundOnRTV = true;
		}

		if (dsv)
		{
			dsv->bindDSV(GetContext());
			GetContext()->OMSetRenderTargets((UINT)rtvs.size(), tempRTV, dsv->getDSV());
		}
		else
		{
			GetContext()->OMSetRenderTargets((UINT)rtvs.size(), tempRTV, nullptr);
		}
	}
	else
	{
		dsv->bindDSV(GetContext());
		GetContext()->OMSetRenderTargets(0, nullptr, dsv->getDSV());
	}

}

void ImCtx::OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs)
{
	if (UnorderedAccessView::curPUAV[0])
	{
		UnorderedAccessView::unbindPUAV(GetContext());
	}
	if (RenderTargetView::curRTV[0])
	{
		RenderTargetView::unbindRTV(GetContext());
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();

	for (UINT i = 0; i < uavs.size(); i++, it++)
	{
		UnorderedAccessView::curPUAV[i] = it[0];

		tempUAV[i] = it[0]->unorderedAccessView.Get();

		it[0]->bindPUAV(GetContext());

		it[0]->boundOnRTV = true;
	}

	GetContext()->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, (UINT)uavs.size(), tempUAV, nullptr);
}

void ImCtx::BindShader(Shader* const shader) const
{
	shader->use(GetContext());
}

void ImCtx::ClearDSV(DepthStencilView* dsv, const UINT& clearFlag, const float& depth, const UINT8& stencil) const
{
	GetContext()->ClearDepthStencilView(dsv->getDSV(), clearFlag, depth, stencil);
}

void ImCtx::ClearRTV(RenderTargetView* rtv, const float* const color) const
{
	GetContext()->ClearRenderTargetView(rtv->getRTV(), color);
}

void ImCtx::ClearUAV(UnorderedAccessView* uav, const float* const color) const
{
	GetContext()->ClearUnorderedAccessViewFloat(uav->getUAV(), color);
}

void ImCtx::ClearUAV(UnorderedAccessView* uav, const unsigned int* const value) const
{
	GetContext()->ClearUnorderedAccessViewUint(uav->getUAV(), value);
}

void ImCtx::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const UINT& slot)
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

		if (!it[0]->unbindFromCUAV(GetContext()) || !it[0]->unbindFromPUAV(GetContext()))
		{
			it[0]->bindCUAV(GetContext());
		}

		it[0]->CUAVSlot = i;
	}

	GetContext()->CSSetUnorderedAccessViews(slot, (UINT)uavs.size(), tempUAV, nullptr);
}

void ImCtx::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
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
			it[0]->unbindVSRV(GetContext());
		}
		else
		{
			it[0]->bindSRV(GetContext());
		}

		it[0]->VSSlot = i;
	}

	GetContext()->VSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
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
			it[0]->unbindHSRV(GetContext());
		}
		else
		{
			it[0]->bindSRV(GetContext());
		}

		it[0]->HSSlot = i;
	}

	GetContext()->HSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
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
			it[0]->unbindDSRV(GetContext());
		}
		else
		{
			it[0]->bindSRV(GetContext());
		}

		it[0]->DSSlot = i;
	}

	GetContext()->DSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
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
			it[0]->unbindGSRV(GetContext());
		}
		else
		{
			it[0]->bindSRV(GetContext());
		}

		it[0]->GSSlot = i;
	}

	GetContext()->GSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
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
			it[0]->unbindPSRV(GetContext());
		}
		else
		{
			it[0]->bindSRV(GetContext());
		}

		it[0]->PSSlot = i;
	}

	GetContext()->PSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
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
			it[0]->unbindCSRV(GetContext());
		}
		else
		{
			it[0]->bindSRV(GetContext());
		}

		it[0]->CSSlot = i;
	}

	GetContext()->CSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::IASetVertexBuffer(const UINT& slot, const std::initializer_list<VertexBuffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
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

		if (!it[0]->unbindFromVertexBuffer(GetContext()))
		{
			it[0]->bindVertexBuffer(GetContext());
		}

		it[0]->IASlot = i;
	}

	GetContext()->IASetVertexBuffers(slot, (UINT)buffers.size(), tempBuffer, tempStrides, tempOffsets);
}

void ImCtx::VSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	GetContext()->VSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::HSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	GetContext()->HSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::DSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	GetContext()->DSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::GSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	GetContext()->GSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::PSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	GetContext()->PSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::CSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot)
{
	std::initializer_list<ConstantBuffer*>::iterator it = constantBuffers.begin();

	for (UINT i = slot; i < slot + (UINT)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	GetContext()->CSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	GetContext()->VSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	GetContext()->HSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	GetContext()->DSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	GetContext()->GSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	GetContext()->PSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	GetContext()->CSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::IASetInputLayout(ID3D11InputLayout* const layout) const
{
	GetContext()->IASetInputLayout(layout);
}

void ImCtx::IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset) const
{
	GetContext()->IASetIndexBuffer(buffer->getBuffer(), format, offset);
}

void ImCtx::RSSetViewport(const float& width, const float& height) const
{
	Renderer::instance->vp.Width = width;
	Renderer::instance->vp.Height = height;
	GetContext()->RSSetViewports(1, &Renderer::instance->vp);
}

void ImCtx::RSSetViewport(const UINT& width, const UINT& height) const
{
	RSSetViewport((float)width, (float)height);
}

void ImCtx::RSSetViewport(const int& width, const int& height) const
{
	RSSetViewport((float)width, (float)height);
}

void ImCtx::IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology) const
{
	GetContext()->IASetPrimitiveTopology(topology);
}

void ImCtx::OMSetBlendState(ID3D11BlendState* const state) const
{
	GetContext()->OMSetBlendState(state, nullptr, 0xFFFFFFFF);
}

void ImCtx::RSSetState(ID3D11RasterizerState* const state) const
{
	GetContext()->RSSetState(state);
}

void ImCtx::OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef) const
{
	GetContext()->OMSetDepthStencilState(state, stencilRef);
}

void ImCtx::ResolveSubresource(const Resource* const dstResource, const UINT& dstSubresource, const Resource* const srcResource, const UINT& srcSubresource, const DXGI_FORMAT& format)
{
	GetContext()->ResolveSubresource(dstResource->getResource(), dstSubresource, srcResource->getResource(), srcSubresource, format);
}

void ImCtx::DrawQuad() const
{
	GetContext()->Draw(3, 0);
}

void ImCtx::DrawCube() const
{
	GetContext()->Draw(36, 0);
}

void ImCtx::Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ) const
{
	GetContext()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ImCtx::Draw(const UINT& vertexCount, const UINT& startVertexLocation) const
{
	GetContext()->Draw(vertexCount, startVertexLocation);
}

void ImCtx::DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation) const
{
	GetContext()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void ImCtx::DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation) const
{
	GetContext()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

D3D11_MAPPED_SUBRESOURCE ImCtx::Map(Resource* const res, const UINT& subresource, const D3D11_MAP& mapType, const UINT& mapFlag) const
{
	D3D11_MAPPED_SUBRESOURCE data = {};

	GetContext()->Map(res->getResource(), subresource, mapType, mapFlag, &data);

	return data;
}

void ImCtx::Unmap(Resource* const res, const UINT& subresource) const
{
	GetContext()->Unmap(res->getResource(), subresource);
}

void ImCtx::UpdateSubresource(Resource* res, const UINT& dstSubresource, const D3D11_BOX* const pDstBox, const void* const data, const UINT& rowPitch, const UINT& depthPitch)
{
	GetContext()->UpdateSubresource(res->getResource(), dstSubresource, pDstBox, data, rowPitch, depthPitch);
}

void ImCtx::GenerateMips(ShaderResourceView* const srv)
{
	GetContext()->GenerateMips(srv->getSRV());
}

void ImCtx::CopySubresourceRegion(Resource* const pDstResource, const UINT& DstSubresource, const UINT& DstX, const UINT& DstY, const UINT& DstZ, Resource* const pSrcResource, const UINT& SrcSubresource, const D3D11_BOX* const pSrcBox)
{
	GetContext()->CopySubresourceRegion(pDstResource->getResource(), DstSubresource, DstX, DstY, DstZ, pSrcResource->getResource(), SrcSubresource, pSrcBox);
}

void ImCtx::VSUnbindShader() const
{
	GetContext()->VSSetShader(nullptr, nullptr, 0);
}

void ImCtx::HSUnbindShader() const
{
	GetContext()->HSSetShader(nullptr, nullptr, 0);
}

void ImCtx::DSUnbindShader() const
{
	GetContext()->DSSetShader(nullptr, nullptr, 0);
}

void ImCtx::GSUnbindShader() const
{
	GetContext()->GSSetShader(nullptr, nullptr, 0);
}

void ImCtx::PSUnbindShader() const
{
	GetContext()->PSSetShader(nullptr, nullptr, 0);
}

void ImCtx::CSUnbindShader() const
{
	GetContext()->CSSetShader(nullptr, nullptr, 0);
}

void ImCtx::UnbindRTV() const
{
	RenderTargetView::unbindRTV(GetContext());
}

void ImCtx::UnbindCSUAV() const
{
	UnorderedAccessView::unbindCUAV(GetContext());
}

void ImCtx::UnbindPSUAV() const
{
	UnorderedAccessView::unbindPUAV(GetContext());
}

void ImCtx::GenNoise(UnorderedAccessView* const uav, const UINT& textureWidth, const UINT& textureHeight)
{
	ImCtx::get()->BindShader(randNoiseCS);
	CSSetUAV({ uav }, 0);
	ImCtx::get()->Dispatch(textureWidth / 32, textureHeight / 18, 1);
}

void ImCtx::DebugDraw(ShaderResourceView* const srv)
{
	IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	OMSetBlendState(nullptr);

	ClearDefRTV(DirectX::Colors::Black);
	OMSetDefRTV(nullptr);
	RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	PSSetSRV({ srv }, 0);
	PSSetSampler({ States::pointClampSampler }, 0);

	ImCtx::get()->BindShader(fullScreenVS);
	ImCtx::get()->BindShader(fullScreenPS);
	DSUnbindShader();
	HSUnbindShader();
	GSUnbindShader();

	ImCtx::get()->DrawQuad();
}

ID3D11DeviceContext4* ImCtx::GetContext()
{
	return get()->imctx.Get();
}
