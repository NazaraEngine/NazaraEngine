// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::DeferredRenderPass
	* \brief Graphics class that represents the pass for rendering in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredRenderPass object by default
	*/

	DeferredRenderPass::DeferredRenderPass() :
	m_enabled(true)
	{
	}

	DeferredRenderPass::~DeferredRenderPass() = default;

	/*!
	* \brief Enables the deferred rendering
	*
	* \param enable Should deferred rendering be activated
	*/

	void DeferredRenderPass::Enable(bool enable)
	{
		m_enabled = enable;
	}

	/*!
	* \brief Initializes the deferred forward pass which needs the deferred technique
	*
	* \param technique Rendering technique
	*/

	void DeferredRenderPass::Initialize(DeferredRenderTechnique* technique)
	{
		m_deferredTechnique = technique;
		m_renderQueue = static_cast<DeferredRenderQueue*>(technique->GetRenderQueue());

		m_depthStencilTexture = technique->GetDepthStencilTexture();

		m_GBufferRTT = technique->GetGBufferRTT();
		for (unsigned int i = 0; i < 3; ++i)
			m_GBuffer[i] = technique->GetGBuffer(i);

		m_workRTT = technique->GetWorkRTT();
		for (unsigned int i = 0; i < 2; ++i)
			m_workTextures[i] = technique->GetWorkTexture(i);
	}

	/*!
	* \brief Checks whether the deferred rendering is enabled
	* \return true If it the case
	*/

	bool DeferredRenderPass::IsEnabled() const
	{
		return m_enabled;
	}

	/*!
	* \brief Resizes the texture sizes
	* \return true If successful
	*
	* \param dimensions Dimensions for the compute texture
	*/

	bool DeferredRenderPass::Resize(const Vector2ui& dimensions)
	{
		m_dimensions = dimensions;

		return true;
	}
}
