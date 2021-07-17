// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline MaterialPass* Material::GetPass(const std::string& name) const
	{
		auto it = m_passes.find(name);
		if (it == m_passes.end())
			return nullptr;

		return it->second.get();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
