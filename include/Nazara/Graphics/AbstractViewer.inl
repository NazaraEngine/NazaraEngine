// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline Vector3f AbstractViewer::ProjectToClipspace(const Matrix4f& viewProjMatrix, const Vector3f& worldPos)
	{
		Vector4f clipspace = viewProjMatrix * Vector4f(worldPos, 1.f);
		clipspace.x = clipspace.x / clipspace.w;
		clipspace.y = clipspace.y / clipspace.w;
		clipspace.z = clipspace.z / clipspace.w;

		return Vector3f(clipspace);
	}

	inline Vector3f AbstractViewer::UnprojectFromClipspace(const Matrix4f& invViewProjMatrix, const Vector3f& clipSpace)
	{
		// Clip space => projection space => view space => world space
		Vector4f unproj = invViewProjMatrix * Vector4f(clipSpace, 1.f);
		return Vector3f(unproj.x, unproj.y, unproj.z) / unproj.w;
	}
}
