// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_HPP
#define NAZARA_VULKAN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Vulkan/Config.hpp>

namespace Nz
{
	class NAZARA_VULKAN_API Vulkan
	{
		public:
			Vulkan() = delete;
			~Vulkan() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};	
}

#endif // NAZARA_VULKAN_HPP
