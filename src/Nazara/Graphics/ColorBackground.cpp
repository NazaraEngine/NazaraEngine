// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ColorBackGround.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzRenderStates BuildRenderStates()
	{
		NzRenderStates states;
		states.depthFunc = nzRendererComparison_Equal;
		states.faceCulling = nzFaceSide_Front;
		states.parameters[nzRendererParameter_DepthBuffer] = true;
		states.parameters[nzRendererParameter_DepthWrite] = false;
		states.parameters[nzRendererParameter_FaceCulling] = true;

		return states;
	}
}

NzColorBackground::NzColorBackground(const NzColor& color) :
m_color(color)
{
	NzShaderProgramManagerParams params;
	params.target = nzShaderTarget_FullscreenQuad;
	params.flags = 0;
	params.fullscreenQuad.alphaMapping = false;
	params.fullscreenQuad.alphaTest = false;
	params.fullscreenQuad.diffuseMapping = false;

	m_program = NzShaderProgramManager::Get(params);
	m_program->SendColor(m_program->GetUniformLocation(nzShaderUniform_MaterialDiffuse), m_color);
	m_program->SendFloat(m_program->GetUniformLocation(nzShaderUniform_VertexDepth), 1.f);
}

void NzColorBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states(BuildRenderStates());

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShaderProgram(m_program);

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
	m_program->SendColor(m_program->GetUniformLocation(nzShaderUniform_MaterialDiffuse), m_color);
}

