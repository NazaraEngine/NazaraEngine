// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ErrorFlags.hpp>

namespace Nz
{
	bool Icon::IsValid() const
	{
		return m_impl != nullptr;
	}
}

