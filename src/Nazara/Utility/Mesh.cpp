// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	MeshParams::MeshParams()
	{
		if (!Buffer::IsStorageSupported(storage))
			storage = DataStorage_Software;
	}

	bool MeshParams::IsValid() const
	{
		if (!Buffer::IsStorageSupported(storage))
		{
			NazaraError("Storage not supported");
			return false;
		}

		if (matrix == Matrix4f::Zero())
		{
			NazaraError("Invalid matrix");
			return false;
		}

		if (!vertexDeclaration)
		{
			NazaraError("The vertex declaration can't be null");
			return false;
		}

		if (!vertexDeclaration->HasComponent(VertexComponent_Position))
		{
			NazaraError("Vertex declaration must contains a vertex position");
			return false;
		}

		return true;
	}


	void Mesh::AddSubMesh(SubMesh* subMesh)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(subMesh, "Invalid submesh");
		NazaraAssert(subMesh->GetAnimationType() == m_animationType, "Submesh animation type doesn't match mesh animation type");

		m_subMeshes.emplace_back();
		SubMeshData& subMeshData = m_subMeshes.back();
		subMeshData.subMesh = subMesh;
		subMeshData.onSubMeshInvalidated.Connect(subMesh->OnSubMeshInvalidateAABB, [this](const SubMesh* /*subMesh*/) { InvalidateAABB(); });

		InvalidateAABB();
	}

	void Mesh::AddSubMesh(const String& identifier, SubMesh* subMesh)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(!identifier.IsEmpty(), "Identifier is empty");
		NazaraAssert(m_subMeshMap.find(identifier) == m_subMeshMap.end(), "SubMesh identifier \"" + identifier + "\" is already in use");
		NazaraAssert(subMesh, "Invalid submesh");
		NazaraAssert(subMesh->GetAnimationType() == m_animationType, "Submesh animation type doesn't match mesh animation type");

		std::size_t index = m_subMeshes.size();

		AddSubMesh(subMesh);

		m_subMeshMap[identifier] = static_cast<UInt32>(index);
	}

	SubMesh* Mesh::BuildSubMesh(const Primitive& primitive, const MeshParams& params)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType_Static, "Submesh building only works for static meshes");
		NazaraAssert(params.IsValid(), "Invalid parameters");
		NazaraAssert(params.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent_Position), "The vertex declaration doesn't have a Vector3 position component");

		Boxf aabb;
		IndexBufferRef indexBuffer;
		VertexBufferRef vertexBuffer;

		Matrix4f matrix(primitive.matrix);
		matrix *= params.matrix;

		VertexDeclaration* declaration = params.vertexDeclaration;

		switch (primitive.type)
		{
			case PrimitiveType_Box:
			{
				unsigned int indexCount;
				unsigned int vertexCount;
				ComputeBoxIndexVertexCount(primitive.box.subdivision, &indexCount, &vertexCount);

				indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, params.indexBufferFlags);
				vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, params.vertexBufferFlags);

				VertexMapper vertexMapper(vertexBuffer, BufferAccess_WriteOnly);

				VertexPointers pointers;
				pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
				pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
				pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
				pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

				IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
				GenerateBox(primitive.box.lengths, primitive.box.subdivision, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
				break;
			}

			case PrimitiveType_Cone:
			{
				unsigned int indexCount;
				unsigned int vertexCount;
				ComputeConeIndexVertexCount(primitive.cone.subdivision, &indexCount, &vertexCount);

				indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, params.indexBufferFlags);
				vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, params.vertexBufferFlags);

				VertexMapper vertexMapper(vertexBuffer, BufferAccess_WriteOnly);

				VertexPointers pointers;
				pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
				pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
				pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
				pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

				IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
				GenerateCone(primitive.cone.length, primitive.cone.radius, primitive.cone.subdivision, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
				break;
			}

			case PrimitiveType_Plane:
			{
				unsigned int indexCount;
				unsigned int vertexCount;
				ComputePlaneIndexVertexCount(primitive.plane.subdivision, &indexCount, &vertexCount);

				indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, params.indexBufferFlags);
				vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, params.vertexBufferFlags);

				VertexMapper vertexMapper(vertexBuffer, BufferAccess_WriteOnly);

				VertexPointers pointers;
				pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
				pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
				pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
				pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

				IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
				GeneratePlane(primitive.plane.subdivision, primitive.plane.size, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
				break;
			}

			case PrimitiveType_Sphere:
			{
				switch (primitive.sphere.type)
				{
					case SphereType_Cubic:
					{
						unsigned int indexCount;
						unsigned int vertexCount;
						ComputeCubicSphereIndexVertexCount(primitive.sphere.cubic.subdivision, &indexCount, &vertexCount);

						indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, params.indexBufferFlags);
						vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, params.vertexBufferFlags);

						VertexMapper vertexMapper(vertexBuffer, BufferAccess_ReadWrite);

						VertexPointers pointers;
						pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
						pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
						pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
						pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

						IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
						GenerateCubicSphere(primitive.sphere.size, primitive.sphere.cubic.subdivision, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
						break;
					}

					case SphereType_Ico:
					{
						unsigned int indexCount;
						unsigned int vertexCount;
						ComputeIcoSphereIndexVertexCount(primitive.sphere.ico.recursionLevel, &indexCount, &vertexCount);

						indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, params.indexBufferFlags);
						vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, params.vertexBufferFlags);

						VertexMapper vertexMapper(vertexBuffer, BufferAccess_WriteOnly);

						VertexPointers pointers;
						pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
						pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
						pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
						pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

						IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
						GenerateIcoSphere(primitive.sphere.size, primitive.sphere.ico.recursionLevel, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
						break;
					}

					case SphereType_UV:
					{
						unsigned int indexCount;
						unsigned int vertexCount;
						ComputeUvSphereIndexVertexCount(primitive.sphere.uv.sliceCount, primitive.sphere.uv.stackCount, &indexCount, &vertexCount);

						indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, params.indexBufferFlags);
						vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, params.vertexBufferFlags);

						VertexMapper vertexMapper(vertexBuffer, BufferAccess_WriteOnly);

						VertexPointers pointers;
						pointers.normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
						pointers.positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
						pointers.tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
						pointers.uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

						IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
						GenerateUvSphere(primitive.sphere.size, primitive.sphere.uv.sliceCount, primitive.sphere.uv.stackCount, matrix, primitive.textureCoords, pointers, indexMapper.begin(), &aabb);
						break;
					}
				}
				break;
			}
		}

		if (params.optimizeIndexBuffers)
			indexBuffer->Optimize();

		StaticMeshRef subMesh = StaticMesh::New(vertexBuffer, indexBuffer);
		subMesh->SetAABB(aabb);

		AddSubMesh(subMesh);
		return subMesh;
	}

	void Mesh::BuildSubMeshes(const PrimitiveList& list, const MeshParams& params)
	{
		for (UInt32 i = 0; i < list.GetSize(); ++i)
			BuildSubMesh(list.GetPrimitive(i), params);
	}

	bool Mesh::CreateSkeletal(UInt32 jointCount)
	{
		Destroy();

		m_animationType = AnimationType_Skeletal;
		m_jointCount = jointCount;
		if (!m_skeleton.Create(jointCount))
		{
			NazaraError("Failed to create skeleton");
			return false;
		}

		m_isValid = true;

		return true;
	}

	bool Mesh::CreateStatic()
	{
		Destroy();

		m_animationType = AnimationType_Static;
		m_isValid = true;

		return true;
	}

	void Mesh::Destroy()
	{
		if (m_isValid)
		{
			OnMeshDestroy(this);

			m_animationPath.Clear();
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
				m_aabb.Set(m_subMeshes.front().subMesh->GetAABB());
				for (std::size_t i = 1; i < subMeshCount; ++i)
					m_aabb.ExtendTo(m_subMeshes[i].subMesh->GetAABB());
			}
			else
				m_aabb.MakeZero();

			m_aabbUpdated = true;
		}

		return m_aabb;
	}

	String Mesh::GetAnimation() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_animationPath;
	}

	AnimationType Mesh::GetAnimationType() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_animationType;
	}

	UInt32 Mesh::GetJointCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType_Skeletal, "Mesh is not skeletal");

		return m_jointCount;
	}

	ParameterList& Mesh::GetMaterialData(UInt32 index)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_materialData.size(), "Material index out of range");

		return m_materialData[index];
	}

	const ParameterList& Mesh::GetMaterialData(UInt32 index) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_materialData.size(), "Material index out of range");

		return m_materialData[index];
	}

	UInt32 Mesh::GetMaterialCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return static_cast<UInt32>(m_materialData.size());
	}

	Skeleton* Mesh::GetSkeleton()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType_Skeletal, "Mesh is not skeletal");

		return &m_skeleton;
	}

	const Skeleton* Mesh::GetSkeleton() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType_Skeletal, "Mesh is not skeletal");

		return &m_skeleton;
	}

	SubMesh* Mesh::GetSubMesh(const String& identifier)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		auto it = m_subMeshMap.find(identifier);
		NazaraAssert(it != m_subMeshMap.end(), "SubMesh " + identifier + " not found");

		return m_subMeshes[it->second].subMesh;
	}

	SubMesh* Mesh::GetSubMesh(UInt32 index)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_subMeshes.size(), "Submesh index out of range");

		return m_subMeshes[index].subMesh;
	}

	const SubMesh* Mesh::GetSubMesh(const String& identifier) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		auto it = m_subMeshMap.find(identifier);
		NazaraAssert(it != m_subMeshMap.end(), "SubMesh " + identifier + " not found");

		return m_subMeshes[it->second].subMesh;
	}

	const SubMesh* Mesh::GetSubMesh(UInt32 index) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_subMeshes.size(), "Submesh index out of range");

		return m_subMeshes[index].subMesh;
	}

	UInt32 Mesh::GetSubMeshCount() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return static_cast<UInt32>(m_subMeshes.size());
	}

	UInt32 Mesh::GetSubMeshIndex(const String& identifier) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		auto it = m_subMeshMap.find(identifier);
		NazaraAssert(it != m_subMeshMap.end(), "SubMesh " + identifier + " not found");

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

	bool Mesh::HasSubMesh(const String& identifier) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_subMeshMap.find(identifier) != m_subMeshMap.end();
	}

	bool Mesh::HasSubMesh(UInt32 index) const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return index < m_subMeshes.size();
	}

	bool Mesh::IsAnimable() const
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		return m_animationType != AnimationType_Static;
	}

	bool Mesh::IsValid() const
	{
		return m_isValid;
	}

	bool Mesh::LoadFromFile(const String& filePath, const MeshParams& params)
	{
		return MeshLoader::LoadFromFile(this, filePath, params);
	}

	bool Mesh::LoadFromMemory(const void* data, std::size_t size, const MeshParams& params)
	{
		return MeshLoader::LoadFromMemory(this, data, size, params);
	}

	bool Mesh::LoadFromStream(Stream& stream, const MeshParams& params)
	{
		return MeshLoader::LoadFromStream(this, stream, params);
	}

	void Mesh::Recenter()
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType_Static, "Mesh is not static");

		// The center of our mesh is the center of our *global* AABB
		Vector3f center = GetAABB().GetCenter();

		for (SubMeshData& data : m_subMeshes)
		{
			StaticMesh& staticMesh = static_cast<StaticMesh&>(*data.subMesh);

			BufferMapper<VertexBuffer> mapper(staticMesh.GetVertexBuffer(), BufferAccess_ReadWrite);
			MeshVertex* vertices = static_cast<MeshVertex*>(mapper.GetPointer());

			UInt32 vertexCount = staticMesh.GetVertexCount();
			for (UInt32 i = 0; i < vertexCount; ++i)
			{
				vertices->position -= center;
				vertices++;
			}

			// Our AABB doesn't change shape, only position
			Boxf aabb = staticMesh.GetAABB();
			aabb.Translate(-center);

			staticMesh.SetAABB(aabb); // This will invalidate our AABB
		}
	}

	void Mesh::RemoveSubMesh(const String& identifier)
	{
		UInt32 index = GetSubMeshIndex(identifier);

		// On déplace l'itérateur du début d'une distance de x
		auto it2 = m_subMeshes.begin();
		std::advance(it2, index);
		m_subMeshes.erase(it2);

		InvalidateAABB();
	}

	void Mesh::RemoveSubMesh(UInt32 index)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(index < m_subMeshes.size(), "Submesh index out of range");

		// On déplace l'itérateur du début de x
		auto it = m_subMeshes.begin();
		std::advance(it, index);
		m_subMeshes.erase(it);

		InvalidateAABB();
	}

	bool Mesh::SaveToFile(const String& filePath, const MeshParams& params)
	{
		return MeshSaver::SaveToFile(*this, filePath, params);
	}

	bool Mesh::SaveToStream(Stream& stream, const String& format, const MeshParams& params)
	{
		return MeshSaver::SaveToStream(*this, stream, format, params);
	}

	void Mesh::SetAnimation(const String& animationPath)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");

		m_animationPath = animationPath;
	}

	void Mesh::SetMaterialData(UInt32 matIndex, ParameterList data)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(matIndex < m_materialData.size(), "Material index out of range");

		m_materialData[matIndex] = std::move(data);
	}

	void Mesh::SetMaterialCount(UInt32 matCount)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(matCount > 0, "A mesh should have at least a material");

		m_materialData.resize(matCount);

		#ifdef NAZARA_DEBUG
		for (SubMeshData& data : m_subMeshes)
		{
			UInt32 matIndex = data.subMesh->GetMaterialIndex();
			if (matIndex >= matCount)
			{
				data.subMesh->SetMaterialIndex(0); // To prevent a crash
				NazaraWarning("SubMesh " + String::Pointer(data.subMesh) + " material index is over mesh new material count (" + String::Number(matIndex) + " >= " + String::Number(matCount) + "), setting it to first material");
			}
		}
		#endif
	}

	void Mesh::Transform(const Matrix4f& matrix)
	{
		NazaraAssert(m_isValid, "Mesh should be created first");
		NazaraAssert(m_animationType == AnimationType_Static, "Mesh is not static");

		for (SubMeshData& data : m_subMeshes)
		{
			StaticMesh& staticMesh = static_cast<StaticMesh&>(*data.subMesh);

			BufferMapper<VertexBuffer> mapper(staticMesh.GetVertexBuffer(), BufferAccess_ReadWrite);
			MeshVertex* vertices = static_cast<MeshVertex*>(mapper.GetPointer());

			Boxf aabb(vertices->position.x, vertices->position.y, vertices->position.z, 0.f, 0.f, 0.f);

			UInt32 vertexCount = staticMesh.GetVertexCount();
			for (UInt32 i = 0; i < vertexCount; ++i)
			{
				vertices->position = matrix.Transform(vertices->position);
				aabb.ExtendTo(vertices->position);

				vertices++;
			}

			staticMesh.SetAABB(aabb); //< This will invalidate our AABB
		}
	}

	bool Mesh::Initialize()
	{
		if (!MeshLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!MeshManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		return true;
	}

	void Mesh::Uninitialize()
	{
		MeshManager::Uninitialize();
		MeshLibrary::Uninitialize();
	}

	MeshLibrary::LibraryMap Mesh::s_library;
	MeshLoader::LoaderList Mesh::s_loaders;
	MeshManager::ManagerMap Mesh::s_managerMap;
	MeshManager::ManagerParams Mesh::s_managerParameters;
	MeshSaver::SaverList Mesh::s_savers;
}
