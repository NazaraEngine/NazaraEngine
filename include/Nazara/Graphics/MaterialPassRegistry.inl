// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPassRegistry.hpp>
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
		if (m_passIndex.find(passName) != m_passIndex.end())
			throw std::runtime_error("pass " + passName + " is already registered");

		m_passNames.push_back(std::move(passName));

		std::size_t passIndex = m_passIndex.size();
		m_passIndex.emplace(m_passNames.back(), passIndex);

		return passIndex;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
