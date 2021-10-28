// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_HPP
#define NAZARA_WIDGETS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Core/ModuleBase.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Widgets/Config.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API Widgets : public ModuleBase<Widgets>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<ECS, Graphics>;

			struct Config;

			Widgets(Config config);
			~Widgets() = default;

			struct Config {};

		private:
			static Widgets* s_instance;
	};
}

#include <Nazara/Widgets/Widgets.inl>

#endif // NAZARA_WIDGETS_HPP
