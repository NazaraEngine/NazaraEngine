// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDDOFPASS_HPP
#define NAZARA_DEFERREDDOFPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

class NAZARA_API NzDeferredDOFPass : public NzDeferredRenderPass
{
	public:
		NzDeferredDOFPass();
		virtual ~NzDeferredDOFPass();

		bool Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const;
		bool Resize(const NzVector2ui& dimensions);

	protected:
		NzRenderTexture m_dofRTT;
		NzRenderStates m_states;
		NzShaderProgramRef m_blurProgram;
		NzShaderProgramRef m_dofProgram;
		NzTextureRef m_dofTextures[2];
		NzTextureSampler m_bilinearSampler;
		NzTextureSampler m_pointSampler;
		int m_blurProgramFilterLocation;
};

#endif // NAZARA_DEFERREDDOFPASS_HPP
