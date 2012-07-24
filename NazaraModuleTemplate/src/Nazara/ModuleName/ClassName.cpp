// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/ClassName.hpp>
#include <Nazara/ModuleName/Debug.hpp>

NzClassName::NzClassName() :
m_attribute(42)
{
}

int NzClassName::GetAttribute() const
{
	return m_attribute;
}

void NzClassName::SetAttribute(int attribute)
{
	m_attribute = attribute;
}
