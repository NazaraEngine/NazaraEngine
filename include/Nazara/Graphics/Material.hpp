// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API Material : public Resource
	{
		public:
			Material();
			~Material() = default;

			void AddPass(std::string name, std::shared_ptr<MaterialPass> pass);

			inline MaterialPass* GetPass(const std::string& name) const;

			bool HasPass(const std::string& name) const;

			void RemovePass(const std::string& name);

		private:
			std::unordered_map<std::string, std::shared_ptr<MaterialPass>> m_passes;
	};
}

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_MATERIAL_HPP
