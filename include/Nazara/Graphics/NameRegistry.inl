// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <stdexcept>

namespace Nz
{
	std::size_t NameRegistry::GetIndex(std::string_view passName) const
	{
		auto it = m_indices.find(passName);
		if (it == m_indices.end())
			throw std::runtime_error("pass " + std::string(passName) + " must be registered before being used");

		return it->second;
	}

	inline std::size_t NameRegistry::Register(std::string passName)
	{
		if (m_indices.contains(passName))
			throw std::runtime_error("pass " + passName + " is already registered");

		std::size_t passIndex = m_indices.size();
		m_indices.emplace(std::move(passName), passIndex);

		return passIndex;
	}
}
