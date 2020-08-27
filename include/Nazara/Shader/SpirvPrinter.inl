// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvPrinter::SpirvPrinter() :
	m_currentState(nullptr)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
