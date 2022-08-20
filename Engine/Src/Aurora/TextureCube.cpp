#include<Aurora/TextureCube.h>

TextureCube* TextureCube::create(std::initializer_list<std::string> texturesPath)
{
	return new TextureCube(texturesPath);
}

void TextureCube::setSRV(const UINT& slot)
{
	Graphics::context->PSSetShaderResources(slot, 1, cubeSRV.GetAddressOf());
}

TextureCube::~TextureCube()
{
}

TextureCube::TextureCube(std::initializer_list<std::string> texturesPath)
{
	Texture2D* textures[6];
	{
		int idx = 0;
		for (const std::string& path : texturesPath)
		{
			textures[idx] = Texture2D::create(path, D3D11_USAGE_STAGING, (D3D11_BIND_FLAG)0, D3D11_CPU_ACCESS_READ);
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
		Graphics::device->CreateTexture2D(&tDesc, nullptr, cubeTexture.ReleaseAndGetAddressOf());
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		Graphics::context->Map(textures[i]->getTexture2D(), 0, D3D11_MAP_READ, 0, &mappedData);
		Graphics::context->UpdateSubresource(cubeTexture.Get(), D3D11CalcSubresource(0, i, 1), 0,
			mappedData.pData, mappedData.RowPitch, mappedData.DepthPitch);
		Graphics::context->Unmap(textures[i]->getTexture2D(), 0);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textures[0]->getFormat();
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = -1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		Graphics::device->CreateShaderResourceView(cubeTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		delete textures[i];
	}

}
