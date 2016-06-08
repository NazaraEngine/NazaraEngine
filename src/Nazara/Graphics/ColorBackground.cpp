// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		/*!
		* \brief Defines render states
		* \return RenderStates for the color background
		*/

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

	/*!
	* \ingroup graphics
	* \class Nz::ColorBackground
	* \brief Graphics class that represents a background with uniform color
	*/

	/*!
	* \brief Constructs a ColorBackground object with a color
	*
	* \param color Uniform color (by default Black)
	*/

	ColorBackground::ColorBackground(const Color& color) :
	m_color(color)
	{
		m_uberShader = UberShaderLibrary::Get("Basic");

		ParameterList list;
		list.SetParameter("UNIFORM_VERTEX_DEPTH", true);
		m_uberShaderInstance = m_uberShader->Get(list);

		const Shader* shader = m_uberShaderInstance->GetShader();
		m_materialDiffuseUniform = shader->GetUniformLocation("MaterialDiffuse");
		m_vertexDepthUniform = shader->GetUniformLocation("VertexDepth");
	}

	/*!
	* \brief Draws this relatively to the viewer
	*
	* \param viewer Viewer for the background
	*/

	void ColorBackground::Draw(const AbstractViewer* viewer) const
	{
		NazaraUnused(viewer);

		static RenderStates states(BuildRenderStates());

		Renderer::SetRenderStates(states);

		m_uberShaderInstance->Activate();

		const Shader* shader = m_uberShaderInstance->GetShader();
		shader->SendColor(m_materialDiffuseUniform, m_color);
		shader->SendFloat(m_vertexDepthUniform, 1.f);

		Renderer::DrawFullscreenQuad();
	}

	/*!
	* \brief Gets the background type
	* \return Type of background
	*/

	BackgroundType ColorBackground::GetBackgroundType() const
	{
		return BackgroundType_Color;
	}

	/*!
	* \brief Gets the color of the background
	* \return Background color
	*/

	Color ColorBackground::GetColor() const
	{
		return m_color;
	}

	/*!
	* \brief Sets the color of the background
	*
	* \param color Background color
	*/

	void ColorBackground::SetColor(const Color& color)
	{
		m_color = color;
	}
}
