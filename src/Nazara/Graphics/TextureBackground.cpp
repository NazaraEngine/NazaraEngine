// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextureBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzRenderStates BuildRenderStates()
	{
		NzRenderStates states;
		states.parameters[nzRendererParameter_DepthBuffer] = false;

		return states;
	}
}

NzTextureBackground::NzTextureBackground()
{
	NzShaderProgramManagerParams params;
	params.target = nzShaderTarget_FullscreenQuad;
	params.flags = 0;
	params.fullscreenQuad.alphaMapping = false;
	params.fullscreenQuad.alphaTest = false;
	params.fullscreenQuad.diffuseMapping = true;

	m_program = NzShaderProgramManager::Get(params);
}

NzTextureBackground::NzTextureBackground(NzTexture* texture) :
NzTextureBackground()
{
	m_texture = texture;
}

void NzTextureBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states(BuildRenderStates());

	m_program->SendColor(m_program->GetUniformLocation(nzShaderUniform_MaterialDiffuse), NzColor::White);
	m_program->SendInteger(m_program->GetUniformLocation(nzShaderUniform_MaterialDiffuseMap), 0);

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShaderProgram(m_program);
	NzRenderer::SetTexture(0, m_texture);

	NzRenderer::DrawFullscreenQuad();
}

nzBackgroundType NzTextureBackground::GetBackgroundType() const
{
	return nzBackgroundType_Texture;
}

NzTexture* NzTextureBackground::GetTexture() const
{
	return m_texture;
}

void NzTextureBackground::SetTexture(NzTexture* texture)
{
	m_texture = texture;
}
