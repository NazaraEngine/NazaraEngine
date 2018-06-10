// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFOGPASS_HPP
#define NAZARA_DEFERREDFOGPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredFogPass : public DeferredRenderPass
	{
		public:
			DeferredFogPass();
			virtual ~DeferredFogPass();

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const override;

		protected:
			RenderStates m_states;
			ShaderRef m_shader;
			TextureSampler m_pointSampler;
			int m_shaderEyePositionLocation;
	};
}

#endif // NAZARA_DEFERREDFOGPASS_HPP
