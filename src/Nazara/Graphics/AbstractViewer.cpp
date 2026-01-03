// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>

namespace Nz
{
	AbstractViewer::~AbstractViewer() = default;

	Vector3f AbstractViewer::ProjectToClipspace(const Vector3f& worldPos) const
	{
		return ProjectToClipspace(GetViewerInstance().GetViewProjMatrix(), worldPos);
	}

	Vector3f AbstractViewer::ProjectToScreen(const Vector3f& worldPos) const
	{
		return ProjectToScreen(GetViewerInstance().GetViewProjMatrix(), worldPos);
	}

	Vector3f AbstractViewer::ProjectToScreen(const Matrix4f& viewProjMatrix, const Vector3f& worldPos) const
	{
		Vector3f clipspace = ProjectToClipspace(viewProjMatrix, worldPos);
		Vector2f screenSize = Vector2f(GetRenderTarget().GetSize());

		Vector3f screenSpace;
		screenSpace.x = (clipspace.x * 0.5f + 0.5f) * screenSize.x;
		screenSpace.y = (clipspace.y * -0.5f + 0.5f) * screenSize.y;
		screenSpace.z = clipspace.z;

		return screenSpace;
	}

	Vector3f AbstractViewer::UnprojectFromClipspace(const Vector3f& clipSpace) const
	{
		return UnprojectFromClipspace(GetViewerInstance().GetInvViewProjMatrix(), clipSpace);
	}

	Vector3f AbstractViewer::UnprojectFromScreen(const Vector3f& screenPos) const
	{
		return UnprojectFromScreen(GetViewerInstance().GetInvViewProjMatrix(), screenPos);
	}

	Vector3f AbstractViewer::UnprojectFromScreen(const Matrix4f& invViewProjMatrix, const Vector3f& screenPos) const
	{
		Vector2f screenSize = Vector2f(GetRenderTarget().GetSize());

		// Screen space => clip space
		Vector3f clipSpace;
		clipSpace.x = screenPos.x / screenSize.x * 2.f - 1.f;
		clipSpace.y = screenPos.y / screenSize.y * -2.f - 1.f;
		clipSpace.z = screenPos.z;

		return UnprojectFromClipspace(invViewProjMatrix, clipSpace);
	}
}
