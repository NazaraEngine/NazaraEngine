// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Format.hpp>

namespace Nz
{
	std::size_t NameRegistry::GetIndex(std::string_view name) const
	{
		auto it = m_indices.find(name);
		if (it == m_indices.end())
			throw std::runtime_error(Format("name {} must be registered before being used", name));

		return it->second;
	}

	inline std::size_t NameRegistry::Register(std::string name)
	{
		if (m_indices.contains(name))
			throw std::runtime_error(Format("{} is already registered", name));

		std::size_t index = m_indices.size();
		m_indices.emplace(std::move(name), index);

		return index;
	}
}
