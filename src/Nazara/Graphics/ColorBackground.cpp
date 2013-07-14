// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ColorBackGround.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderManager.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzColorBackground::NzColorBackground(const NzColor& color) :
m_color(color)
{
	NzShaderManagerParams params;
	params.target = nzShaderTarget_FullscreenQuad;
	params.flags = 0;
	params.fullscreenQuad.alphaMapping = false;
	params.fullscreenQuad.alphaTest = false;
	params.fullscreenQuad.diffuseMapping = false;

	m_shader = NzShaderManager::Get(params);
}

void NzColorBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states;

	m_shader->SendColor(m_shader->GetUniformLocation(nzShaderUniform_MaterialDiffuse), m_color);

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShader(m_shader);

	NzRenderer::DrawFullscreenQuad();
}

nzBackgroundType NzColorBackground::GetBackgroundType() const
{
	return nzBackgroundType_Color;
}

NzColor NzColorBackground::GetColor() const
{
	return m_color;
}

void NzColorBackground::SetColor(const NzColor& color)
{
	m_color = color;
}

