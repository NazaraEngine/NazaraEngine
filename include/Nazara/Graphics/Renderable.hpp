// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERABLE_HPP
#define NAZARA_RENDERABLE_HPP

#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>

class NzAbstractRenderQueue;

class NAZARA_GRAPHICS_API NzRenderable
{
	public:
		NzRenderable() = default;
		NzRenderable(const NzRenderable& renderable) = default;
		virtual ~NzRenderable();

		void EnsureBoundingVolumeUpdated() const;

		virtual void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const = 0;
		virtual bool Cull(const NzFrustumf& frustum, const NzMatrix4f& transformMatrix) const = 0;
		virtual const NzBoundingVolumef& GetBoundingVolume() const;
		virtual void UpdateBoundingVolume(const NzMatrix4f& transformMatrix);

		NzRenderable& operator=(const NzRenderable& renderable) = default;

	protected:
		virtual void MakeBoundingVolume() const = 0;
		void InvalidateBoundingVolume();
		void UpdateBoundingVolume() const;

		mutable NzBoundingVolumef m_boundingVolume;

	private:
		mutable bool m_boundingVolumeUpdated;
};

#include <Nazara/Graphics/Renderable.inl>

#endif // NAZARA_RENDERABLE_HPP
