// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERABLE_HPP
#define NAZARA_RENDERABLE_HPP

#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>

class NzAbstractRenderQueue;
class NzRenderable;

using NzRenderableConstRef = NzObjectRef<const NzRenderable>;
using NzRenderableLibrary = NzObjectLibrary<NzRenderable>;
using NzRenderableRef = NzObjectRef<NzRenderable>;

class NAZARA_GRAPHICS_API NzRenderable : public NzRefCounted
{
	public:
		struct InstanceData;

		NzRenderable() = default;
		inline NzRenderable(const NzRenderable& renderable);
		virtual ~NzRenderable();

		inline void EnsureBoundingVolumeUpdated() const;

		virtual void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const InstanceData& instanceData) const = 0;
		virtual bool Cull(const NzFrustumf& frustum, const InstanceData& instanceData) const;
		virtual const NzBoundingVolumef& GetBoundingVolume() const;
		virtual void InvalidateData(InstanceData* instanceData, nzUInt32 flags) const;
		virtual void UpdateBoundingVolume(InstanceData* instanceData) const;
		virtual void UpdateData(InstanceData* instanceData) const;

		inline NzRenderable& operator=(const NzRenderable& renderable);

		// Signals:
		NazaraSignal(OnRenderableInvalidateInstanceData, const NzRenderable*, nzUInt32); //< Args: me, flags
		NazaraSignal(OnRenderableRelease, const NzRenderable*); //< Args: me

		struct InstanceData
		{
			InstanceData(NzMatrix4f& referenceMatrix) :
			transformMatrix(referenceMatrix),
			flags(0)
			{
			}

			std::vector<nzUInt8> data;
			NzBoundingVolumef volume;
			NzMatrix4f& transformMatrix;
			nzUInt32 flags;
		};

	protected:
		virtual void MakeBoundingVolume() const = 0;
		void InvalidateBoundingVolume();
		inline void InvalidateInstanceData(nzUInt32 flags);
		inline void UpdateBoundingVolume() const;

		mutable NzBoundingVolumef m_boundingVolume;

	private:
		mutable bool m_boundingVolumeUpdated;

		static NzRenderableLibrary::LibraryMap s_library;
};

#include <Nazara/Graphics/Renderable.inl>

#endif // NAZARA_RENDERABLE_HPP
