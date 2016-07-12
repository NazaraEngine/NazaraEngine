// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <cstring>
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

		return true;
	}

	struct MeshImpl
	{
		MeshImpl()
		{
			materialData.resize(1); // Un matériau par défaut
		}

		std::unordered_map<String, unsigned int> subMeshMap;
		std::vector<ParameterList> materialData;
		std::vector<SubMeshRef> subMeshes;
		AnimationType animationType;
		Boxf aabb;
		Skeleton skeleton; // Uniquement pour les meshs squelettiques
		String animationPath;
		bool aabbUpdated = false;
		unsigned int jointCount; // Uniquement pour les meshs squelettiques
	};

	Mesh::~Mesh()
	{
		OnMeshRelease(this);

		Destroy();
	}

	void Mesh::AddSubMesh(SubMesh* subMesh)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(subMesh, "Invalid submesh");
		NazaraAssert(subMesh->GetAnimationType() == m_impl->animationType, "Submesh animation type doesn't match mesh animation type");

		m_impl->subMeshes.push_back(subMesh);

		InvalidateAABB();
	}

	void Mesh::AddSubMesh(const String& identifier, SubMesh* subMesh)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(!identifier.IsEmpty(), "Identifier is empty");
		NazaraAssert(m_impl->subMeshMap.find(identifier) == m_impl->subMeshMap.end(), "SubMesh identifier \"" + identifier + "\" is already in use");
		NazaraAssert(subMesh, "Invalid submesh");
		NazaraAssert(subMesh->GetAnimationType() == m_impl->animationType, "Submesh animation type doesn't match mesh animation type");

		int index = m_impl->subMeshes.size();

		m_impl->subMeshes.push_back(subMesh);
		m_impl->subMeshMap[identifier] = index;

		InvalidateAABB();
	}

	SubMesh* Mesh::BuildSubMesh(const Primitive& primitive, const MeshParams& params)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(m_impl->animationType == AnimationType_Static, "Submesh building only works for static meshes");
		NazaraAssert(params.IsValid(), "Invalid parameters");

		Boxf aabb;
		IndexBufferRef indexBuffer;
		VertexBufferRef vertexBuffer;

		Matrix4f matrix(primitive.matrix);
		matrix *= params.matrix;

		VertexDeclaration* declaration = VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent);

		switch (primitive.type)
		{
			case PrimitiveType_Box:
			{
				unsigned int indexCount;
				unsigned int vertexCount;
				ComputeBoxIndexVertexCount(primitive.box.subdivision, &indexCount, &vertexCount);

				indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, BufferUsage_Static);
				vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, BufferUsage_Static);

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

				indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, BufferUsage_Static);
				vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, BufferUsage_Static);

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

				indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, BufferUsage_Static);
				vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, BufferUsage_Static);

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

						indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, BufferUsage_Static);
						vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, BufferUsage_Static);

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

						indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, BufferUsage_Static);
						vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, BufferUsage_Static);

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

						indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indexCount, params.storage, BufferUsage_Static);
						vertexBuffer = VertexBuffer::New(declaration, vertexCount, params.storage, BufferUsage_Static);

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

		StaticMeshRef subMesh = StaticMesh::New(this);
		if (!subMesh->Create(vertexBuffer))
		{
			NazaraError("Failed to create StaticMesh");
			return nullptr;
		}

		if (params.optimizeIndexBuffers)
			indexBuffer->Optimize();

		subMesh->SetAABB(aabb);
		subMesh->SetIndexBuffer(indexBuffer);

		AddSubMesh(subMesh);
		return subMesh;
	}

	void Mesh::BuildSubMeshes(const PrimitiveList& list, const MeshParams& params)
	{
		for (unsigned int i = 0; i < list.GetSize(); ++i)
			BuildSubMesh(list.GetPrimitive(i), params);
	}

	bool Mesh::CreateSkeletal(unsigned int jointCount)
	{
		Destroy();

		std::unique_ptr<MeshImpl> impl(new MeshImpl);
		impl->animationType = AnimationType_Skeletal;
		impl->jointCount = jointCount;
		if (!impl->skeleton.Create(jointCount))
		{
			NazaraError("Failed to create skeleton");
			return false;
		}

		m_impl = impl.release();

		return true;
	}

	bool Mesh::CreateStatic()
	{
		Destroy();

		m_impl = new MeshImpl;
		m_impl->animationType = AnimationType_Static;

		return true;
	}

	void Mesh::Destroy()
	{
		if (m_impl)
		{
			OnMeshDestroy(this);

			delete m_impl;
			m_impl = nullptr;
		}
	}

	void Mesh::GenerateNormals()
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		for (SubMesh* subMesh : m_impl->subMeshes)
			subMesh->GenerateNormals();
	}

	void Mesh::GenerateNormalsAndTangents()
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		for (SubMesh* subMesh : m_impl->subMeshes)
			subMesh->GenerateNormalsAndTangents();
	}

	void Mesh::GenerateTangents()
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		for (SubMesh* subMesh : m_impl->subMeshes)
			subMesh->GenerateTangents();
	}

	const Boxf& Mesh::GetAABB() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		if (!m_impl->aabbUpdated)
		{
			unsigned int subMeshCount = m_impl->subMeshes.size();
			if (subMeshCount > 0)
			{
				m_impl->aabb.Set(m_impl->subMeshes[0]->GetAABB());
				for (unsigned int i = 1; i < subMeshCount; ++i)
					m_impl->aabb.ExtendTo(m_impl->subMeshes[i]->GetAABB());
			}
			else
				m_impl->aabb.MakeZero();

			m_impl->aabbUpdated = true;
		}

		return m_impl->aabb;
	}

	String Mesh::GetAnimation() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return m_impl->animationPath;
	}

	AnimationType Mesh::GetAnimationType() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return m_impl->animationType;
	}

	unsigned int Mesh::GetJointCount() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(m_impl->animationType == AnimationType_Skeletal, "Mesh is not skeletal");

		return m_impl->jointCount;
	}

	ParameterList& Mesh::GetMaterialData(unsigned int index)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(index < m_impl->materialData.size(), "Material index out of range");

		return m_impl->materialData[index];
	}

	const ParameterList& Mesh::GetMaterialData(unsigned int index) const
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(index < m_impl->materialData.size(), "Material index out of range");

		return m_impl->materialData[index];
	}

	unsigned int Mesh::GetMaterialCount() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return m_impl->materialData.size();
	}

	Skeleton* Mesh::GetSkeleton()
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(m_impl->animationType == AnimationType_Skeletal, "Mesh is not skeletal");

		return &m_impl->skeleton;
	}

	const Skeleton* Mesh::GetSkeleton() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(m_impl->animationType == AnimationType_Skeletal, "Mesh is not skeletal");

		return &m_impl->skeleton;
	}

	SubMesh* Mesh::GetSubMesh(const String& identifier)
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		auto it = m_impl->subMeshMap.find(identifier);
		NazaraAssert(it != m_impl->subMeshMap.end(), "SubMesh " + identifier + " not found");

		return m_impl->subMeshes[it->second];
	}

	SubMesh* Mesh::GetSubMesh(unsigned int index)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(index < m_impl->subMeshes.size(), "Submesh index out of range");

		return m_impl->subMeshes[index];
	}

	const SubMesh* Mesh::GetSubMesh(const String& identifier) const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		auto it = m_impl->subMeshMap.find(identifier);
		NazaraAssert(it != m_impl->subMeshMap.end(), "SubMesh " + identifier + " not found");

		return m_impl->subMeshes[it->second];
	}

	const SubMesh* Mesh::GetSubMesh(unsigned int index) const
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(index < m_impl->subMeshes.size(), "Submesh index out of range");

		return m_impl->subMeshes[index];
	}

	unsigned int Mesh::GetSubMeshCount() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return m_impl->subMeshes.size();
	}

	int Mesh::GetSubMeshIndex(const String& identifier) const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		auto it = m_impl->subMeshMap.find(identifier);
		NazaraAssert(it != m_impl->subMeshMap.end(), "SubMesh " + identifier + " not found");

		return it->second;
	}

	unsigned int Mesh::GetTriangleCount() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		unsigned int triangleCount = 0;
		for (SubMesh* subMesh : m_impl->subMeshes)
			triangleCount += subMesh->GetTriangleCount();

		return triangleCount;
	}

	unsigned int Mesh::GetVertexCount() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		unsigned int vertexCount = 0;
		for (SubMesh* subMesh : m_impl->subMeshes)
			vertexCount += subMesh->GetVertexCount();

		return vertexCount;
	}

	void Mesh::InvalidateAABB() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		m_impl->aabbUpdated = false;
	}

	bool Mesh::HasSubMesh(const String& identifier) const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return m_impl->subMeshMap.find(identifier) != m_impl->subMeshMap.end();
	}

	bool Mesh::HasSubMesh(unsigned int index) const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return index < m_impl->subMeshes.size();
	}

	bool Mesh::IsAnimable() const
	{
		NazaraAssert(m_impl, "Mesh should be created first");

		return m_impl->animationType != AnimationType_Static;
	}

	bool Mesh::IsValid() const
	{
		return m_impl != nullptr;
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
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(m_impl->animationType == AnimationType_Static, "Mesh is not static");

		// The center of our mesh is the center of our *global* AABB
		Vector3f center = GetAABB().GetCenter();

		for (SubMesh* subMesh : m_impl->subMeshes)
		{
			StaticMesh* staticMesh = static_cast<StaticMesh*>(subMesh);

			BufferMapper<VertexBuffer> mapper(staticMesh->GetVertexBuffer(), BufferAccess_ReadWrite);
			MeshVertex* vertices = static_cast<MeshVertex*>(mapper.GetPointer());

			unsigned int vertexCount = staticMesh->GetVertexCount();
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				vertices->position -= center;
				vertices++;
			}

			// Our AABB doesn't change shape, only position
			Boxf aabb = staticMesh->GetAABB();
			aabb.Translate(-center);

			staticMesh->SetAABB(aabb);
		}

		InvalidateAABB();
	}

	void Mesh::RemoveSubMesh(const String& identifier)
	{
		unsigned int index = GetSubMeshIndex(identifier);

		// On déplace l'itérateur du début d'une distance de x
		auto it2 = m_impl->subMeshes.begin();
		std::advance(it2, index);
		m_impl->subMeshes.erase(it2);

		InvalidateAABB();
	}

	void Mesh::RemoveSubMesh(unsigned int index)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(index < m_impl->subMeshes.size(), "Submesh index out of range");

		// On déplace l'itérateur du début de x
		auto it = m_impl->subMeshes.begin();
		std::advance(it, index);
		m_impl->subMeshes.erase(it);

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
		NazaraAssert(m_impl, "Mesh should be created first");

		m_impl->animationPath = animationPath;
	}

	void Mesh::SetMaterialData(unsigned int matIndex, ParameterList data)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(matIndex < m_impl->materialData.size(), "Material index out of range");

		m_impl->materialData[matIndex] = std::move(data);
	}

	void Mesh::SetMaterialCount(unsigned int matCount)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(matCount > 0, "A mesh should have at least a material");

		m_impl->materialData.resize(matCount);

		#ifdef NAZARA_DEBUG
		for (SubMesh* subMesh : m_impl->subMeshes)
		{
			unsigned int matIndex = subMesh->GetMaterialIndex();
			if (matIndex >= matCount)
			{
				subMesh->SetMaterialIndex(0); // To prevent a crash
				NazaraWarning("SubMesh " + String::Pointer(subMesh) + " material index is over mesh new material count (" + String::Number(matIndex) + " >= " + String::Number(matCount) + "), setting it to first material");
			}
		}
		#endif
	}

	void Mesh::Transform(const Matrix4f& matrix)
	{
		NazaraAssert(m_impl, "Mesh should be created first");
		NazaraAssert(m_impl->animationType == AnimationType_Static, "Mesh is not static");

		for (SubMesh* subMesh : m_impl->subMeshes)
		{
			StaticMesh* staticMesh = static_cast<StaticMesh*>(subMesh);

			BufferMapper<VertexBuffer> mapper(staticMesh->GetVertexBuffer(), BufferAccess_ReadWrite);
			MeshVertex* vertices = static_cast<MeshVertex*>(mapper.GetPointer());

			Boxf aabb(vertices->position.x, vertices->position.y, vertices->position.z, 0.f, 0.f, 0.f);

			unsigned int vertexCount = staticMesh->GetVertexCount();
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				vertices->position = matrix.Transform(vertices->position);
				aabb.ExtendTo(vertices->position);

				vertices++;
			}

			staticMesh->SetAABB(aabb);
		}

		InvalidateAABB();
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
