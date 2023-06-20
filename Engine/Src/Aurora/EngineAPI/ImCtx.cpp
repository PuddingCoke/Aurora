#include<Aurora/EngineAPI/ImCtx.h>

ImCtx* ImCtx::instance = nullptr;
RenderOnlyRTV* ImCtx::defRenderTargetView = nullptr;

ImCtx::ImCtx() :
	tempBuffer{}, tempStartConstants{}, tempNumConstants{}, tempRTV{}, tempUAV{}, tempSRV{}, tempOffsets{}, tempStrides{}, tempSampler{},
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f }, states{}
{
	instance = this;
}

ImCtx* ImCtx::get()
{
	return instance;
}

ImCtx::~ImCtx()
{
	delete defRenderTargetView;
}

void ImCtx::ClearDefRTV(const float* const color)
{
	ClearRTV(defRenderTargetView, color);
}

void ImCtx::OMSetDefRTV(DepthStencilView* const dsv)
{
	OMSetRTV({ defRenderTargetView }, dsv);
}

void ImCtx::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv)
{
	if (states.curPUAV[0])
	{
		UnorderedAccessView::unbindPUAV(getContext(), &states);
	}

	for (UINT i = 0; states.curRTV[i]; i++)
	{
		states.curRTV[i]->boundOnRTV = false;
		states.curRTV[i] = nullptr;
	}

	if (rtvs.size())
	{
		std::initializer_list<RenderTargetView*>::iterator it = rtvs.begin();

		for (UINT i = 0; i < rtvs.size(); i++, it++)
		{
			states.curRTV[i] = it[0];

			tempRTV[i] = it[0]->renderTargetView.Get();

			it[0]->bindRTV(getContext(), &states);

			it[0]->boundOnRTV = true;
		}

		if (dsv)
		{
			dsv->bindDSV(getContext(), &states);
			getContext()->OMSetRenderTargets((UINT)rtvs.size(), tempRTV, dsv->getDSV());
		}
		else
		{
			getContext()->OMSetRenderTargets((UINT)rtvs.size(), tempRTV, nullptr);
		}
	}
	else
	{
		dsv->bindDSV(getContext(), &states);
		getContext()->OMSetRenderTargets(0, nullptr, dsv->getDSV());
	}

}

void ImCtx::OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs)
{
	if (states.curPUAV[0])
	{
		UnorderedAccessView::unbindPUAV(getContext(), &states);
	}
	if (states.curRTV[0])
	{
		RenderTargetView::unbindRTV(getContext(), &states);
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();

	for (UINT i = 0; i < uavs.size(); i++, it++)
	{
		states.curPUAV[i] = it[0];

		tempUAV[i] = it[0]->unorderedAccessView.Get();

		it[0]->bindPUAV(getContext(), &states);

		it[0]->boundOnRTV = true;
	}

	getContext()->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, (UINT)uavs.size(), tempUAV, nullptr);
}

void ImCtx::BindShader(Shader* const shader)
{
	shader->use(getContext());
}

void ImCtx::ClearDSV(DepthStencilView* dsv, const UINT& clearFlag, const float& depth, const UINT8& stencil)
{
	getContext()->ClearDepthStencilView(dsv->getDSV(), clearFlag, depth, stencil);
}

void ImCtx::ClearRTV(RenderTargetView* rtv, const float* const color)
{
	getContext()->ClearRenderTargetView(rtv->getRTV(), color);
}

void ImCtx::ClearUAV(UnorderedAccessView* uav, const float* const color)
{
	getContext()->ClearUnorderedAccessViewFloat(uav->getUAV(), color);
}

void ImCtx::ClearUAV(UnorderedAccessView* uav, const unsigned int* const value)
{
	getContext()->ClearUnorderedAccessViewUint(uav->getUAV(), value);
}

