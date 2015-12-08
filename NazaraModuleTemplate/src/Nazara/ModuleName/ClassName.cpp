// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/ClassName.hpp>
#include <Nazara/ModuleName/Debug.hpp>

namespace Nz
{
	ClassName::ClassName() :
	m_attribute(42)
	{
	}

	int ClassName::GetAttribute() const
	{
		return m_attribute;
	}

	void ClassName::SetAttribute(int attribute)
	{
		m_attribute = attribute;
	}
}

