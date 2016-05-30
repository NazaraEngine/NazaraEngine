// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDDOFPASS_HPP
#define NAZARA_DEFERREDDOFPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredDOFPass : public DeferredRenderPass
	{
		public:
			DeferredDOFPass();
			virtual ~DeferredDOFPass();

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const;
			bool Resize(const Vector2ui& dimensions);

		protected:
			RenderTexture m_dofRTT;
			RenderStates m_states;
			ShaderConstRef m_dofShader;
			ShaderConstRef m_gaussianBlurShader;
			TextureRef m_dofTextures[2];
			TextureSampler m_bilinearSampler;
			TextureSampler m_pointSampler;
			int m_gaussianBlurShaderFilterLocation;
	};
}

#endif // NAZARA_DEFERREDDOFPASS_HPP
