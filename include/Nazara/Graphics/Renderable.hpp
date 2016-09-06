// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERABLE_HPP
#define NAZARA_RENDERABLE_HPP

#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>

namespace Nz
{
	class AbstractRenderQueue;

	class NAZARA_GRAPHICS_API Renderable
	{
		public:
			Renderable() = default;
			Renderable(const Renderable& renderable) = default;
			Renderable(Renderable&&) = default;
			virtual ~Renderable();

			virtual void AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix) const = 0;

			virtual bool Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const;

			inline void EnsureBoundingVolumeUpdated() const;
			virtual const BoundingVolumef& GetBoundingVolume() const;
			virtual void UpdateBoundingVolume(const Matrix4f& transformMatrix);

			Renderable& operator=(const Renderable& renderable) = default;
			Renderable& operator=(Renderable&& renderable) = default;

		protected:
			virtual void MakeBoundingVolume() const = 0;
			inline void InvalidateBoundingVolume();

			mutable BoundingVolumef m_boundingVolume;

		private:
			inline void UpdateBoundingVolume() const;

			mutable bool m_boundingVolumeUpdated;
	};
}

#include <Nazara/Graphics/Renderable.inl>

#endif // NAZARA_RENDERABLE_HPP
