// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DEPTHMATERIALPASS_HPP
#define NAZARA_GRAPHICS_DEPTHMATERIALPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BasicMaterialPass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DepthMaterialPass : public BasicMaterialPass
	{
		friend class MaterialPipeline;

		public:
			using BasicMaterialPass::BasicMaterialPass;
			~DepthMaterialPass() = default;

			static inline const std::shared_ptr<MaterialSettings>& GetSettings();

		protected:
			static std::vector<std::shared_ptr<UberShader>> BuildShaders();

		private:
			static bool Initialize();
			static void Uninitialize();

			static std::shared_ptr<MaterialSettings> s_depthMaterialSettings;
	};
}

#include <Nazara/Graphics/DepthMaterialPass.inl>

#endif // NAZARA_GRAPHICS_DEPTHMATERIALPASS_HPP
