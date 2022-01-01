// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPASSREGISTRY_HPP
#define NAZARA_GRAPHICS_MATERIALPASSREGISTRY_HPP

#include <Nazara/Prerequisites.hpp>
#include <string>
#include <unordered_map>

namespace Nz
{
	class MaterialPassRegistry
	{
		public:
			MaterialPassRegistry() = default;
			MaterialPassRegistry(const MaterialPassRegistry&) = default;
			MaterialPassRegistry(MaterialPassRegistry&&) = default;
			~MaterialPassRegistry() = default;

			inline std::size_t GetPassIndex(const std::string& passName) const;

			inline std::size_t RegisterPass(std::string passName);

			MaterialPassRegistry& operator=(const MaterialPassRegistry&) = default;
			MaterialPassRegistry& operator=(MaterialPassRegistry&&) = default;

		private:
			std::unordered_map<std::string, std::size_t> m_passIndex;
	};
}

#include <Nazara/Graphics/MaterialPassRegistry.inl>

#endif // NAZARA_GRAPHICS_MATERIALPASSREGISTRY_HPP
