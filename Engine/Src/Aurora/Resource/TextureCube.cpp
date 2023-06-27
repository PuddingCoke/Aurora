#include<Aurora/Resource/TextureCube.h>

TextureCube::TextureCube(std::initializer_list<std::string> texturesPath)
{
	Texture2D* textures[6]{};
	{
		int idx = 0;
		for (const std::string& path : texturesPath)
		{
			textures[idx] = new Texture2D(path);
			idx++;
		}
	}

	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = textures[0]->getWidth();
		tDesc.Height = textures[0]->getHeight();
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		tDesc.Format = textures[0]->getFormat();
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.ArraySize = 6;
		tDesc.MipLevels = 1;
		GraphicsDevice::get()->createTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		ImCtx::get()->CopySubresourceRegion(this, D3D11CalcSubresource(0, i, 1), 0, 0, 0, textures[i], 0, nullptr);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textures[0]->getFormat();
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		createSRV(texture.Get(), srvDesc);
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		delete textures[i];
	}

}

TextureCube::TextureCube(const std::string& texturePath)
{
	const std::wstring wTexturePath(texturePath.begin(), texturePath.end());
	DirectX::CreateDDSTextureFromFileEx(GraphicsDevice::getDevice(), ImCtx::get()->getContext(), wTexturePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DDS_LOADER_DEFAULT, (ID3D11Resource**)texture.ReleaseAndGetAddressOf(), shaderResourceView.ReleaseAndGetAddressOf());

	std::cout << "[class TextureCube] " << texturePath << " create successfully!\n";
}

TextureCube::TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const unsigned int& mipLevels)
{
	ConstantBuffer* buffer = new ConstantBuffer(sizeof(DirectX::XMMATRIX), D3D11_USAGE_DYNAMIC);

	ResourceTexture* equirectangularMap = new ResourceTexture(texturePath);

	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = skyboxResolution;
	tDesc.Height = skyboxResolution;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	tDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tDesc.ArraySize = 6;
	tDesc.MipLevels = mipLevels;
	GraphicsDevice::get()->createTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());

	const DirectX::XMVECTOR focusPoints[6] =
	{
		{1.0f,  0.0f,  0.0f},
		{-1.0f,  0.0f,  0.0f},
		{0.0f,  1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f},
		{0.0f,  0.0f,  1.0f},
		{0.0f,  0.0f, -1.0f}
	};
	const DirectX::XMVECTOR upVectors[6] =
	{
		{0.0f, 1.0f,  0.0f},
		{0.0f, 1.0f,  0.0f},
		{0.0f,  0.0f,  -1.0f},
		{0.0f,  0.0f, 1.0f},
		{0.0f, 1.0f,  0.0f},
		{0.0f, 1.0f,  0.0f}
	};

	ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);
	ImCtx::get()->PSSetSRV({ equirectangularMap }, 0);
	ImCtx::get()->OMSetBlendState(nullptr);
	ImCtx::get()->RSSetViewport(skyboxResolution, skyboxResolution);
	ImCtx::get()->VSSetConstantBuffer({ buffer }, 2);

	ImCtx::get()->BindShader(Shader::equirectangularVS);
	ImCtx::get()->BindShader(Shader::equirectangularPS);

	const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 0.1f, 10.f);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, mipLevels);

		RenderOnlyRTV* rtv = new RenderOnlyRTV(texture.Get(), rtvDesc);

		ImCtx::get()->OMSetRTV({ rtv }, nullptr);
		const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH({ 0.f,0.f,0.f }, focusPoints[i], upVectors[i]);
		const DirectX::XMMATRIX viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		memcpy(ImCtx::get()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &viewProj, sizeof(DirectX::XMMATRIX));
		ImCtx::get()->Unmap(buffer, 0);
		ImCtx::get()->DrawCube();

		ImCtx::get()->UnbindRTV();
		delete rtv;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = mipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	createSRV(texture.Get(), srvDesc);

	if (mipLevels > 1)
	{
		ImCtx::get()->GenerateMips(this);
	}

	ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	ImCtx::get()->UnbindSRV(equirectangularMap);

	delete equirectangularMap;
	delete buffer;
}

TextureCube::~TextureCube()
{
}

ID3D11Resource* TextureCube::getResource() const
{
	return texture.Get();
}

void TextureCube::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
}