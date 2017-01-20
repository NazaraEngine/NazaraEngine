// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GUILLOTINETEXTUREATLAS_HPP
#define NAZARA_GUILLOTINETEXTUREATLAS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/GuillotineImageAtlas.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API GuillotineTextureAtlas : public GuillotineImageAtlas
	{
		public:
			GuillotineTextureAtlas() = default;
			~GuillotineTextureAtlas() = default;

			UInt32 GetStorage() const override;

		private:
			AbstractImage* ResizeImage(AbstractImage* oldImage, const Vector2ui& size) const override;
	};
}

#endif // NAZARA_GUILLOTINETEXTUREATLAS_HPP
