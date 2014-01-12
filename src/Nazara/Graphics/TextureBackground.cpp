// Copyright (C) 2014 Jérôme Leclercq
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

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShaderProgram(m_program);
	NzRenderer::SetTexture(0, m_texture);

	m_program->SendColor(m_program->GetUniformLocation(nzShaderUniform_MaterialDiffuse), NzColor::White);
	m_program->SendFloat(m_program->GetUniformLocation(nzShaderUniform_VertexDepth), 1.f);
	m_program->SendInteger(m_program->GetUniformLocation(nzShaderUniform_MaterialDiffuseMap), 0);

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
