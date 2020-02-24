// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Systems/DebugSystem.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <NazaraSDK/Components/CollisionComponent2D.hpp>
#include <NazaraSDK/Components/CollisionComponent3D.hpp>
#include <NazaraSDK/Components/DebugComponent.hpp>
#include <NazaraSDK/Components/GraphicsComponent.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Components/PhysicsComponent2D.hpp>

namespace Ndk
{
	namespace
	{
		class DebugRenderable : public Nz::InstancedRenderable
		{
			public:
				DebugRenderable(Ndk::Entity* owner, Nz::IndexBufferRef indexBuffer, Nz::VertexBufferRef vertexBuffer) :
				m_entityOwner(owner),
				m_indexBuffer(std::move(indexBuffer)),
				m_vertexBuffer(std::move(vertexBuffer))
				{
					ResetMaterials(1);

					m_meshData.indexBuffer = m_indexBuffer;
					m_meshData.primitiveMode = Nz::PrimitiveMode_LineList;
					m_meshData.vertexBuffer = m_vertexBuffer;
				}

				void UpdateBoundingVolume(InstanceData* /*instanceData*/) const override
				{
				}

				void MakeBoundingVolume() const override
				{
					// We generate an infinite bounding volume so that we're always considered for rendering when culling does occurs
					// (bounding volume culling happens only if GraphicsComponent AABB partially fail)
					m_boundingVolume.MakeInfinite();
				}

			protected:
				Ndk::EntityHandle m_entityOwner;
				Nz::IndexBufferRef m_indexBuffer;
				Nz::MeshData m_meshData;
				Nz::VertexBufferRef m_vertexBuffer;
		};

		class AABBDebugRenderable : public DebugRenderable
		{
			public:
				AABBDebugRenderable(Ndk::Entity* owner, Nz::MaterialRef globalMaterial, Nz::MaterialRef localMaterial, Nz::IndexBufferRef indexBuffer, Nz::VertexBufferRef vertexBuffer) :
				DebugRenderable(owner, std::move(indexBuffer), std::move(vertexBuffer)),
				m_globalMaterial(std::move(globalMaterial)),
				m_localMaterial(std::move(localMaterial))
				{
				}

				void AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Nz::Recti& scissorRect) const override
				{
					NazaraAssert(m_entityOwner, "DebugRenderable has no owner");

					const DebugComponent& entityDebug = m_entityOwner->GetComponent<DebugComponent>();
					const GraphicsComponent& entityGfx = m_entityOwner->GetComponent<GraphicsComponent>();

					auto DrawBox = [&](const Nz::Boxf& box, const Nz::MaterialRef& mat)
					{
						Nz::Matrix4f transformMatrix = Nz::Matrix4f::Identity();
						transformMatrix.SetScale(box.GetLengths());
						transformMatrix.SetTranslation(box.GetCenter());

						renderQueue->AddMesh(0, mat, m_meshData, Nz::Boxf::Zero(), transformMatrix, scissorRect);
					};

					DrawBox(entityGfx.GetAABB(), m_globalMaterial);

					for (std::size_t i = 0; i < entityGfx.GetAttachedRenderableCount(); ++i)
					{
						const Nz::BoundingVolumef& boundingVolume = entityGfx.GetBoundingVolume(i);
						if (boundingVolume.IsFinite())
							DrawBox(boundingVolume.aabb, m_localMaterial);
					}
				}

				std::unique_ptr<InstancedRenderable> Clone() const override
				{
					return nullptr;
				}

			private:
				Nz::MaterialRef m_globalMaterial;
				Nz::MaterialRef m_localMaterial;
		};

		class OBBDebugRenderable : public DebugRenderable
		{
			public:
				OBBDebugRenderable(Ndk::Entity* owner, Nz::MaterialRef material, Nz::IndexBufferRef indexBuffer, Nz::VertexBufferRef vertexBuffer) :
				DebugRenderable(owner, std::move(indexBuffer), std::move(vertexBuffer)),
				m_material(std::move(material))
				{
				}

				void AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Nz::Recti& scissorRect) const override
				{
					NazaraAssert(m_entityOwner, "DebugRenderable has no owner");

					const DebugComponent& entityDebug = m_entityOwner->GetComponent<DebugComponent>();
					const GraphicsComponent& entityGfx = m_entityOwner->GetComponent<GraphicsComponent>();

					auto DrawBox = [&](const Nz::Boxf& box, const Nz::Matrix4f& transformMatrix)
					{
						Nz::Matrix4f boxMatrix = Nz::Matrix4f::Identity();
						boxMatrix.SetScale(box.GetLengths());
						boxMatrix.SetTranslation(box.GetCenter());
						boxMatrix.ConcatenateAffine(transformMatrix);

						renderQueue->AddMesh(0, m_material, m_meshData, Nz::Boxf::Zero(), boxMatrix, scissorRect);
					};

					for (std::size_t i = 0; i < entityGfx.GetAttachedRenderableCount(); ++i)
					{
						const Nz::BoundingVolumef& boundingVolume = entityGfx.GetBoundingVolume(i);
						if (boundingVolume.IsFinite())
							DrawBox(boundingVolume.obb.localBox, entityGfx.GetTransformMatrix(i));
					}
				}

				std::unique_ptr<InstancedRenderable> Clone() const override
				{
					return nullptr;
				}

			private:
				Nz::MaterialRef m_material;
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
	DebugSystem::DebugSystem() :
	m_isDepthBufferEnabled(true)
	{
		Requires<DebugComponent, GraphicsComponent, NodeComponent>();
		SetUpdateOrder(1000); //< Update last
	}

