// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredForwardPass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::DeferredForwardPass
	* \brief Graphics class that represents the forward pass in deferred rendering
	*/

	DeferredForwardPass::DeferredForwardPass() = default;
	DeferredForwardPass::~DeferredForwardPass() = default;

	/*!
	* \brief Initializes the deferred forward pass which needs the forward technique
	*
	* \param technique Rendering technique
	*/

	void DeferredForwardPass::Initialize(DeferredRenderTechnique* technique)
	{
		DeferredRenderPass::Initialize(technique);

		m_forwardTechnique = technique->GetForwardTechnique();
	}

	/*!
	* \brief Processes the work on the data while working with textures
	* \return true
	*
	* \param sceneData Data for the scene
	* \param firstWorkTexture Index of the first texture to work with
	* \param firstWorkTexture Index of the second texture to work with
	*/

	bool DeferredForwardPass::Process(const SceneData& sceneData, unsigned int workTexture, unsigned int sceneTexture) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");
		NazaraUnused(workTexture);

		m_workRTT->SetColorTarget(sceneTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		if (sceneData.background)
			sceneData.background->Draw(sceneData.viewer);

		Renderer::SetMatrix(MatrixType_Projection, sceneData.viewer->GetProjectionMatrix());
		Renderer::SetMatrix(MatrixType_View, sceneData.viewer->GetViewMatrix());

		m_forwardTechnique->Draw(sceneData);

		return false;
	}
}
