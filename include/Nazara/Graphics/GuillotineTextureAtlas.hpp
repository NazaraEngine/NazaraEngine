// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GUILLOTINETEXTUREATLAS_HPP
#define NAZARA_GRAPHICS_GUILLOTINETEXTUREATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/GuillotineImageAtlas.hpp>
#include <Nazara/Graphics/Export.hpp>

namespace Nz
{
	class RenderDevice;

	class NAZARA_GRAPHICS_API GuillotineTextureAtlas : public GuillotineImageAtlas
	{
		public:
			inline GuillotineTextureAtlas(RenderDevice& renderDevice, PixelFormat pixelFormat, UInt32 initialLayerSize = 512);
			inline GuillotineTextureAtlas(RenderDevice& renderDevice, PixelFormat texturePixelFormat, PixelFormat imagePixelFormat, UInt32 initialLayerSize = 512);
			~GuillotineTextureAtlas() = default;

			DataStoreFlags GetStorage() const override;

		private:
			std::shared_ptr<AbstractImage> ResizeImage(const std::shared_ptr<AbstractImage>& oldImage, const Vector2ui& size) const override;

			RenderDevice& m_renderDevice;
			PixelFormat m_texturePixelFormat;
	};
}

#include <Nazara/Graphics/GuillotineTextureAtlas.inl>

#endif // NAZARA_GRAPHICS_GUILLOTINETEXTUREATLAS_HPP
