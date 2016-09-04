// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INSTANCEDRENDERABLE_HPP
#define NAZARA_INSTANCEDRENDERABLE_HPP

#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>

namespace Nz
{
	class AbstractRenderQueue;
	class InstancedRenderable;

	using InstancedRenderableConstRef = ObjectRef<const InstancedRenderable>;
	using InstancedRenderableLibrary = ObjectLibrary<InstancedRenderable>;
	using InstancedRenderableRef = ObjectRef<InstancedRenderable>;

	class NAZARA_GRAPHICS_API InstancedRenderable : public RefCounted
	{
		public:
			struct InstanceData;

			InstancedRenderable() = default;
			inline InstancedRenderable(const InstancedRenderable& renderable);
			InstancedRenderable(InstancedRenderable&& renderable) = delete;
			virtual ~InstancedRenderable();

			inline void EnsureBoundingVolumeUpdated() const;

			virtual void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const = 0;
			virtual bool Cull(const Frustumf& frustum, const InstanceData& instanceData) const;
			virtual const BoundingVolumef& GetBoundingVolume() const;
			virtual void InvalidateData(InstanceData* instanceData, UInt32 flags) const;
			virtual void UpdateBoundingVolume(InstanceData* instanceData) const;
			virtual void UpdateData(InstanceData* instanceData) const;

			inline InstancedRenderable& operator=(const InstancedRenderable& renderable);
			InstancedRenderable& operator=(InstancedRenderable&& renderable) = delete;

			// Signals:
			NazaraSignal(OnInstancedRenderableInvalidateData, const InstancedRenderable* /*instancedRenderable*/, UInt32 /*flags*/);
			NazaraSignal(OnInstancedRenderableRelease, const InstancedRenderable* /*instancedRenderable*/);

			struct InstanceData
			{
				InstanceData(const Matrix4f& transformationMatrix) :
				localMatrix(transformationMatrix),
				flags(0)
				{
				}

				InstanceData(InstanceData&& instanceData) noexcept = default;

				InstanceData& operator=(InstanceData&& instanceData) noexcept
				{
					data = std::move(instanceData.data);
					flags = instanceData.flags;
					renderOrder = instanceData.renderOrder;
					localMatrix = instanceData.localMatrix;
					transformMatrix = instanceData.transformMatrix;
					volume = instanceData.volume;

					return *this;
				}

				std::vector<UInt8> data;
				BoundingVolumef volume;
				Matrix4f localMatrix;
				mutable Matrix4f transformMatrix;
				UInt32 flags;
				int renderOrder;
			};

		protected:
			virtual void MakeBoundingVolume() const = 0;
			void InvalidateBoundingVolume();
			inline void InvalidateInstanceData(UInt32 flags);
			inline void UpdateBoundingVolume() const;

			mutable BoundingVolumef m_boundingVolume;

		private:
			mutable bool m_boundingVolumeUpdated;

			static InstancedRenderableLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/InstancedRenderable.inl>

#endif // NAZARA_INSTANCEDRENDERABLE_HPP
