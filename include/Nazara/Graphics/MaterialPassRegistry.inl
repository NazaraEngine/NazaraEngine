// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	std::size_t MaterialPassRegistry::GetPassIndex(std::string_view passName) const
	{
		auto it = m_passIndex.find(passName);
		if (it == m_passIndex.end())
			throw std::runtime_error("pass " + std::string(passName) + " must be registered before being used");

		return it->second;
	}

	inline std::size_t MaterialPassRegistry::RegisterPass(std::string passName)
	{
		if (m_passIndex.contains(passName))
			throw std::runtime_error("pass " + passName + " is already registered");

		std::size_t passIndex = m_passIndex.size();
		m_passIndex.emplace(std::move(passName), passIndex);

		return passIndex;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
