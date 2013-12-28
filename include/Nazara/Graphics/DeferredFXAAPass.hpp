// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFXAAPASS_HPP
#define NAZARA_DEFERREDFXAAPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

class NAZARA_API NzDeferredFXAAPass : public NzDeferredRenderPass
{
	public:
		NzDeferredFXAAPass();
		virtual ~NzDeferredFXAAPass();

		bool Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const;

	protected:
		NzRenderStates m_states;
		NzShaderProgramRef m_fxaaProgram;
		NzTextureSampler m_pointSampler;
};

#endif // NAZARA_DEFERREDFXAAPASS_HPP
