// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP
#define NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/GuillotineAtlas.hpp>

namespace Nz
{
	class NAZARA_CORE_API GuillotineImageAtlas final : public GuillotineAtlas
	{
		public:
			using GuillotineAtlas::GuillotineAtlas;
			~GuillotineImageAtlas() = default;

			DataStoreFlags GetStorage() const override;

		private:
			std::shared_ptr<AbstractImage> ResizeImage(const AbstractImage* oldImage, const Vector2ui& size) const override;
			void UpdateImage(AbstractImage& image, const void* ptr, const Rectui& rect, UInt32 srcWidth, UInt32 srcHeight) const override;
	};
}

#include <Nazara/Core/GuillotineImageAtlas.inl>

#endif // NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP
