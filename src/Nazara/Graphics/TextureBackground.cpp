// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextureBackground.hpp>
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

NzTextureBackground::NzTextureBackground()
{
	m_uberShader = NzUberShaderLibrary::Get("Basic");

	NzParameterList list;
	list.SetParameter("DIFFUSE_MAPPING", true);
	list.SetParameter("TEXTURE_MAPPING", true);
	list.SetParameter("UNIFORM_VERTEX_DEPTH", true);

	m_uberShaderInstance = m_uberShader->Get(list);

	const NzShader* shader = m_uberShaderInstance->GetShader();
	m_materialDiffuseUniform = shader->GetUniformLocation("MaterialDiffuse");
	m_materialDiffuseMapUniform = shader->GetUniformLocation("MaterialDiffuseMap");
	m_vertexDepthUniform = shader->GetUniformLocation("VertexDepth");
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

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetTexture(0, m_texture);

	m_uberShaderInstance->Activate();

	const NzShader* shader = m_uberShaderInstance->GetShader();
	shader->SendColor(m_materialDiffuseUniform, NzColor::White);
	shader->SendFloat(m_vertexDepthUniform, 1.f);
	shader->SendInteger(m_materialDiffuseMapUniform, 0);

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
