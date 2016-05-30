// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFINALPASS_HPP
#define NAZARA_DEFERREDFINALPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/UberShader.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredFinalPass : public DeferredRenderPass
	{
		public:
			DeferredFinalPass();
			virtual ~DeferredFinalPass();

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const;

		protected:
			RenderStates m_states;
			TextureSampler m_pointSampler;
			UberShaderConstRef m_uberShader;
			const UberShaderInstance* m_uberShaderInstance;
			int m_materialDiffuseUniform;
			int m_materialDiffuseMapUniform;
	};
}

#endif // NAZARA_DEFERREDFINALPASS_HPP
