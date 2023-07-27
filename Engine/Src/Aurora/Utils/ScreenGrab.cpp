#include<Aurora/Utils/ScreenGrab.h>

ScreenGrab::ScreenGrab(ComPtr<ID3D11Texture2D> backBuffer) :
	copyTexture(new Texture2D(Graphics::getWidth(), Graphics::getHeight(), 1, 1, FMT::BGRA8UN, 0, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ)),
	backBuffer(new Texture2D(backBuffer))
{
	eventIndex = Keyboard::addKeyDownEvent(Keyboard::F1, [this]()
		{
			ImCtx::get()->CopyResource(this->copyTexture, this->backBuffer);

			D3D11_MAPPED_SUBRESOURCE mappedData = ImCtx::get()->Map(copyTexture, 0, D3D11_MAP_READ, 0);

			uint32_t width = Graphics::getWidth();

			uint32_t height = Graphics::getHeight();

			uint32_t* pixels = (uint32_t*)mappedData.pData;

			for (uint32_t i = 0; i < width * height; i++)
			{
				uint32_t pixel = pixels[i];
				// Extract the color channels
				uint8_t blue = (pixel >> 0) & 0xFF;
				uint8_t green = (pixel >> 8) & 0xFF;
				uint8_t red = (pixel >> 16) & 0xFF;
				uint8_t alpha = (pixel >> 24) & 0xFF;

				// Reorder the color channels to BGRA format
				pixels[i] = (red << 0) | (green << 8) | (blue << 16) | (alpha << 24);
			}

			stbi_write_png("screen_grab_result.png", width, height, 4, pixels, width * 4);

			ImCtx::get()->Unmap(this->copyTexture, 0);

			std::cout << "[class ScreenGrab] Save texture to screen_grab_result.png\n";
		});
}

ScreenGrab::~ScreenGrab()
{
	Keyboard::removeKeyDownEvent(Keyboard::F1, eventIndex);

	delete copyTexture;
	delete backBuffer;
}
