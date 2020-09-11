// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_HPP
#define NAZARA_SHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Shader/Config.hpp>

namespace Nz
{
	class NAZARA_SHADER_API Shader : public ModuleBase<Shader>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			Shader();
			~Shader() = default;

		private:
			static Shader* s_instance;
	};	
}

#endif // NAZARA_SHADER_HPP
