// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GUILLOTINETEXTUREATLAS_HPP
#define NAZARA_GRAPHICS_GUILLOTINETEXTUREATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/GuillotineImageAtlas.hpp>

namespace Nz
{
	class RenderDevice;

	class NAZARA_GRAPHICS_API GuillotineTextureAtlas : public GuillotineImageAtlas
	{
		public:
			inline GuillotineTextureAtlas(RenderDevice& renderDevice);
			~GuillotineTextureAtlas() = default;

			DataStoreFlags GetStorage() const override;

		private:
			std::shared_ptr<AbstractImage> ResizeImage(const std::shared_ptr<AbstractImage>& oldImage, const Vector2ui& size) const override;

			RenderDevice& m_renderDevice;
	};
}

#include <Nazara/Graphics/GuillotineTextureAtlas.inl>

#endif // NAZARA_GRAPHICS_GUILLOTINETEXTUREATLAS_HPP
