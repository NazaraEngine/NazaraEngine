// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderWriter.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	ShaderWriter::~ShaderWriter() = default;

	void ShaderWriter::EnableCondition(const String& name, bool cond)
	{
		if (cond)
			m_conditions.insert(name);
		else
			m_conditions.erase(name);
	}

	bool ShaderWriter::IsConditionEnabled(const String & name) const
	{
		return m_conditions.count(name) != 0;
	}
}
