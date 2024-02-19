// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Buffer.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/IndexMapper.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/SubMesh.hpp>
#include <Nazara/Core/VertexMapper.hpp>
#include <limits>
#include <memory>
#include <unordered_map>

namespace Nz
{
	bool MeshParams::IsValid() const
	{
		if (!vertexDeclaration)
		{
			NazaraError("the vertex declaration can't be null");
			return false;
		}

		if (!vertexDeclaration->HasComponent(VertexComponent::Position))
		{
			NazaraError("vertex declaration must contains a vertex position");
			return false;
		}

		return true;
	}


	void Mesh::AddSubMesh(std::shared_ptr<SubMesh> subMesh)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(subMesh, "Invalid submesh");
		NazaraAssert(subMesh->GetAnimationType() == m_animationType, "Submesh animation type doesn't match mesh animation type");

		m_subMeshes.emplace_back();
		SubMeshData& subMeshData = m_subMeshes.back();
		subMeshData.subMesh = std::move(subMesh);
		subMeshData.onSubMeshInvalidated.Connect(subMeshData.subMesh->OnSubMeshInvalidateAABB, [this](const SubMesh* /*subMesh*/) { InvalidateAABB(); });

		InvalidateAABB();
	}

	void Mesh::AddSubMesh(std::string identifier, std::shared_ptr<SubMesh> subMesh)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(!identifier.empty(), "empty identifier");
		NazaraAssertFmt(!m_subMeshMap.contains(identifier), "SubMesh identifier \"{0}\" is already in use", identifier);
		NazaraAssert(subMesh, "invalid submesh");
		NazaraAssert(subMesh->GetAnimationType() == m_animationType, "Submesh animation type doesn't match mesh animation type");

		std::size_t index = m_subMeshes.size();
		AddSubMesh(std::move(subMesh));

		m_subMeshMap.emplace(std::move(identifier), index);
	}

	std::shared_ptr<SubMesh> Mesh::BuildSubMesh(const Primitive& primitive, const MeshParams& params)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType::Static, "Submesh building only works for static meshes");
		NazaraAssert(params.IsValid(), "Invalid parameters");
		NazaraAssert(params.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent::Position), "The vertex declaration doesn't have a Vector3 position component");

		Boxf aabb;
		std::shared_ptr<IndexBuffer> indexBuffer;
		std::shared_ptr<VertexBuffer> vertexBuffer;

		Matrix4f matrix = Matrix4f::ConcatenateTransform(primitive.matrix, Matrix4f::Transform(params.vertexOffset, params.vertexRotation, params.vertexScale));

		const std::shared_ptr<VertexDeclaration>& declaration = params.vertexDeclaration;

		switch (primitive.type)
		{
			case PrimitiveType::Box:
			{
				UInt32 indexCount;
				UInt32 vertexCount;
				ComputeBoxIndexVertexCount(primitive.box.subdivision, &indexCount, &vertexCount);

				bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

				indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, params.indexBufferFlags, params.bufferFactory);
				vertexBuffer = std::make_shared<VertexBuffer>(declaration, vertexCount, params.vertexBufferFlags, params.bufferFactory);

				VertexMapper vertexMapper(*vertexBuffer);

				VertexPointers pointers;
				pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
				pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
				pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent);
				pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

				IndexMapper indexMapper(*indexBuffer);
				GenerateBox(primitive.box.lengths, primitive.box.subdivision, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
				break;
			}

			case PrimitiveType::Cone:
			{
				UInt32 indexCount;
				UInt32 vertexCount;
				ComputeConeIndexVertexCount(primitive.cone.subdivision, &indexCount, &vertexCount);

				bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

				indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, params.indexBufferFlags, params.bufferFactory);
				vertexBuffer = std::make_shared<VertexBuffer>(declaration, vertexCount, params.vertexBufferFlags, params.bufferFactory);

				VertexMapper vertexMapper(*vertexBuffer);

				VertexPointers pointers;
				pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
				pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
				pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent);
				pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

				IndexMapper indexMapper(*indexBuffer);
				GenerateCone(primitive.cone.length, primitive.cone.radius, primitive.cone.subdivision, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
				break;
			}

			case PrimitiveType::Plane:
			{
				UInt32 indexCount;
				UInt32 vertexCount;
				ComputePlaneIndexVertexCount(primitive.plane.subdivision, &indexCount, &vertexCount);

				bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

				indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, params.indexBufferFlags, params.bufferFactory);
				vertexBuffer = std::make_shared<VertexBuffer>(declaration, vertexCount, params.vertexBufferFlags, params.bufferFactory);

				VertexMapper vertexMapper(*vertexBuffer);

				VertexPointers pointers;
				pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
				pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
				pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent);
				pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

				IndexMapper indexMapper(*indexBuffer);
				GeneratePlane(primitive.plane.subdivision, primitive.plane.size, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
				break;
			}

			case PrimitiveType::Sphere:
			{
				switch (primitive.sphere.type)
				{
					case SphereType::Cubic:
					{
						UInt32 indexCount;
						UInt32 vertexCount;
						ComputeCubicSphereIndexVertexCount(primitive.sphere.cubic.subdivision, &indexCount, &vertexCount);

						bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

						indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, params.indexBufferFlags, params.bufferFactory);
						vertexBuffer = std::make_shared<VertexBuffer>(declaration, vertexCount, params.vertexBufferFlags, params.bufferFactory);

						VertexMapper vertexMapper(*vertexBuffer);

						VertexPointers pointers;
						pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
						pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
						pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent);
						pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

						IndexMapper indexMapper(*indexBuffer);
						GenerateCubicSphere(primitive.sphere.size, primitive.sphere.cubic.subdivision, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
						break;
					}

					case SphereType::Ico:
					{
						UInt32 indexCount;
						UInt32 vertexCount;
						ComputeIcoSphereIndexVertexCount(primitive.sphere.ico.recursionLevel, &indexCount, &vertexCount);

						bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

						indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, params.indexBufferFlags, params.bufferFactory);
						vertexBuffer = std::make_shared<VertexBuffer>(declaration, vertexCount, params.vertexBufferFlags, params.bufferFactory);

						VertexMapper vertexMapper(*vertexBuffer);

						VertexPointers pointers;
						pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
						pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
						pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent);
						pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

						IndexMapper indexMapper(*indexBuffer);
						GenerateIcoSphere(primitive.sphere.size, primitive.sphere.ico.recursionLevel, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
						break;
					}

					case SphereType::UV:
					{
						UInt32 indexCount;
						UInt32 vertexCount;
						ComputeUvSphereIndexVertexCount(primitive.sphere.uv.sliceCount, primitive.sphere.uv.stackCount, &indexCount, &vertexCount);

						bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

						indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, params.indexBufferFlags, params.bufferFactory);
						vertexBuffer = std::make_shared<VertexBuffer>(declaration, vertexCount, params.vertexBufferFlags, params.bufferFactory);

						VertexMapper vertexMapper(*vertexBuffer);

						VertexPointers pointers;
						pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
						pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
						pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent);
						pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

						IndexMapper indexMapper(*indexBuffer);
						GenerateUvSphere(primitive.sphere.size, primitive.sphere.uv.sliceCount, primitive.sphere.uv.stackCount, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
						break;
					}
				}
				break;
			}
		}

		if (params.optimizeIndexBuffers)
			indexBuffer->Optimize();

		std::shared_ptr<StaticMesh> subMesh = std::make_shared<StaticMesh>(vertexBuffer, indexBuffer);
		subMesh->SetAABB(aabb);

		AddSubMesh(subMesh);
		return subMesh;
	}

	void Mesh::BuildSubMeshes(const PrimitiveList& primitiveList, const MeshParams& params)
	{
		for (std::size_t i = 0; i < primitiveList.GetSize(); ++i)
			BuildSubMesh(primitiveList.GetPrimitive(i), params);
	}

	bool Mesh::CreateSkeletal(std::size_t jointCount)
	{
		Destroy();

		m_animationType = AnimationType::Skeletal;
		m_jointCount = jointCount;
		if (!m_skeleton.Create(jointCount))
		{
			NazaraError("failed to create skeleton");
			return false;
		}

		m_isValid = true;

		return true;
	}

	bool Mesh::CreateStatic()
	{
		Destroy();

		m_animationType = AnimationType::Static;
		m_isValid = true;

		return true;
	}

	void Mesh::Destroy()
	{
		if (m_isValid)
		{
			m_animationPath.clear();
			m_materialData.clear();
			m_materialData.resize(1);
			m_skeleton.Destroy();
			m_subMeshes.clear();
			m_subMeshMap.clear();

			m_isValid = false;
		}
	}

	void Mesh::GenerateNormals()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		for (SubMeshData& data : m_subMeshes)
			data.subMesh->GenerateNormals();
	}

	void Mesh::GenerateNormalsAndTangents()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		for (SubMeshData& data : m_subMeshes)
			data.subMesh->GenerateNormalsAndTangents();
	}

	void Mesh::GenerateTangents()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		for (SubMeshData& data : m_subMeshes)
			data.subMesh->GenerateTangents();
	}

	const Boxf& Mesh::GetAABB() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		if (!m_aabbUpdated)
		{
			std::size_t subMeshCount = m_subMeshes.size();
			if (subMeshCount > 0)
			{
				m_aabb = Boxf(m_subMeshes.front().subMesh->GetAABB());
				for (std::size_t i = 1; i < subMeshCount; ++i)
					m_aabb.ExtendTo(m_subMeshes[i].subMesh->GetAABB());
			}
			else
				m_aabb = Boxf::Zero();

			m_aabbUpdated = true;
		}

		return m_aabb;
	}

	std::filesystem::path Mesh::GetAnimation() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_animationPath;
	}

	AnimationType Mesh::GetAnimationType() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_animationType;
	}

	std::size_t Mesh::GetJointCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType::Skeletal, "Mesh is not skeletal");

		return m_jointCount;
	}

	ParameterList& Mesh::GetMaterialData(std::size_t index)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_materialData.size(), "Material index out of range");

		return m_materialData[index];
	}

	const ParameterList& Mesh::GetMaterialData(std::size_t index) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_materialData.size(), "Material index out of range");

		return m_materialData[index];
	}

	std::size_t Mesh::GetMaterialCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return static_cast<std::size_t>(m_materialData.size());
	}

	Skeleton* Mesh::GetSkeleton()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType::Skeletal, "Mesh is not skeletal");

		return &m_skeleton;
	}

	const Skeleton* Mesh::GetSkeleton() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType::Skeletal, "Mesh is not skeletal");

		return &m_skeleton;
	}

	const std::shared_ptr<SubMesh>& Mesh::GetSubMesh(std::string_view identifier) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		auto it = m_subMeshMap.find(identifier);
		NazaraAssertFmt(it != m_subMeshMap.end(), "SubMesh {0} not found", identifier);

		return m_subMeshes[it->second].subMesh;
	}

	const std::shared_ptr<SubMesh>& Mesh::GetSubMesh(std::size_t index) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_subMeshes.size(), "Submesh index out of range");

		return m_subMeshes[index].subMesh;
	}

	std::size_t Mesh::GetSubMeshCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return static_cast<std::size_t>(m_subMeshes.size());
	}

	std::size_t Mesh::GetSubMeshIndex(std::string_view identifier) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		auto it = m_subMeshMap.find(identifier);
		NazaraAssertFmt(it != m_subMeshMap.end(), "SubMesh {0} not found", identifier);

		return it->second;
	}

	UInt32 Mesh::GetTriangleCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		UInt32 triangleCount = 0;
		for (const SubMeshData& data : m_subMeshes)
			triangleCount += data.subMesh->GetTriangleCount();

		return triangleCount;
	}

	UInt32 Mesh::GetVertexCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		UInt32 vertexCount = 0;
		for (const SubMeshData& data : m_subMeshes)
			vertexCount += data.subMesh->GetVertexCount();

		return vertexCount;
	}

	void Mesh::InvalidateAABB() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		m_aabbUpdated = false;

		OnMeshInvalidateAABB(this);
	}

	bool Mesh::HasSubMesh(std::string_view identifier) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		return m_subMeshMap.contains(identifier);
	}

	bool Mesh::HasSubMesh(std::size_t index) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		return index < m_subMeshes.size();
	}

	bool Mesh::IsAnimable() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_animationType != AnimationType::Static;
	}

	bool Mesh::IsValid() const
	{
		return m_isValid;
	}

	void Mesh::Recenter()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType::Static, "Mesh is not static");

		// The center of our mesh is the center of our *global* AABB
		Vector3f center = GetAABB().GetCenter();

		for (SubMeshData& data : m_subMeshes)
		{
			StaticMesh& staticMesh = static_cast<StaticMesh&>(*data.subMesh);

			VertexMapper mapper(*staticMesh.GetVertexBuffer());
			SparsePtr<Vector3f> position = mapper.GetComponentPtr<Vector3f>(VertexComponent::Position);

			std::size_t vertexCount = staticMesh.GetVertexCount();
			for (std::size_t i = 0; i < vertexCount; ++i)
				*position++ -= center;

			// Our AABB doesn't change shape, only position
			Boxf aabb = staticMesh.GetAABB();
			aabb.Translate(-center);

			staticMesh.SetAABB(aabb); // This will invalidate our AABB
		}
	}

	void Mesh::RemoveSubMesh(std::string_view identifier)
	{
		std::size_t index = GetSubMeshIndex(identifier);
		RemoveSubMesh(index);
	}

	void Mesh::RemoveSubMesh(std::size_t index)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_subMeshes.size(), "Submesh index out of range");

		m_subMeshes.erase(m_subMeshes.begin() + index);

		// Shift indices
		for (auto& it : m_subMeshMap)
		{
			if (it.second > index)
				it.second--;
		}

		InvalidateAABB();
	}

	bool Mesh::SaveToFile(const std::filesystem::path& filePath, const MeshParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetMeshSaver().SaveToFile(*this, filePath, params);
	}

	bool Mesh::SaveToStream(Stream& stream, std::string_view format, const MeshParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetMeshSaver().SaveToStream(*this, stream, format, params);
	}

	void Mesh::SetAnimation(const std::filesystem::path& animationPath)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		m_animationPath = animationPath;
	}

	void Mesh::SetMaterialData(std::size_t matIndex, ParameterList data)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(matIndex < m_materialData.size(), "Material index out of range");

		m_materialData[matIndex] = std::move(data);
	}

	void Mesh::SetMaterialCount(std::size_t matCount)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(matCount > 0, "A mesh should have at least a material");

		m_materialData.resize(matCount);

