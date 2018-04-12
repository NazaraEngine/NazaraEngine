// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTBACKGROUND_HPP
#define NAZARA_ABSTRACTBACKGROUND_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>

namespace Nz
{
	class AbstractBackground;
	class AbstractViewer;

	using BackgroundConstRef = ObjectRef<const AbstractBackground>;
	using BackgroundLibrary = ObjectLibrary<AbstractBackground>;
	using BackgroundRef = ObjectRef<AbstractBackground>;

	class NAZARA_GRAPHICS_API AbstractBackground : public RefCounted
	{
		public:
			AbstractBackground() = default;
			AbstractBackground(const AbstractBackground&) = default;
			AbstractBackground(AbstractBackground&&) = delete;
			virtual ~AbstractBackground();

			virtual void Draw(const AbstractViewer* viewer) const = 0;

			virtual BackgroundType GetBackgroundType() const = 0;

			AbstractBackground& operator=(const AbstractBackground&) = default;
			AbstractBackground& operator=(AbstractBackground&&) = delete;

		private:
			static BackgroundLibrary::LibraryMap s_library;
	};
}

#endif // NAZARA_ABSTRACTBACKGROUND_HPP
