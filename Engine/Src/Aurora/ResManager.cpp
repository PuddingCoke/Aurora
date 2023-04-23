#include<Aurora/ResManager.h>

ResManager* ResManager::instance = nullptr;

ResManager::ResManager() :
	tempBuffer{}, tempStartConstants{}, tempNumConstants{}, tempRTV{}, tempUAV{}, tempSRV{}, tempOffsets{}, tempStrides{}, tempSampler{}
{
}

ResManager* ResManager::get()
{
	return instance;
}

void ResManager::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv)
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
			Renderer::context->OMSetRenderTargets((unsigned int)rtvs.size(), tempRTV, dsv->getDSV());
		}
		else
		{
			Renderer::context->OMSetRenderTargets((unsigned int)rtvs.size(), tempRTV, nullptr);
		}
	}
	else
	{
		dsv->bindDSV();
		Renderer::context->OMSetRenderTargets(0, nullptr, dsv->getDSV());
	}

}

void ResManager::OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs)
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

	Renderer::context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, (unsigned int)uavs.size(), tempUAV, nullptr);
}

void ResManager::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
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

	Renderer::context->VSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void ResManager::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
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

	Renderer::context->HSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void ResManager::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
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

	Renderer::context->DSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void ResManager::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
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

	Renderer::context->GSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void ResManager::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
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

	Renderer::context->PSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void ResManager::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
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

	Renderer::context->CSSetShaderResources(slot, (unsigned int)srvs.size(), tempSRV);
}

void ResManager::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot)
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

	Renderer::context->CSSetUnorderedAccessViews(slot, (unsigned int)uavs.size(), tempUAV, nullptr);
}

void ResManager::IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
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

	Renderer::context->IASetVertexBuffers(slot, (unsigned int)buffers.size(), tempBuffer, tempStrides, tempOffsets);
}

void ResManager::VSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::context->VSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ResManager::HSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::context->HSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ResManager::DSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::context->DSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ResManager::GSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::context->GSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ResManager::PSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::context->PSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ResManager::CSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = constantBuffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)constantBuffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
		tempStartConstants[i - slot] = it[0]->startConstants;
		tempNumConstants[i - slot] = it[0]->numConstants;
	}

	Renderer::context->CSSetConstantBuffers1(slot, (unsigned int)constantBuffers.size(), tempBuffer, tempStartConstants, tempNumConstants);
}

void ResManager::VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::context->VSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void ResManager::HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::context->HSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void ResManager::DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::context->DSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void ResManager::GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::context->GSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void ResManager::PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::context->PSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}

void ResManager::CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot)
{
	std::initializer_list<ID3D11SamplerState*>::iterator it = samplers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)samplers.size(); i++, it++)
	{
		tempSampler[i - slot] = it[0];
	}

	Renderer::context->CSSetSamplers(slot, (unsigned int)samplers.size(), tempSampler);
}
