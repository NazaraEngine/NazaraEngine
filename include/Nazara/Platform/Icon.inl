// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	bool Icon::IsValid() const
	{
		return m_impl != nullptr;
	}
}

#include <Nazara/Platform/DebugOff.hpp>
