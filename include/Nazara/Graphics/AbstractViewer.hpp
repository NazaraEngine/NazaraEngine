// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTVIEWER_HPP
#define NAZARA_ABSTRACTVIEWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>

namespace Nz
{
	class RenderTarget;

	class NAZARA_GRAPHICS_API AbstractViewer
	{
		public:
			AbstractViewer() = default;
			AbstractViewer(const AbstractViewer&) = default;
			AbstractViewer(AbstractViewer&&) noexcept = default;
			virtual ~AbstractViewer();

			virtual void ApplyView() const = 0;

			virtual float GetAspectRatio() const = 0;
			virtual Vector3f GetEyePosition() const = 0;
			virtual Vector3f GetForward() const = 0;
			virtual const Frustumf& GetFrustum() const = 0;
			virtual const Matrix4f& GetProjectionMatrix() const = 0;
			virtual Nz::ProjectionType GetProjectionType() const = 0;
			virtual const RenderTarget* GetTarget() const = 0;
			virtual const Matrix4f& GetViewMatrix() const = 0;
			virtual const Recti& GetViewport() const = 0;
			virtual float GetZFar() const = 0;
			virtual float GetZNear() const = 0;

			AbstractViewer& operator=(const AbstractViewer&) = default;
			AbstractViewer& operator=(AbstractViewer&&) noexcept = default;
	};
}

#endif // NAZARA_ABSTRACTVIEWER_HPP
