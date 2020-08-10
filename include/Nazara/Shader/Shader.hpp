// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_HPP
#define NAZARA_SHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Shader/Config.hpp>

namespace Nz
{
	class NAZARA_SHADER_API Shader
	{
		public:
			Shader() = delete;
			~Shader() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};	
}

#endif // NAZARA_SHADER_HPP
