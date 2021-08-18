// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline GlslWriter::GlslWriter() :
	m_currentState(nullptr)
	{
	}

	inline std::string GlslWriter::Generate(ShaderAst::Statement& shader, const BindingMapping& bindingMapping, const States& states)
	{
		return Generate(std::nullopt, shader, bindingMapping, states);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
