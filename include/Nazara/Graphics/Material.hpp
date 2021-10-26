// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIAL_HPP
#define NAZARA_GRAPHICS_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API Material : public Resource
	{
		public:
			Material();
			~Material() = default;

			inline void AddPass(std::size_t passIndex, std::shared_ptr<MaterialPass> pass);
			inline void AddPass(std::string passName, std::shared_ptr<MaterialPass> pass);

			inline MaterialPass* GetPass(std::size_t passIndex) const;

			inline bool HasPass(std::size_t passIndex) const;

			inline void RemovePass(std::size_t passIndex);
			inline void RemovePass(const std::string& passName);

		private:
			std::vector<std::shared_ptr<MaterialPass>> m_passes;
	};
}

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_GRAPHICS_MATERIAL_HPP
