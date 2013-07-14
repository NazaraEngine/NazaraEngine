// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextureBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderManager.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzTextureBackground::NzTextureBackground()
{
	NzShaderManagerParams params;
	params.target = nzShaderTarget_FullscreenQuad;
	params.flags = 0;
	params.fullscreenQuad.alphaMapping = false;
	params.fullscreenQuad.alphaTest = false;
	params.fullscreenQuad.diffuseMapping = true;

	m_shader = NzShaderManager::Get(params);
}

NzTextureBackground::NzTextureBackground(NzTexture* texture) :
NzTextureBackground()
{
	m_texture = texture;
}

void NzTextureBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states;

	m_shader->SendColor(m_shader->GetUniformLocation(nzShaderUniform_MaterialDiffuse), NzColor::White);
	m_shader->SendInteger(m_shader->GetUniformLocation(nzShaderUniform_MaterialDiffuseMap), 0);

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShader(m_shader);
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
