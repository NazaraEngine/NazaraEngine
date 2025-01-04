// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ABSTRACTVIEWER_HPP
#define NAZARA_GRAPHICS_ABSTRACTVIEWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class DebugDrawer;
	class RenderTarget;
	class ViewerInstance;

	class NAZARA_GRAPHICS_API AbstractViewer
	{
		public:
			AbstractViewer() = default;
			virtual ~AbstractViewer();

			virtual const Color& GetClearColor() const = 0;
			virtual float GetClearDepth() const = 0;
			virtual DebugDrawer* GetDebugDrawer() = 0;
			virtual UInt32 GetRenderMask() const = 0;
			virtual const RenderTarget& GetRenderTarget() const = 0;
			virtual ViewerInstance& GetViewerInstance() = 0;
			virtual const ViewerInstance& GetViewerInstance() const = 0;
			virtual const Recti& GetViewport() const = 0;

			virtual bool IsZReversed() const = 0;

			Vector3f ProjectToClipspace(const Vector3f& worldPos) const;
			Vector3f ProjectToScreen(const Vector3f& worldPos) const;
			Vector3f ProjectToScreen(const Matrix4f& viewProjMatrix, const Vector3f& worldPos) const;

			Vector3f UnprojectFromClipspace(const Vector3f& clipSpace) const;
			Vector3f UnprojectFromScreen(const Vector3f& screenPos) const;
			Vector3f UnprojectFromScreen(const Matrix4f& invViewProjMatrix, const Vector3f& screenPos) const;

			static inline Vector3f ProjectToClipspace(const Matrix4f& viewProjMatrix, const Vector3f& worldPos);
			static inline Vector3f UnprojectFromClipspace(const Matrix4f& viewProjMatrix, const Vector3f& clipSpace);

			NazaraSignal(OnRenderMaskUpdated, AbstractViewer* /*viewer*/, UInt32 /*newRenderMask*/);
	};
}

#include <Nazara/Graphics/AbstractViewer.inl>

#endif // NAZARA_GRAPHICS_ABSTRACTVIEWER_HPP
