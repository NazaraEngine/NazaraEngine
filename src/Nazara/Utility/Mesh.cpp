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

		if (scale == Vector3f::Zero())
		{
			NazaraError("Invalid scale");
			return false;
		}

		return true;
	}

	struct MeshImpl
	{
		MeshImpl()
		{
			materials.resize(1); // Un matériau par défaut
		}

		std::unordered_map<String, unsigned int> subMeshMap;
		std::vector<String> materials;
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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (!subMesh)
		{
			NazaraError("Invalid submesh");
			return;
		}

		if (subMesh->GetAnimationType() != m_impl->animationType)
		{
			NazaraError("Submesh animation type must match mesh animation type");
			return;
		}
		#endif

		m_impl->aabbUpdated = false; // On invalide l'AABB
		m_impl->subMeshes.push_back(subMesh);
	}

	void Mesh::AddSubMesh(const String& identifier, SubMesh* subMesh)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (identifier.IsEmpty())
		{
			NazaraError("Identifier is empty");
			return;
		}

		auto it = m_impl->subMeshMap.find(identifier);
		if (it != m_impl->subMeshMap.end())
		{
			NazaraError("SubMesh identifier \"" + identifier + "\" is already used");
			return;
		}

		if (!subMesh)
		{
			NazaraError("Invalid submesh");
			return;
		}

		if (m_impl->animationType != subMesh->GetAnimationType())
		{
			NazaraError("Submesh animation type must match mesh animation type");
			return;
		}
		#endif

		int index = m_impl->subMeshes.size();

		m_impl->aabbUpdated = false; // On invalide l'AABB
		m_impl->subMeshes.push_back(subMesh);
		m_impl->subMeshMap[identifier] = index;
	}

	SubMesh* Mesh::BuildSubMesh(const Primitive& primitive, const MeshParams& params)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return nullptr;
		}

		if (m_impl->animationType != AnimationType_Static)
		{
			NazaraError("Mesh must be static");
			return nullptr;
		}

		if (!params.IsValid())
		{
			NazaraError("Parameters must be valid");
			return nullptr;
		}
		#endif

		Boxf aabb;
		IndexBufferRef indexBuffer;
		VertexBufferRef vertexBuffer;

		Matrix4f matrix(primitive.matrix);
		matrix.ApplyScale(params.scale);

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
		unsigned int primitiveCount = list.GetSize();

		#if NAZARA_UTILITY_SAFE
		if (primitiveCount == 0)
		{
			NazaraError("PrimitiveList must have at least one primitive");
			return;
		}
		#endif

		for (unsigned int i = 0; i < primitiveCount; ++i)
			BuildSubMesh(list.GetPrimitive(i), params);
	}

	bool Mesh::CreateSkeletal(unsigned int jointCount)
	{
		Destroy();

		m_impl = new MeshImpl;
		m_impl->animationType = AnimationType_Skeletal;
		m_impl->jointCount = jointCount;
		if (!m_impl->skeleton.Create(jointCount))
		{
			delete m_impl;
			m_impl = nullptr;

			NazaraError("Failed to create skeleton");
			return false;
		}

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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}
		#endif

		for (SubMesh* subMesh : m_impl->subMeshes)
			subMesh->GenerateNormals();
	}

	void Mesh::GenerateNormalsAndTangents()
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}
		#endif

		for (SubMesh* subMesh : m_impl->subMeshes)
			subMesh->GenerateNormalsAndTangents();
	}

	void Mesh::GenerateTangents()
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}
		#endif

		for (SubMesh* subMesh : m_impl->subMeshes)
			subMesh->GenerateTangents();
	}

	const Boxf& Mesh::GetAABB() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");

			static Boxf dummy;
			return dummy;
		}
		#endif

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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return String();
		}
		#endif

		return m_impl->animationPath;
	}

	AnimationType Mesh::GetAnimationType() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return AnimationType_Static;
		}
		#endif

		return m_impl->animationType;
	}

	unsigned int Mesh::GetJointCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return 0;
		}

		if (m_impl->animationType != AnimationType_Skeletal)
		{
			NazaraError("Mesh's animation type is not skeletal");
			return 0;
		}
		#endif

		return m_impl->jointCount;
	}

	String Mesh::GetMaterial(unsigned int index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return String();
		}

		if (index >= m_impl->materials.size())
		{
			NazaraError("Material index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->materials.size()) + ')');
			return String();
		}
		#endif

		return m_impl->materials[index];
	}

	unsigned int Mesh::GetMaterialCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return 0;
		}
		#endif

		return m_impl->materials.size();
	}

	Skeleton* Mesh::GetSkeleton()
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}

		if (m_impl->animationType != AnimationType_Skeletal)
		{
			NazaraError("Mesh's animation type is not skeletal");
			return nullptr;
		}
		#endif

		return &m_impl->skeleton;
	}

	const Skeleton* Mesh::GetSkeleton() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}

		if (m_impl->animationType != AnimationType_Skeletal)
		{
			NazaraError("Mesh's animation type is not skeletal");
			return nullptr;
		}
		#endif

		return &m_impl->skeleton;
	}

	SubMesh* Mesh::GetSubMesh(const String& identifier)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return nullptr;
		}
		#endif

		auto it = m_impl->subMeshMap.find(identifier);

		#if NAZARA_UTILITY_SAFE
		if (it == m_impl->subMeshMap.end())
		{
			NazaraError("SubMesh not found");
			return nullptr;
		}
		#endif

		return m_impl->subMeshes[it->second];
	}

	SubMesh* Mesh::GetSubMesh(unsigned int index)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return nullptr;
		}

		if (index >= m_impl->subMeshes.size())
		{
			NazaraError("SubMesh index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->subMeshes.size()) + ')');
			return nullptr;
		}
		#endif

		return m_impl->subMeshes[index];
	}

	const SubMesh* Mesh::GetSubMesh(const String& identifier) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return nullptr;
		}
		#endif

		auto it = m_impl->subMeshMap.find(identifier);

		#if NAZARA_UTILITY_SAFE
		if (it == m_impl->subMeshMap.end())
		{
			NazaraError("SubMesh not found");
			return nullptr;
		}
		#endif

		return m_impl->subMeshes[it->second];
	}

	const SubMesh* Mesh::GetSubMesh(unsigned int index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return nullptr;
		}

		if (index >= m_impl->subMeshes.size())
		{
			NazaraError("SubMesh index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->subMeshes.size()) + ')');
			return nullptr;
		}
		#endif

		return m_impl->subMeshes[index];
	}

	unsigned int Mesh::GetSubMeshCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return 0;
		}
		#endif

		return m_impl->subMeshes.size();
	}

	int Mesh::GetSubMeshIndex(const String& identifier) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return -1;
		}
		#endif

		auto it = m_impl->subMeshMap.find(identifier);

		#if NAZARA_UTILITY_SAFE
		if (it == m_impl->subMeshMap.end())
		{
			NazaraError("SubMesh not found");
			return -1;
		}
		#endif

		return it->second;
	}

	unsigned int Mesh::GetTriangleCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return 0;
		}
		#endif

		unsigned int triangleCount = 0;
		for (SubMesh* subMesh : m_impl->subMeshes)
			triangleCount += subMesh->GetTriangleCount();

		return triangleCount;
	}

	unsigned int Mesh::GetVertexCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return 0;
		}
		#endif

		unsigned int vertexCount = 0;
		for (SubMesh* subMesh : m_impl->subMeshes)
			vertexCount += subMesh->GetVertexCount();

		return vertexCount;
	}

	void Mesh::InvalidateAABB() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}
		#endif

		m_impl->aabbUpdated = false;
	}

	bool Mesh::HasSubMesh(const String& identifier) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return false;
		}
		#endif

		return m_impl->subMeshMap.find(identifier) != m_impl->subMeshMap.end();
	}

	bool Mesh::HasSubMesh(unsigned int index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return false;
		}
		#endif

		return index < m_impl->subMeshes.size();
	}

	bool Mesh::IsAnimable() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return false;
		}
		#endif

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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (m_impl->animationType != AnimationType_Static)
		{
			NazaraError("Mesh must be static");
			return;
		}
		#endif

		// Le centre de notre mesh est le centre de l'AABB *globale*
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

			// l'AABB ne change pas de dimensions mais seulement de position, appliquons-lui le même procédé
			Boxf aabb = staticMesh->GetAABB();
			aabb.Translate(-center);

			staticMesh->SetAABB(aabb);
		}

		// Il ne faut pas oublier d'invalider notre AABB
		m_impl->aabbUpdated = false;
	}

	void Mesh::RemoveSubMesh(const String& identifier)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		auto it = m_impl->subMeshMap.find(identifier);
		if (it == m_impl->subMeshMap.end())
		{
			NazaraError("SubMesh not found");
			return;
		}

		unsigned int index = it->second;
		#else
		unsigned int index = m_impl->subMeshMap[identifier];
		#endif

		// On déplace l'itérateur du début d'une distance de x
		auto it2 = m_impl->subMeshes.begin();
		std::advance(it2, index);
		m_impl->subMeshes.erase(it2);

		m_impl->aabbUpdated = false; // On invalide l'AABB
	}

	void Mesh::RemoveSubMesh(unsigned int index)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (index >= m_impl->subMeshes.size())
		{
			NazaraError("SubMesh index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->subMeshes.size()) + ')');
			return;
		}
		#endif

		// On déplace l'itérateur du début de x
		auto it = m_impl->subMeshes.begin();
		std::advance(it, index);
		m_impl->subMeshes.erase(it);

		m_impl->aabbUpdated = false; // On invalide l'AABB
	}

	void Mesh::SetAnimation(const String& animationPath)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}
		#endif

		m_impl->animationPath = animationPath;
	}

	void Mesh::SetMaterial(unsigned int matIndex, const String& materialPath)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (matIndex >= m_impl->materials.size())
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_impl->materials.size()) + ')');
			return;
		}
		#endif

		m_impl->materials[matIndex] = materialPath;
	}

	void Mesh::SetMaterialCount(unsigned int matCount)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (matCount == 0)
		{
			NazaraError("A mesh should have at least a material");
			return;
		}
		#endif

		m_impl->materials.resize(matCount);

		#ifdef NAZARA_DEBUG
		for (SubMesh* subMesh : m_impl->subMeshes)
		{
			unsigned int matIndex = subMesh->GetMaterialIndex();
			if (matIndex >= matCount)
			{
				subMesh->SetMaterialIndex(0); // Pour empêcher un crash
				NazaraWarning("SubMesh " + String::Pointer(subMesh) + " material index is over mesh new material count (" + String::Number(matIndex) + " >= " + String::Number(matCount) + "), setting it to first material");
			}
		}
		#endif
	}

	void Mesh::Transform(const Matrix4f& matrix)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Mesh not created");
			return;
		}

		if (m_impl->animationType != AnimationType_Static)
		{
			NazaraError("Mesh must be static");
			return;
		}
		#endif

		if (matrix.IsIdentity())
			return;

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

		// Il ne faut pas oublier d'invalider notre AABB
		m_impl->aabbUpdated = false;
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
}
