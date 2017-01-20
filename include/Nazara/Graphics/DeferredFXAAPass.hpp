// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFXAAPASS_HPP
#define NAZARA_DEFERREDFXAAPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredFXAAPass : public DeferredRenderPass
	{
		public:
			DeferredFXAAPass();
			virtual ~DeferredFXAAPass();

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const;

		protected:
			RenderStates m_states;
			ShaderRef m_fxaaShader;
			TextureSampler m_pointSampler;
	};
}

#endif // NAZARA_DEFERREDFXAAPASS_HPP
