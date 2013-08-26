// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTVIEWER_HPP
#define NAZARA_ABSTRACTVIEWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>

class NzRenderTarget;
class NzScene;

class NAZARA_API NzAbstractViewer
{
	public:
		NzAbstractViewer() = default;
		virtual ~NzAbstractViewer();

		virtual void ApplyView() const = 0;

		virtual float GetAspectRatio() const = 0;
		virtual NzVector3f GetEyePosition() const = 0;
		virtual NzVector3f GetForward() const = 0;
		virtual const NzFrustumf& GetFrustum() const = 0;
		virtual const NzMatrix4f& GetProjectionMatrix() const = 0;
		virtual const NzRenderTarget* GetTarget() const = 0;
		virtual const NzMatrix4f& GetViewMatrix() const = 0;
		virtual const NzRecti& GetViewport() const = 0;
		virtual float GetZFar() const = 0;
		virtual float GetZNear() const = 0;
};

#endif // NAZARA_ABSTRACTVIEWER_HPP
