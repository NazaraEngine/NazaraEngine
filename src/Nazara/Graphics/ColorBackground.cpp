// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ColorBackGround.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UberShaderLibrary.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzRenderStates BuildRenderStates()
	{
		NzRenderStates states;
		states.depthFunc = nzRendererComparison_Equal;
		states.faceCulling = nzFaceSide_Back;
		states.parameters[nzRendererParameter_DepthBuffer] = true;
		states.parameters[nzRendererParameter_DepthWrite] = false;
		states.parameters[nzRendererParameter_FaceCulling] = true;

		return states;
	}
}

NzColorBackground::NzColorBackground(const NzColor& color) :
m_color(color)
{
	m_uberShader = NzUberShaderLibrary::Get("Basic");

	NzParameterList list;
	list.SetParameter("UNIFORM_VERTEX_DEPTH", true);
	m_uberShaderInstance = m_uberShader->Get(list);

	const NzShader* shader = m_uberShaderInstance->GetShader();
	m_materialDiffuseUniform = shader->GetUniformLocation("MaterialDiffuse");
	m_vertexDepthUniform = shader->GetUniformLocation("VertexDepth");
}

void NzColorBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	static NzRenderStates states(BuildRenderStates());

	NzRenderer::SetRenderStates(states);

	m_uberShaderInstance->Activate();

	const NzShader* shader = m_uberShaderInstance->GetShader();
	shader->SendColor(m_materialDiffuseUniform, m_color);
	shader->SendFloat(m_vertexDepthUniform, 1.f);

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

