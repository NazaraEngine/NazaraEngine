// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFOGPASS_HPP
#define NAZARA_DEFERREDFOGPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

class NAZARA_API NzDeferredFogPass : public NzDeferredRenderPass
{
	public:
		NzDeferredFogPass();
		virtual ~NzDeferredFogPass();

		bool Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const;

	protected:
		NzRenderStates m_states;
		NzShaderRef m_shader;
		NzTextureSampler m_pointSampler;
};

#endif // NAZARA_DEFERREDFOGPASS_HPP
