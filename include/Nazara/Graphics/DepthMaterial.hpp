// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEPTH_MATERIAL_HPP
#define NAZARA_DEPTH_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DepthMaterial : public BasicMaterial
	{
		friend class MaterialPipeline;

		public:
			using BasicMaterial::BasicMaterial;
			~DepthMaterial() = default;

			static inline const std::shared_ptr<MaterialSettings>& GetSettings();

		protected:
			static std::vector<std::shared_ptr<UberShader>> BuildShaders();

		private:
			static bool Initialize();
			static void Uninitialize();

			static std::shared_ptr<MaterialSettings> s_materialSettings;
	};
}

#include <Nazara/Graphics/DepthMaterial.inl>

#endif
