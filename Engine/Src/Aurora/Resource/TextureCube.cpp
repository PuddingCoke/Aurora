#include<Aurora/Resource/TextureCube.h>

Shader* TextureCube::equirectangularVS;

Shader* TextureCube::equirectangularPS;

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
		Renderer::get()->createTexture2D(&tDesc, nullptr, cubeTexture.ReleaseAndGetAddressOf());
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		Renderer::getContext()->CopySubresourceRegion(cubeTexture.Get(), D3D11CalcSubresource(0, i, 1), 0, 0, 0, textures[i]->getResource(), 0, nullptr);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textures[0]->getFormat();
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		createSRV(cubeTexture.Get(), srvDesc);
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		delete textures[i];
	}

}

TextureCube::TextureCube(const std::string& texturePath)
{
	const std::wstring wTexturePath(texturePath.begin(), texturePath.end());
	DirectX::CreateDDSTextureFromFileEx(Renderer::getDevice(), Renderer::getContext(), wTexturePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DDS_LOADER_DEFAULT, (ID3D11Resource**)cubeTexture.ReleaseAndGetAddressOf(), releaseAndGetSRV());

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
	Renderer::get()->createTexture2D(&tDesc, nullptr, cubeTexture.ReleaseAndGetAddressOf());

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

	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
	RenderAPI::get()->PSSetSRV({ equirectangularMap }, 0);
	RenderAPI::get()->OMSetBlendState(nullptr);
	RenderAPI::get()->RSSetViewport(skyboxResolution, skyboxResolution);
	RenderAPI::get()->VSSetConstantBuffer({ buffer }, 2);

	equirectangularVS->use();
	equirectangularPS->use();

	const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 0.1f, 10.f);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, mipLevels);

		RenderOnlyRTV* rtv = new RenderOnlyRTV(cubeTexture.Get(), rtvDesc);

		RenderAPI::get()->OMSetRTV({ rtv }, nullptr);
		const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH({ 0.f,0.f,0.f }, focusPoints[i], upVectors[i]);
		const DirectX::XMMATRIX viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		memcpy(buffer->map().pData, &viewProj, sizeof(DirectX::XMMATRIX));
		buffer->unmap();
		RenderAPI::get()->DrawCube();

		delete rtv;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = mipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	createSRV(cubeTexture.Get(), srvDesc);

	if (mipLevels > 1)
	{
		generateMips();
	}

	RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	delete equirectangularMap;
	delete buffer;
}

TextureCube::~TextureCube()
{
}

void TextureCube::iniShader()
{
	std::cout << "equirectangularVS ";
	equirectangularVS = new Shader(g_EquirectangularVSBytes, sizeof(g_EquirectangularVSBytes), ShaderType::Vertex);
	std::cout << "equirectangularPS ";
	equirectangularPS = new Shader(g_EquirectangularPSBytes, sizeof(g_EquirectangularPSBytes), ShaderType::Pixel);
}

void TextureCube::releaseShader()
{
	delete equirectangularVS;
	delete equirectangularPS;
}

void TextureCube::bindSRV()
{

}