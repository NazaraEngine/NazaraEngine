// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERSUBMESH_HPP
#define NAZARA_GRAPHICS_RENDERSUBMESH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class MaterialProxy;
	class RenderPipeline;
	class ShaderBinding;

	class RenderSubmesh : public RenderElement
	{
		public:
			inline RenderSubmesh(Int32 renderLayer, std::shared_ptr<MaterialProxy> materialProxy, MaterialPassFlags materialFlags, std::shared_ptr<RenderPipeline> renderPipeline, UInt32 instanceIndex, const SkeletonInstance* skeletonInstance, std::size_t indexCount, IndexType indexType, std::shared_ptr<RenderBuffer> indexBuffer, std::shared_ptr<RenderBuffer> vertexBuffer, const Recti& scissorBox, const Spheref& boundingSphere, UInt32 renderMask);
			~RenderSubmesh() = default;

			inline const Spheref& GetBoundingSphere() const;
			inline const RenderBuffer* GetIndexBuffer() const;
			inline std::size_t GetIndexCount() const;
			inline IndexType GetIndexType() const;
			inline UInt32 GetInstanceIndex() const;
			inline const MaterialProxy& GetMaterialProxy() const;
			inline const RenderPipeline* GetRenderPipeline() const;
			inline const Recti& GetScissorBox() const;
			inline const SkeletonInstance* GetSkeletonInstance() const;
			inline const RenderBuffer* GetVertexBuffer() const;

			inline void Register(RenderQueueRegistry& registry) const override;

			static constexpr BasicRenderElement ElementType = BasicRenderElement::Submesh;

		private:
			inline UInt64 ComputeSortKey(const RenderQueueRegistry& registry) const override;

			std::shared_ptr<RenderBuffer> m_indexBuffer;
			std::shared_ptr<RenderBuffer> m_vertexBuffer;
			std::shared_ptr<MaterialProxy> m_materialProxy;
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::size_t m_indexCount;
			const SkeletonInstance* m_skeletonInstance;
			IndexType m_indexType;
			MaterialPassFlags m_materialFlags;
			Recti m_scissorBox;
			Spheref m_boundingSphere;
			UInt32 m_instanceIndex;
	};
}

#include <Nazara/Graphics/RenderSubmesh.inl>

#endif // NAZARA_GRAPHICS_RENDERSUBMESH_HPP