void ImCtx::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const UINT& slot)
{
	for (UINT i = slot; i < slot + uavs.size(); i++)
	{
		if (states.curCUAV[i])
		{
			states.curCUAV[i]->CUAVSlot = -1;
			states.curCUAV[i] = nullptr;
		}
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();
	for (UINT i = slot; i < slot + uavs.size(); i++, it++)
	{
		states.curCUAV[i] = it[0];
		tempUAV[i - slot] = it[0]->unorderedAccessView.Get();

		if (!it[0]->unbindFromCUAV(getContext(), &states) || !it[0]->unbindFromPUAV(getContext(), &states))
		{
			it[0]->bindCUAV(getContext(), &states);
		}

		it[0]->CUAVSlot = i;
	}

	getContext()->CSSetUnorderedAccessViews(slot, (UINT)uavs.size(), tempUAV, nullptr);
}

void ImCtx::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (states.curVSRV[i])
		{
			states.curVSRV[i]->VSSlot = -1;
			states.curVSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
	{
		states.curVSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->VSSlot != -1)
		{
			it[0]->unbindVSRV(getContext(), &states);
		}
		else
		{
			it[0]->bindSRV(getContext(), &states);
		}

		it[0]->VSSlot = i;
	}

	getContext()->VSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (states.curHSRV[i])
		{
			states.curHSRV[i]->HSSlot = -1;
			states.curHSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
	{
		states.curHSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->HSSlot != -1)
		{
			it[0]->unbindHSRV(getContext(), &states);
		}
		else
		{
			it[0]->bindSRV(getContext(), &states);
		}

		it[0]->HSSlot = i;
	}

	getContext()->HSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (states.curDSRV[i])
		{
			states.curDSRV[i]->DSSlot = -1;
			states.curDSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
	{
		states.curDSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->DSSlot != -1)
		{
			it[0]->unbindDSRV(getContext(), &states);
		}
		else
		{
			it[0]->bindSRV(getContext(), &states);
		}

		it[0]->DSSlot = i;
	}

	getContext()->DSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (states.curGSRV[i])
		{
			states.curGSRV[i]->GSSlot = -1;
			states.curGSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
	{
		states.curGSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->GSSlot != -1)
		{
			it[0]->unbindGSRV(getContext(), &states);
		}
		else
		{
			it[0]->bindSRV(getContext(), &states);
		}

		it[0]->GSSlot = i;
	}

	getContext()->GSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (states.curPSRV[i])
		{
			states.curPSRV[i]->PSSlot = -1;
			states.curPSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
	{
		states.curPSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->PSSlot != -1)
		{
			it[0]->unbindPSRV(getContext(), &states);
		}
		else
		{
			it[0]->bindSRV(getContext(), &states);
		}

		it[0]->PSSlot = i;
	}

	getContext()->PSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot)
{
	for (UINT i = slot; i < slot + srvs.size(); i++)
	{
		if (states.curCSRV[i])
		{
			states.curCSRV[i]->CSSlot = -1;
			states.curCSRV[i] = nullptr;
		}
	}

	std::initializer_list<ShaderResourceView*>::iterator it = srvs.begin();
	for (UINT i = slot; i < slot + srvs.size(); i++, it++)
	{
		states.curCSRV[i] = it[0];
		tempSRV[i - slot] = it[0]->shaderResourceView.Get();

		if (it[0]->CSSlot != -1)
		{
			it[0]->unbindCSRV(getContext(), &states);
		}
		else
		{
			it[0]->bindSRV(getContext(), &states);
		}

		it[0]->CSSlot = i;
	}

	getContext()->CSSetShaderResources(slot, (UINT)srvs.size(), tempSRV);
}

