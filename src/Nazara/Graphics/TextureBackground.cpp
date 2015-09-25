// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextureBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		RenderStates BuildRenderStates()
		{
			RenderStates states;
			states.depthFunc = RendererComparison_Equal;
			states.faceCulling = FaceSide_Back;
			states.parameters[RendererParameter_DepthBuffer] = true;
			states.parameters[RendererParameter_DepthWrite] = false;
			states.parameters[RendererParameter_FaceCulling] = true;

			return states;
		}
	}

	TextureBackground::TextureBackground(TextureRef texture)
	{
		m_uberShader = UberShaderLibrary::Get("Basic");

		ParameterList list;
		list.SetParameter("DIFFUSE_MAPPING", true);
		list.SetParameter("TEXTURE_MAPPING", true);
		list.SetParameter("UNIFORM_VERTEX_DEPTH", true);

		m_uberShaderInstance = m_uberShader->Get(list);

		const Shader* shader = m_uberShaderInstance->GetShader();
		m_materialDiffuseUniform = shader->GetUniformLocation("MaterialDiffuse");
		m_materialDiffuseMapUniform = shader->GetUniformLocation("MaterialDiffuseMap");
		m_vertexDepthUniform = shader->GetUniformLocation("VertexDepth");

		SetTexture(std::move(texture));
	}

	void TextureBackground::Draw(const AbstractViewer* viewer) const
	{
		NazaraUnused(viewer);

		static RenderStates states(BuildRenderStates());

		Renderer::SetRenderStates(states);
		Renderer::SetTexture(0, m_texture);

		m_uberShaderInstance->Activate();

		const Shader* shader = m_uberShaderInstance->GetShader();
		shader->SendColor(m_materialDiffuseUniform, Color::White);
		shader->SendFloat(m_vertexDepthUniform, 1.f);
		shader->SendInteger(m_materialDiffuseMapUniform, 0);

		Renderer::DrawFullscreenQuad();
	}

	BackgroundType TextureBackground::GetBackgroundType() const
	{
		return BackgroundType_Texture;
	}
}
