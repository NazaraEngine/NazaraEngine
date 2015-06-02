// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERABLE_HPP
#define NAZARA_RENDERABLE_HPP

#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>

class NzAbstractRenderQueue;
class NzRenderable;

using NzRenderableConstListener = NzObjectListenerWrapper<const NzRenderable>;
using NzRenderableConstRef = NzObjectRef<const NzRenderable>;
using NzRenderableLibrary = NzObjectLibrary<NzRenderable>;
using NzRenderableListener = NzObjectListenerWrapper<NzRenderable>;
using NzRenderableRef = NzObjectRef<NzRenderable>;

class NAZARA_API NzRenderable : public NzRefCounted
{
	public:
		NzRenderable() = default;
		inline NzRenderable(const NzRenderable& renderable);
		virtual ~NzRenderable();

		inline void EnsureBoundingVolumeUpdated() const;

		virtual void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const = 0;
		virtual bool Cull(const NzFrustumf& frustum, const NzBoundingVolumef& volume, const NzMatrix4f& transformMatrix) const;
		virtual const NzBoundingVolumef& GetBoundingVolume() const;
		virtual void UpdateBoundingVolume(NzBoundingVolumef* boundingVolume, const NzMatrix4f& transformMatrix) const;

		inline NzRenderable& operator=(const NzRenderable& renderable);

	protected:
		virtual void MakeBoundingVolume() const = 0;
		void InvalidateBoundingVolume();
		inline void UpdateBoundingVolume() const;

		mutable NzBoundingVolumef m_boundingVolume;

	private:
		mutable bool m_boundingVolumeUpdated;

		static NzRenderableLibrary::LibraryMap s_library;
};

#include <Nazara/Graphics/Renderable.inl>

#endif // NAZARA_RENDERABLE_HPP