void ImCtx::IASetVertexBuffer(const UINT& slot, const std::initializer_list<VertexBuffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
{
	for (UINT i = slot; i < slot + (UINT)buffers.size(); i++)
	{
		if (states.curBuffer[i])
		{
			states.curBuffer[i]->IASlot = -1;
			states.curBuffer[i] = nullptr;
		}
	}

	std::initializer_list<VertexBuffer*>::iterator it = buffers.begin();
	std::initializer_list<UINT>::iterator itStride = strides.begin();
	std::initializer_list<UINT>::iterator itOffset = offsets.begin();

	for (UINT i = slot; i < slot + (UINT)buffers.size(); i++, it++, itStride++, itOffset++)
	{
		states.curBuffer[i] = it[0];
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStrides[i - slot] = itStride[0];
		tempOffsets[i - slot] = itOffset[0];

		if (!it[0]->unbindFromVertexBuffer(getContext(), &states))
		{
			it[0]->bindVertexBuffer(getContext(), &states);
		}

		it[0]->IASlot = i;
	}

	getContext()->IASetVertexBuffers(slot, (UINT)buffers.size(), tempBuffer, tempStrides, tempOffsets);
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

	getContext()->VSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
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

	getContext()->HSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
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

	getContext()->DSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
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

	getContext()->GSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
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

	getContext()->PSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
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

	getContext()->CSSetConstantBuffers1(slot, (UINT)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ImCtx::VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	getContext()->VSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	getContext()->HSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	getContext()->DSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	getContext()->GSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	getContext()->PSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (UINT i = slot; i < slot + (UINT)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	getContext()->CSSetSamplers(slot, (UINT)samplers.size(), tempSampler);
}

void ImCtx::IASetInputLayout(ID3D11InputLayout* const layout)
{
	getContext()->IASetInputLayout(layout);
}

void ImCtx::IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset)
{
	getContext()->IASetIndexBuffer(buffer->getBuffer(), format, offset);
}

void ImCtx::RSSetViewport(const float& width, const float& height)
{
	vp.Width = width;
	vp.Height = height;
	getContext()->RSSetViewports(1, &vp);
}

void ImCtx::RSSetViewport(const UINT& width, const UINT& height)
{
	RSSetViewport((float)width, (float)height);
}

void ImCtx::RSSetViewport(const int& width, const int& height)
{
	RSSetViewport((float)width, (float)height);
}

void ImCtx::IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
{
	getContext()->IASetPrimitiveTopology(topology);
}

void ImCtx::OMSetBlendState(ID3D11BlendState* const state)
{
	getContext()->OMSetBlendState(state, nullptr, 0xFFFFFFFF);
}

void ImCtx::RSSetState(ID3D11RasterizerState* const state)
{
	getContext()->RSSetState(state);
}

void ImCtx::OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef)
{
	getContext()->OMSetDepthStencilState(state, stencilRef);
}

void ImCtx::ResolveSubresource(const Resource* const dstResource, const UINT& dstSubresource, const Resource* const srcResource, const UINT& srcSubresource, const DXGI_FORMAT& format)
{
	getContext()->ResolveSubresource(dstResource->getResource(), dstSubresource, srcResource->getResource(), srcSubresource, format);
}

void ImCtx::DrawQuad()
{
	getContext()->Draw(3, 0);
}

void ImCtx::DrawCube()
{
	getContext()->Draw(36, 0);
}

void ImCtx::Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ)
{
	getContext()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ImCtx::Draw(const UINT& vertexCount, const UINT& startVertexLocation)
{
	getContext()->Draw(vertexCount, startVertexLocation);
}

void ImCtx::DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation)
{
	getContext()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void ImCtx::DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation)
{
	getContext()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

D3D11_MAPPED_SUBRESOURCE ImCtx::Map(Resource* const res, const UINT& subresource, const D3D11_MAP& mapType, const UINT& mapFlag)
{
	D3D11_MAPPED_SUBRESOURCE data = {};

	getContext()->Map(res->getResource(), subresource, mapType, mapFlag, &data);

	return data;
}

void ImCtx::Unmap(Resource* const res, const UINT& subresource)
{
	getContext()->Unmap(res->getResource(), subresource);
}

void ImCtx::UpdateSubresource(Resource* res, const UINT& dstSubresource, const D3D11_BOX* const pDstBox, const void* const data, const UINT& rowPitch, const UINT& depthPitch)
{
	getContext()->UpdateSubresource(res->getResource(), dstSubresource, pDstBox, data, rowPitch, depthPitch);
}

void ImCtx::GenerateMips(ShaderResourceView* const srv)
{
	getContext()->GenerateMips(srv->getSRV());
}

void ImCtx::CopySubresourceRegion(Resource* const pDstResource, const UINT& DstSubresource, const UINT& DstX, const UINT& DstY, const UINT& DstZ, Resource* const pSrcResource, const UINT& SrcSubresource, const D3D11_BOX* const pSrcBox)
{
	getContext()->CopySubresourceRegion(pDstResource->getResource(), DstSubresource, DstX, DstY, DstZ, pSrcResource->getResource(), SrcSubresource, pSrcBox);
}

void ImCtx::VSUnbindShader()
{
	getContext()->VSSetShader(nullptr, nullptr, 0);
}

void ImCtx::HSUnbindShader()
{
	getContext()->HSSetShader(nullptr, nullptr, 0);
}

void ImCtx::DSUnbindShader()
{
	getContext()->DSSetShader(nullptr, nullptr, 0);
}

void ImCtx::GSUnbindShader()
{
	getContext()->GSSetShader(nullptr, nullptr, 0);
}

void ImCtx::PSUnbindShader()
{
	getContext()->PSSetShader(nullptr, nullptr, 0);
}

void ImCtx::CSUnbindShader()
{
	getContext()->CSSetShader(nullptr, nullptr, 0);
}

void ImCtx::UnbindRTV()
{
	RenderTargetView::unbindRTV(getContext(), &states);
}

void ImCtx::UnbindCSUAV()
{
	UnorderedAccessView::unbindCUAV(getContext(), &states);
}

void ImCtx::UnbindPSUAV()
{
	UnorderedAccessView::unbindPUAV(getContext(), &states);
}

void ImCtx::GenNoise(UnorderedAccessView* const uav, const UINT& textureWidth, const UINT& textureHeight)
{
	BindShader(Shader::randNoiseCS);
	CSSetUAV({ uav }, 0);
	Dispatch(textureWidth / 32, textureHeight / 18, 1);
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

	BindShader(Shader::fullScreenVS);
	BindShader(Shader::fullScreenPS);
	DSUnbindShader();
	HSUnbindShader();
	GSUnbindShader();

	DrawQuad();
}

ID3D11DeviceContext4* ImCtx::getContext()
{
	return imctx.Get();
}

GraphicsStates* ImCtx::getStates()
{
	return &instance->states;
}
