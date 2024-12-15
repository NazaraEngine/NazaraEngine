// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PipelinePassList.hpp>
#include <cassert>
#include <stdexcept>

namespace Nz
{
	Camera::Camera(std::shared_ptr<const RenderTarget> renderTarget, ProjectionType projectionType) :
	Camera(std::move(renderTarget), Graphics::Instance()->GetDefaultPipelinePasses(), projectionType)
	{
	}

	DebugDrawer* Camera::AccessDebugDrawer()
	{
		// FIXME: If reversed depth is turned on/off after creating the debug drawer the debug drawer will not follow
		if (!m_debugDrawer)
			m_debugDrawer = std::make_unique<DebugDrawer>(*Graphics::Instance()->GetRenderDevice(), m_isReversedZEnabled);

		return m_debugDrawer.get();
	}

	std::vector<std::unique_ptr<FramePipelinePass>> Camera::BuildPasses(FramePipelinePass::PassData& passData) const
	{
		assert(m_framePipelinePasses);
		return m_framePipelinePasses->BuildPasses(passData);
	}

	const Color& Camera::GetClearColor() const
	{
		return m_clearColor;
	}

	float Camera::GetClearDepth() const
	{
		return m_clearDepth;
	}

	UInt32 Camera::GetRenderMask() const
	{
		return m_renderMask;
	}

	const RenderTarget& Camera::GetRenderTarget() const
	{
		if (!m_renderTarget)
			throw std::runtime_error("no rendertarget set");

		return* m_renderTarget;
	}

	ViewerInstance& Camera::GetViewerInstance()
	{
		return m_viewerInstance;
	}

	const ViewerInstance& Camera::GetViewerInstance() const
	{
		return m_viewerInstance;
	}

	const Recti& Camera::GetViewport() const
	{
		return m_viewport;
	}

	bool Camera::IsZReversed() const
	{
		return IsReversedZEnabled();
	}

	std::size_t Camera::RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph, std::optional<unsigned int> viewerIndex, const FunctionRef<void(std::size_t passIndex, FramePass& framePass, FramePipelinePassFlags flags)>& passCallback) const
	{
		assert(m_framePipelinePasses);
		return m_framePipelinePasses->RegisterPasses(passes, frameGraph, viewerIndex, passCallback);
	}

	void Camera::UpdateTarget(std::shared_ptr<const RenderTarget> renderTarget)
	{
		m_onRenderTargetRelease.Disconnect();
		m_onRenderTargetSizeChange.Disconnect();

		m_renderTarget = std::move(renderTarget);
		if (m_renderTarget)
		{
			m_onRenderTargetRelease.Connect(m_renderTarget->OnRenderTargetRelease, [this](const RenderTarget*)
			{
				UpdateTarget(nullptr);
			});

			m_onRenderTargetSizeChange.Connect(m_renderTarget->OnRenderTargetSizeChange, [this](const RenderTarget*, const Vector2ui& newSize)
			{
				UpdateViewport(newSize);
			});

			UpdateViewport();
		}
	}

	DebugDrawer* Camera::GetDebugDrawer()
	{
		return m_debugDrawer.get();
	}
}
