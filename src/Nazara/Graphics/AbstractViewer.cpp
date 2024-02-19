// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>

namespace Nz
{
	AbstractViewer::~AbstractViewer() = default;

	Vector3f AbstractViewer::Project(const Vector3f& worldPos) const
	{
		const Matrix4f& viewProj = GetViewerInstance().GetViewProjMatrix();

		Vector4f clipspace = viewProj * Vector4f(worldPos, 1.f);
		clipspace.x = clipspace.x / clipspace.w;
		clipspace.y = clipspace.y / clipspace.w;
		clipspace.z = clipspace.z / clipspace.w;

		Vector2f screenSize = Vector2f(GetRenderTarget().GetSize());

		Vector3f screenSpace;
		screenSpace.x = (clipspace.x * 0.5f + 0.5f) * screenSize.x;
		screenSpace.y = (clipspace.y * 0.5f + 0.5f) * screenSize.y;
		screenSpace.z = clipspace.z;

		return screenSpace;
	}

	Vector3f AbstractViewer::Unproject(const Vector3f& screenPos) const
	{
		Vector2f screenSize = Vector2f(GetRenderTarget().GetSize());

		// Screen space => clip space
		Vector4f clipSpace;
		clipSpace.x = screenPos.x / screenSize.x * 2.f - 1.f;
		clipSpace.y = screenPos.y / screenSize.y * 2.f - 1.f;
		clipSpace.z = screenPos.z;
		clipSpace.w = 1.f;

		// Clip space => projection space => view space => world space
		const Matrix4f& inverseViewProj = GetViewerInstance().GetInvViewProjMatrix();

		Vector4f unproj = inverseViewProj * clipSpace;
		return Vector3f(unproj.x, unproj.y, unproj.z) / unproj.w;
	}
}
