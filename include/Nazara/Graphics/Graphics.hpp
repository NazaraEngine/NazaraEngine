// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_HPP
#define NAZARA_GRAPHICS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API Graphics : public ModuleBase<Graphics>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Renderer>;

			struct Config {};

			Graphics(Config /*config*/);
			~Graphics();

		private:
			static Graphics* s_instance;
	};
}

#endif
