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
		for (std::size_t y{ 0 }; y < blockSize; y += 3) {
			for (std::size_t x{ 0 }; x < blockSize; x += 3) {
				if (x < blockSize * .1f || y < blockSize * .1f) {
					pixels[y*x]   = std::max(r - 10, 0);
					pixels[y*x+1] = std::max(g - 10, 0);
					pixels[y*x+2] = std::max(b - 10, 0);
				}
				else {
					pixels[y*x]   = r;
					pixels[y*x+1] = g;
					pixels[y*x+2] = b;
				}
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