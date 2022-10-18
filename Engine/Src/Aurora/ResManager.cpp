#include<Aurora/ResManager.h>

ResManager* ResManager::instance = nullptr;

ResManager::ResManager() :
	tempBuffer{}, tempRTV{}, tempUAV{}, tempSRV{}
{
}

ResManager* ResManager::get()
{
	return instance;
}

void ResManager::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, ID3D11DepthStencilView* const dsv)
{
	for (unsigned int i = 0; RenderTargetView::curRTV[i]; i++)
	{
		RenderTargetView::curRTV[i]->boundOnRTV = false;
		RenderTargetView::curRTV[i] = nullptr;
	}

	std::initializer_list<RenderTargetView*>::iterator it = rtvs.begin();

	for (unsigned int i = 0; i < rtvs.size(); i++, it++)
	{
		RenderTargetView::curRTV[i] = it[0];

		tempRTV[i] = it[0]->renderTargetView.Get();

		it[0]->bindRTV();

		it[0]->boundOnRTV = true;
	}

	Renderer::context->OMSetRenderTargets((unsigned int)rtvs.size(), tempRTV, dsv);
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
		if (UnorderedAccessView::curUAV[i])
		{
			UnorderedAccessView::curUAV[i]->UAVSlot = -1;
			UnorderedAccessView::curUAV[i] = nullptr;
		}
	}

	std::initializer_list<UnorderedAccessView*>::iterator it = uavs.begin();
	for (unsigned int i = slot; i < slot + uavs.size(); i++, it++)
	{
		UnorderedAccessView::curUAV[i] = it[0];
		tempUAV[i - slot] = it[0]->unorderedAccessView.Get();

		if (!it[0]->unbindFromUAV())
		{
			it[0]->bindUAV();
		}

		it[0]->UAVSlot = i;
	}

	Renderer::context->CSSetUnorderedAccessViews(slot, (unsigned int)uavs.size(), tempUAV, nullptr);
}

void ResManager::VSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
	}

	Renderer::context->VSSetConstantBuffers(slot, (unsigned int)buffers.size(), tempBuffer);
}

void ResManager::HSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
	}

	Renderer::context->HSSetConstantBuffers(slot, (unsigned int)buffers.size(), tempBuffer);
}

void ResManager::DSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
	}

	Renderer::context->DSSetConstantBuffers(slot, (unsigned int)buffers.size(), tempBuffer);
}

void ResManager::GSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
	}

	Renderer::context->GSSetConstantBuffers(slot, (unsigned int)buffers.size(), tempBuffer);
}

void ResManager::PSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
	}

	Renderer::context->PSSetConstantBuffers(slot, (unsigned int)buffers.size(), tempBuffer);
}

void ResManager::CSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();

	for (unsigned int i = slot; i < slot + (unsigned int)buffers.size(); i++, it++)
	{
		tempBuffer[i - slot] = it[0]->buffer.Get();
	}

	Renderer::context->CSSetConstantBuffers(slot, (unsigned int)buffers.size(), tempBuffer);
}

void ResManager::IASetVertexBuffer(const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
{
	Buffer::unbindVertexBuffer();

	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	std::initializer_list<UINT>::iterator itStride = strides.begin();
	std::initializer_list<UINT>::iterator itOffset = offsets.begin();

	for (unsigned int i = 0; i < (unsigned int)buffers.size(); i++, it++, itStride++, itOffset++)
	{
		tempBuffer[i] = it[0]->buffer.Get();
		tempStrides[i] = itStride[0];
		tempOffsets[i] = itOffset[0];
		it[0]->boundOnIA = true;
		it[0]->bindVertexBuffer();
	}

	Renderer::context->IASetVertexBuffers(0, (unsigned int)buffers.size(), tempBuffer, tempStrides, tempOffsets);
}