	void DebugSystem::EnableDepthBuffer(bool enable)
	{
		m_isDepthBufferEnabled = enable;

		if (m_collisionMaterial)
			m_collisionMaterial->EnableDepthBuffer(enable);

		if (m_globalAabbMaterial)
			m_globalAabbMaterial->EnableDepthBuffer(enable);

		if (m_localAabbMaterial)
			m_localAabbMaterial->EnableDepthBuffer(enable);

		if (m_obbMaterial)
			m_obbMaterial->EnableDepthBuffer(enable);
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
	
	Nz::InstancedRenderableRef DebugSystem::GenerateCollision2DMesh(Entity* entity, Nz::Vector3f* offset)
	{
		if (entity->HasComponent<CollisionComponent2D>())
		{
			CollisionComponent2D& entityCollision = entity->GetComponent<CollisionComponent2D>();
			const Nz::Collider2DRef& geom = entityCollision.GetGeom();

			std::vector<Nz::Vector3f> vertices;
			std::vector<std::size_t> indices;

			geom->ForEachPolygon([&](const Nz::Vector2f* polygonVertices, std::size_t vertexCount)
			{
				std::size_t firstIndex = vertices.size();

				// Don't reserve and let the vector handle its own capacity
				for (std::size_t i = 0; i < vertexCount; ++i)
					vertices.emplace_back(*polygonVertices++);

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

			// Find center of mass
			if (entity->HasComponent<PhysicsComponent2D>())
			{
				const PhysicsComponent2D& entityPhys = entity->GetComponent<PhysicsComponent2D>();
				*offset = entityPhys.GetMassCenter(Nz::CoordSys_Local) + entityCollision.GetGeomOffset();
			}
			else
				*offset = entityCollision.GetGeomOffset();

			return model;
		}
		else
			return nullptr;
	}

	Nz::InstancedRenderableRef DebugSystem::GenerateCollision3DMesh(Entity* entity)
	{
		if (entity->HasComponent<CollisionComponent3D>())
		{
			CollisionComponent3D& entityCollision = entity->GetComponent<CollisionComponent3D>();
			const Nz::Collider3DRef& geom = entityCollision.GetGeom();

			std::vector<Nz::Vector3f> vertices;
			std::vector<std::size_t> indices;
			
			geom->ForEachPolygon([&](const Nz::Vector3f* polygonVertices, std::size_t vertexCount)
			{
				std::size_t firstIndex = vertices.size();
				vertices.resize(firstIndex + vertexCount);
				std::copy(polygonVertices, polygonVertices + vertexCount, &vertices[firstIndex]);

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

	Nz::MaterialRef DebugSystem::GetGlobalAABBMaterial()
	{
		if (!m_globalAabbMaterial)
		{
			m_globalAabbMaterial = Nz::Material::New();
			m_globalAabbMaterial->EnableFaceCulling(false);
			m_globalAabbMaterial->EnableDepthBuffer(true);
			m_globalAabbMaterial->SetDiffuseColor(Nz::Color::Orange);
			m_globalAabbMaterial->SetFaceFilling(Nz::FaceFilling_Line);
			//m_globalAabbMaterial->SetLineWidth(2.f);
		}

		return m_globalAabbMaterial;
	}

	Nz::MaterialRef DebugSystem::GetLocalAABBMaterial()
	{
		if (!m_localAabbMaterial)
		{
			m_localAabbMaterial = Nz::Material::New();
			m_localAabbMaterial->EnableFaceCulling(false);
			m_localAabbMaterial->EnableDepthBuffer(true);
			m_localAabbMaterial->SetDiffuseColor(Nz::Color::Red);
			m_localAabbMaterial->SetFaceFilling(Nz::FaceFilling_Line);
			//m_localAabbMaterial->SetLineWidth(2.f);
		}

		return m_localAabbMaterial;
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
			//m_collisionMaterial->SetLineWidth(2.f);
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
			//m_obbMaterial->SetLineWidth(2.f);
		}

		return m_obbMaterial;
	}
	
	void DebugSystem::OnEntityValidation(Entity* entity, bool /*justAdded*/)
	{
		static constexpr int DebugDrawOrder = 1'000;

		DebugComponent& entityDebug = entity->GetComponent<DebugComponent>();
		GraphicsComponent& entityGfx = entity->GetComponent<GraphicsComponent>();
		NodeComponent& entityNode = entity->GetComponent<NodeComponent>();

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
					case DebugDraw::Collider2D:
					{
						Nz::Vector3f offset;
						Nz::InstancedRenderableRef renderable = GenerateCollision2DMesh(entity, &offset);
						if (renderable)
							entityGfx.Attach(renderable, Nz::Matrix4f::Translate(offset), DebugDrawOrder);

						entityDebug.UpdateDebugRenderable(option, std::move(renderable));
						break;
					}

					case DebugDraw::Collider3D:
					{
						const Nz::Boxf& obb = entityGfx.GetAABB();

						Nz::InstancedRenderableRef renderable = GenerateCollision3DMesh(entity);
						if (renderable)
							entityGfx.Attach(renderable, Nz::Matrix4f::Translate(obb.GetCenter() - entityNode.GetPosition()), DebugDrawOrder);

						entityDebug.UpdateDebugRenderable(option, std::move(renderable));
						break;
					}

					case DebugDraw::GraphicsAABB:
					{
						auto indexVertexBuffers = GetBoxMesh();

						Nz::InstancedRenderableRef renderable = new AABBDebugRenderable(entity, GetGlobalAABBMaterial(), GetLocalAABBMaterial(), indexVertexBuffers.first, indexVertexBuffers.second);
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

	SystemIndex DebugSystem::systemIndex;
}
