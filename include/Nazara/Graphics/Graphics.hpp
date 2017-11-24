// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_HPP
#define NAZARA_GRAPHICS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API Graphics
	{
		public:
			Graphics() = delete;
			~Graphics() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_GRAPHICS_HPP
