// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredForwardPass.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredForwardPass::NzDeferredForwardPass() = default;
NzDeferredForwardPass::~NzDeferredForwardPass() = default;

void NzDeferredForwardPass::Initialize(NzDeferredRenderTechnique* technique)
{
	NzDeferredRenderPass::Initialize(technique);

	m_forwardTechnique = technique->GetForwardTechnique();
}

bool NzDeferredForwardPass::Process(const NzScene* scene, unsigned int workTexture, unsigned sceneTexture) const
{
	NazaraUnused(workTexture);

	m_workRTT->SetColorTarget(sceneTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzAbstractBackground* background = scene->GetBackground();
	if (background)
		background->Draw(scene);

	NzAbstractViewer* viewer = scene->GetViewer();

	NzRenderer::SetMatrix(nzMatrixType_Projection, viewer->GetProjectionMatrix());
	NzRenderer::SetMatrix(nzMatrixType_View, viewer->GetViewMatrix());

	m_forwardTechnique->Draw(scene);

	return false;
}