#ifdef NAZARA_DEBUG
		for (SubMeshData& data : m_subMeshes)
		{
			std::size_t matIndex = data.subMesh->GetMaterialIndex();
			if (matIndex >= matCount)
			{
				data.subMesh->SetMaterialIndex(0); // To prevent a crash
				NazaraWarningFmt("SubMesh {0} material index is over mesh new material count ({1} >= {2}), setting it to first material", fmt::ptr(data.subMesh.get()), matIndex, matCount);
			}
		}
#endif
	}

	void Mesh::Transform(const Matrix4f& matrix)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType::Static, "Mesh is not static");

		for (SubMeshData& data : m_subMeshes)
		{
			StaticMesh& staticMesh = static_cast<StaticMesh&>(*data.subMesh);

			BufferMapper<VertexBuffer> mapper(*staticMesh.GetVertexBuffer(), 0, staticMesh.GetVertexCount());
			MeshVertex* vertices = static_cast<MeshVertex*>(mapper.GetPointer());

			Boxf aabb(vertices->position.x, vertices->position.y, vertices->position.z, 0.f, 0.f, 0.f);

			std::size_t vertexCount = staticMesh.GetVertexCount();
			for (std::size_t i = 0; i < vertexCount; ++i)
			{
				vertices->position = matrix.Transform(vertices->position);
				aabb.ExtendTo(vertices->position);

				vertices++;
			}

			staticMesh.SetAABB(aabb); //< This will invalidate our AABB
		}
	}

	std::shared_ptr<Mesh> Mesh::LoadFromFile(const std::filesystem::path& filePath, const MeshParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetMeshLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Mesh> Mesh::LoadFromMemory(const void* data, std::size_t size, const MeshParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetMeshLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Mesh> Mesh::LoadFromStream(Stream& stream, const MeshParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetMeshLoader().LoadFromStream(stream, params);
	}
}
