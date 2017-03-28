// Copyright (C) 2017 Jérôme Leclercq
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
#include <Nazara/Graphics/CullingList.hpp>
#include <Nazara/Graphics/Material.hpp>
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

			inline InstancedRenderable();
			inline InstancedRenderable(const InstancedRenderable& renderable);
			InstancedRenderable(InstancedRenderable&& renderable) = delete;
			virtual ~InstancedRenderable();

			virtual void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const = 0;

			virtual bool Cull(const Frustumf& frustum, const InstanceData& instanceData) const;

			inline void EnsureBoundingVolumeUpdated() const;

			virtual const BoundingVolumef& GetBoundingVolume() const;

			inline const MaterialRef& GetMaterial(std::size_t matIndex = 0) const;
			inline const MaterialRef& GetMaterial(std::size_t skinIndex, std::size_t matIndex) const;
			inline std::size_t GetMaterialCount() const;
			inline std::size_t GetSkin() const;
			inline std::size_t GetSkinCount() const;

			virtual void InvalidateData(InstanceData* instanceData, UInt32 flags) const;

			inline void SetSkin(std::size_t skinIndex);
			inline void SetSkinCount(std::size_t skinCount);

			virtual void UpdateBoundingVolume(InstanceData* instanceData) const;
			virtual void UpdateData(InstanceData* instanceData) const;

			inline InstancedRenderable& operator=(const InstancedRenderable& renderable);
			InstancedRenderable& operator=(InstancedRenderable&& renderable) = delete;

			// Signals:
			NazaraSignal(OnInstancedRenderableInvalidateBoundingVolume, const InstancedRenderable* /*instancedRenderable*/);
			NazaraSignal(OnInstancedRenderableInvalidateData, const InstancedRenderable* /*instancedRenderable*/, UInt32 /*flags*/);
			NazaraSignal(OnInstancedRenderableInvalidateMaterial, const InstancedRenderable* /*instancedRenderable*/, std::size_t /*skinIndex*/, std::size_t /*matIndex*/, const MaterialRef& /*newMat*/);
			NazaraSignal(OnInstancedRenderableRelease, const InstancedRenderable* /*instancedRenderable*/);
			NazaraSignal(OnInstancedRenderableResetMaterials, const InstancedRenderable* /*instancedRenderable*/, std::size_t /*newMaterialCount*/);
			NazaraSignal(OnInstancedRenderableSkinChange, const InstancedRenderable* /*instancedRenderable*/, std::size_t /*newSkinIndex*/);

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
			inline void InvalidateBoundingVolume();
			inline void InvalidateInstanceData(UInt32 flags);

			virtual void MakeBoundingVolume() const = 0;

			inline void ResetMaterials(std::size_t matCount, std::size_t skinCount = 1);

			inline void SetMaterial(std::size_t matIndex, MaterialRef material);
			inline void SetMaterial(std::size_t skinIndex, std::size_t matIndex, MaterialRef material);

			mutable BoundingVolumef m_boundingVolume;

		private:
			inline void UpdateBoundingVolume() const;

			std::size_t m_matCount;
			std::size_t m_skin;
			std::size_t m_skinCount;
			std::vector<MaterialRef> m_materials;
			mutable bool m_boundingVolumeUpdated;

			static InstancedRenderableLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/InstancedRenderable.inl>

#endif // NAZARA_INSTANCEDRENDERABLE_HPP
