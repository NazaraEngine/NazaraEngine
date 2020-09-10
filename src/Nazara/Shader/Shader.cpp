// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Shader.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	Shader::Shader() :
	Module("Shader", this)
	{
	}

	Shader* Shader::s_instance = nullptr;
}

