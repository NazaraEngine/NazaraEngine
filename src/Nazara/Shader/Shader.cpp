// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Shader.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	Shader::Shader(Config /*config*/) :
	ModuleBase("Shader", this)
	{
	}

	Shader* Shader::s_instance = nullptr;
}
