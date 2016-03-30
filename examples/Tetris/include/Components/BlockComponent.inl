#include "Constants.hpp"
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>

/// *****
#include <iostream>

namespace Tet {
	inline BlockComponent::BlockComponent(const Nz::Color& blockColor) {
		GenerateTextureFromColor(blockColor);
	}

	inline BlockComponent::BlockComponent(const Nz::TextureRef& blockTexture)
		: texture{ blockTexture }
	{}

	inline const Nz::TextureRef& BlockComponent::GenerateTextureFromColor(const Nz::Color& blockColor) {
		Nz::TextureRef blockTexture{ Nz::Texture::New() };
		blockTexture->Create(Nz::ImageType_2D, Nz::PixelFormatType_RGB8, blockSize, blockSize);
		const Nz::UInt8& r{ blockColor.r }, g{ blockColor.g }, b{ blockColor.b };
		Nz::UInt8 pixels[blockSize*blockSize*3];
		//bool xShadowSetted{ false };
		for (std::size_t i{ 0 }; i < blockSize*blockSize*3; i += 3) {
			if (i % (blockSize*3) < blockSize * .5f || i / blockSize <= blockSize * .6f) {
				pixels[i]   = std::max(r - 100, 0);
				pixels[i+1] = std::max(g - 100, 0);
				pixels[i+2] = std::max(b - 100, 0);
			}
			else {
				pixels[i]   = r;
				pixels[i+1] = g;
				pixels[i+2] = b;
			}
		}
        
        NazaraAssert(blockTexture->Update(pixels, static_cast<unsigned int>(blockSize), static_cast<unsigned int>(blockSize)),
                     "Failed to generate Texture from color");
        
        texture = blockTexture;
        return texture;
    }

	inline BlockComponent& BlockComponent::operator=(const Nz::Color& blockColor) {
		GenerateTextureFromColor(blockColor);
		return *this;
	}
	inline BlockComponent& BlockComponent::operator=(const Nz::TextureRef& blockTexture) {
		texture = blockTexture;
		return *this;
	}
}