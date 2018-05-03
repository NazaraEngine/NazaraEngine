// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Systems/DebugSystem.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <NDK/Components/CollisionComponent3D.hpp>
#include <NDK/Components/DebugComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	namespace
	{
		class DebugRenderable : public Nz::InstancedRenderable
		{
			public:
				DebugRenderable(Ndk::Entity* owner, Nz::MaterialRef mat, Nz::IndexBufferRef indexBuffer, Nz::VertexBufferRef vertexBuffer) :
				m_entityOwner(owner),
				m_material(std::move(mat)),
				m_indexBuffer(std::move(indexBuffer)),
				m_vertexBuffer(std::move(vertexBuffer))
				{
					ResetMaterials(1);

					m_meshData.indexBuffer = m_indexBuffer;
					m_meshData.primitiveMode = Nz::PrimitiveMode_LineList;
					m_meshData.vertexBuffer = m_vertexBuffer;
				}

				void UpdateBoundingVolume(InstanceData* instanceData) const override
				{
				}

				void MakeBoundingVolume() const override
				{
					m_boundingVolume.MakeNull();
				}

			protected:
				Ndk::EntityHandle m_entityOwner;
				Nz::IndexBufferRef m_indexBuffer;
				Nz::MaterialRef m_material;
				Nz::MeshData m_meshData;
				Nz::VertexBufferRef m_vertexBuffer;
		};

		class AABBDebugRenderable : public DebugRenderable
		{
			public:
				using DebugRenderable::DebugRenderable;

				void AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Nz::Recti& scissorRect) const override
				{
					NazaraAssert(m_entityOwner, "DebugRenderable has no owner");

					const DebugComponent& entityDebug = m_entityOwner->GetComponent<DebugComponent>();
					const GraphicsComponent& entityGfx = m_entityOwner->GetComponent<GraphicsComponent>();

					Nz::Matrix4f transformMatrix = Nz::Matrix4f::Identity();
					transformMatrix.SetScale(entityGfx.GetBoundingVolume().aabb.GetLengths());
					transformMatrix.SetTranslation(entityGfx.GetBoundingVolume().aabb.GetCenter());

					renderQueue->AddMesh(0, m_material, m_meshData, Nz::Boxf::Zero(), transformMatrix, scissorRect);
				}
		};

		class OBBDebugRenderable : public DebugRenderable
		{
			public:
				using DebugRenderable::DebugRenderable;

				void AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Nz::Recti& scissorRect) const override
				{
					NazaraAssert(m_entityOwner, "DebugRenderable has no owner");

					const DebugComponent& entityDebug = m_entityOwner->GetComponent<DebugComponent>();
					const GraphicsComponent& entityGfx = m_entityOwner->GetComponent<GraphicsComponent>();

					Nz::Matrix4f transformMatrix = instanceData.transformMatrix;
					transformMatrix.ApplyScale(entityGfx.GetBoundingVolume().obb.localBox.GetLengths());

					renderQueue->AddMesh(0, m_material, m_meshData, Nz::Boxf::Zero(), transformMatrix, scissorRect);
				}
		};
	}

	/*!
	* \ingroup NDK
	* \class Ndk::DebugSystem
	* \brief NDK class that represents the debug system
	*
	* \remark This system is enabled if the entity owns the trait: DebugComponent and GraphicsComponent
	*/

	/*!
	* \brief Constructs an DebugSystem object by default
	*/
	DebugSystem::DebugSystem()
	{
		Requires<DebugComponent, GraphicsComponent>();
		SetUpdateOrder(1000); //< Update last
	}

	std::pair<Nz::IndexBufferRef, Nz::VertexBufferRef> DebugSystem::GetBoxMesh()
	{
		if (!m_boxMeshIndexBuffer)
		{
			std::array<Nz::UInt16, 24> indices = {
				{
					0, 1,
					1, 2,
					2, 3,
					3, 0,

					4, 5,
					5, 6,
					6, 7,
					7, 4,

					0, 4,
					1, 5,
					2, 6,
					3, 7
				}
			};

			m_boxMeshIndexBuffer = Nz::IndexBuffer::New(false, Nz::UInt32(indices.size()), Nz::DataStorage_Hardware, 0);
			m_boxMeshIndexBuffer->Fill(indices.data(), 0, Nz::UInt32(indices.size()));
		}

		if (!m_boxMeshVertexBuffer)
		{
			Nz::Boxf box(-0.5f, -0.5f, -0.5f, 1.f, 1.f, 1.f);

			std::array<Nz::Vector3f, 8> positions = {
				{
					box.GetCorner(Nz::BoxCorner_FarLeftBottom),
					box.GetCorner(Nz::BoxCorner_NearLeftBottom),
					box.GetCorner(Nz::BoxCorner_NearRightBottom),
					box.GetCorner(Nz::BoxCorner_FarRightBottom),
					box.GetCorner(Nz::BoxCorner_FarLeftTop),
					box.GetCorner(Nz::BoxCorner_NearLeftTop),
					box.GetCorner(Nz::BoxCorner_NearRightTop),
					box.GetCorner(Nz::BoxCorner_FarRightTop)
				}
			};

			m_boxMeshVertexBuffer = Nz::VertexBuffer::New(Nz::VertexDeclaration::Get(Nz::VertexLayout_XYZ), Nz::UInt32(positions.size()), Nz::DataStorage_Hardware, 0);
			m_boxMeshVertexBuffer->Fill(positions.data(), 0, Nz::UInt32(positions.size()));
		}

		return { m_boxMeshIndexBuffer, m_boxMeshVertexBuffer };
	}

	void DebugSystem::OnEntityValidation(Entity* entity, bool /*justAdded*/)
	{
		static constexpr int DebugDrawOrder = 1'000;

		DebugComponent& entityDebug = entity->GetComponent<DebugComponent>();
		GraphicsComponent& entityGfx = entity->GetComponent<GraphicsComponent>();

		DebugDrawFlags enabledFlags = entityDebug.GetEnabledFlags();
		DebugDrawFlags flags = entityDebug.GetFlags();

		DebugDrawFlags flagsToEnable = flags & ~enabledFlags;
		for (std::size_t i = 0; i <= static_cast<std::size_t>(DebugDraw::Max); ++i)
		{
			DebugDraw option = static_cast<DebugDraw>(i);
			if (flagsToEnable & option)
			{
				switch (option)
				{
					case DebugDraw::Collider3D:
					{
						const Nz::Boxf& obb = entityGfx.GetBoundingVolume().obb.localBox;

						Nz::InstancedRenderableRef renderable = GenerateCollision3DMesh(entity);
						if (renderable)
						{
							renderable->SetPersistent(false);

							entityGfx.Attach(renderable, Nz::Matrix4f::Translate(obb.GetCenter()), DebugDrawOrder);
						}

						entityDebug.UpdateDebugRenderable(option, std::move(renderable));
						break;
					}

					case DebugDraw::GraphicsAABB:
					{
						auto indexVertexBuffers = GetBoxMesh();

						Nz::InstancedRenderableRef renderable = new AABBDebugRenderable(entity, GetAABBMaterial(), indexVertexBuffers.first, indexVertexBuffers.second);
						renderable->SetPersistent(false);

						entityGfx.Attach(renderable, Nz::Matrix4f::Identity(), DebugDrawOrder);

						entityDebug.UpdateDebugRenderable(option, std::move(renderable));
						break;
					}

					case DebugDraw::GraphicsOBB:
					{
						auto indexVertexBuffers = GetBoxMesh();

						Nz::InstancedRenderableRef renderable = new OBBDebugRenderable(entity, GetOBBMaterial(), indexVertexBuffers.first, indexVertexBuffers.second);
						renderable->SetPersistent(false);

						entityGfx.Attach(renderable, Nz::Matrix4f::Identity(), DebugDrawOrder);

						entityDebug.UpdateDebugRenderable(option, std::move(renderable));
						break;
					}

					default:
						break;
				}
			}
		}

		DebugDrawFlags flagsToDisable = enabledFlags & ~flags;
		for (std::size_t i = 0; i <= static_cast<std::size_t>(DebugDraw::Max); ++i)
		{
			DebugDraw option = static_cast<DebugDraw>(i);
			if (flagsToDisable & option)
				entityGfx.Detach(entityDebug.GetDebugRenderable(option));
		}

		entityDebug.UpdateEnabledFlags(flags);
	}

	void DebugSystem::OnUpdate(float elapsedTime)
	{
		// Nothing to do
	}

	Nz::InstancedRenderableRef DebugSystem::GenerateBox(Nz::Boxf box)
	{
		Nz::MeshRef mesh = Nz::Mesh::New();
		mesh->CreateStatic();

		mesh->BuildSubMesh(Nz::Primitive::Box(box.GetLengths()));
		mesh->SetMaterialCount(1);

		Nz::ModelRef model = Nz::Model::New();
		model->SetMesh(mesh);
		model->SetMaterial(0, GetOBBMaterial());

		return model;
	}

	Nz::InstancedRenderableRef DebugSystem::GenerateCollision3DMesh(Entity* entity)
	{
		if (entity->HasComponent<CollisionComponent3D>())
		{
			CollisionComponent3D& entityCollision = entity->GetComponent<CollisionComponent3D>();
			const Nz::Collider3DRef& geom = entityCollision.GetGeom();

			std::vector<Nz::Vector3f> vertices;
			std::vector<std::size_t> indices;

			geom->ForEachPolygon([&](const float* polygonVertices, std::size_t vertexCount)
			{
				std::size_t firstIndex = vertices.size();

				for (std::size_t i = 0; i < vertexCount; ++i)
				{
					const float* vertexData = &polygonVertices[i * 3];
					vertices.emplace_back(vertexData[0], vertexData[1], vertexData[2]);
				}

				for (std::size_t i = 0; i < vertexCount - 1; ++i)
				{
					indices.push_back(firstIndex + i);
					indices.push_back(firstIndex + i + 1);
				}

				indices.push_back(firstIndex + vertexCount - 1);
				indices.push_back(firstIndex);
			});

			Nz::IndexBufferRef indexBuffer = Nz::IndexBuffer::New(vertices.size() > 0xFFFF, Nz::UInt32(indices.size()), Nz::DataStorage_Hardware, 0);
			Nz::IndexMapper indexMapper(indexBuffer, Nz::BufferAccess_WriteOnly);

			Nz::IndexIterator indexPtr = indexMapper.begin();
			for (std::size_t index : indices)
				*indexPtr++ = static_cast<Nz::UInt32>(index);

			indexMapper.Unmap();

			Nz::VertexBufferRef vertexBuffer = Nz::VertexBuffer::New(Nz::VertexDeclaration::Get(Nz::VertexLayout_XYZ), Nz::UInt32(vertices.size()), Nz::DataStorage_Hardware, 0);
			vertexBuffer->Fill(vertices.data(), 0, Nz::UInt32(vertices.size()));

			Nz::MeshRef mesh = Nz::Mesh::New();
			mesh->CreateStatic();

			Nz::StaticMeshRef subMesh = Nz::StaticMesh::New(vertexBuffer, indexBuffer);
			subMesh->SetPrimitiveMode(Nz::PrimitiveMode_LineList);
			subMesh->SetMaterialIndex(0);
			subMesh->GenerateAABB();

			mesh->SetMaterialCount(1);
			mesh->AddSubMesh(subMesh);

			Nz::ModelRef model = Nz::Model::New();
			model->SetMesh(mesh);
			model->SetMaterial(0, GetCollisionMaterial());

			return model;
		}
		else
			return nullptr;
	}

	Nz::MaterialRef DebugSystem::GetAABBMaterial()
	{
		if (!m_aabbMaterial)
		{
			m_aabbMaterial = Nz::Material::New();
			m_aabbMaterial->EnableFaceCulling(false);
			m_aabbMaterial->EnableDepthBuffer(true);
			m_aabbMaterial->SetDiffuseColor(Nz::Color::Red);
			m_aabbMaterial->SetFaceFilling(Nz::FaceFilling_Line);
		}

		return m_aabbMaterial;
	}

	Nz::MaterialRef DebugSystem::GetCollisionMaterial()
	{
		if (!m_collisionMaterial)
		{
			m_collisionMaterial = Nz::Material::New();
			m_collisionMaterial->EnableFaceCulling(false);
			m_collisionMaterial->EnableDepthBuffer(true);
			m_collisionMaterial->SetDiffuseColor(Nz::Color::Blue);
			m_collisionMaterial->SetFaceFilling(Nz::FaceFilling_Line);
		}

		return m_collisionMaterial;
	}

	Nz::MaterialRef DebugSystem::GetOBBMaterial()
	{
		if (!m_obbMaterial)
		{
			m_obbMaterial = Nz::Material::New();
			m_obbMaterial->EnableFaceCulling(false);
			m_obbMaterial->EnableDepthBuffer(true);
			m_obbMaterial->SetDiffuseColor(Nz::Color::Green);
			m_obbMaterial->SetFaceFilling(Nz::FaceFilling_Line);
		}

		return m_obbMaterial;
	}

	SystemIndex DebugSystem::systemIndex;
}
