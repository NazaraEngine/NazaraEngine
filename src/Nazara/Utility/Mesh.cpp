// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <cstring>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

NzMeshParams::NzMeshParams()
{
	if (!NzBuffer::IsSupported(storage))
		storage = nzBufferStorage_Software;
}

bool NzMeshParams::IsValid() const
{
	if (!NzBuffer::IsSupported(storage))
	{
		NazaraError("Storage not supported");
		return false;
	}

	if (scale == NzVector3f::Zero())
	{
		NazaraError("Invalid scale");
		return false;
	}

	return true;
}

struct NzMeshImpl
{
	NzMeshImpl()
	{
		materials.resize(1); // Un matériau par défaut
	}

	std::unordered_map<NzString, unsigned int> subMeshMap;
	std::vector<NzString> materials;
	std::vector<NzSubMesh*> subMeshes;
	nzAnimationType animationType;
	NzBoxf aabb;
	NzSkeleton skeleton; // Uniquement pour les meshs squelettiques
	NzString animationPath;
	bool aabbUpdated = false;
	unsigned int jointCount; // Uniquement pour les meshs squelettiques
};

NzMesh::~NzMesh()
{
	Destroy();
}

bool NzMesh::AddSubMesh(NzSubMesh* subMesh)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}

	if (!subMesh)
	{
		NazaraError("Invalid submesh");
		return false;
	}

	if (subMesh->GetAnimationType() != m_impl->animationType)
	{
		NazaraError("Submesh animation type must match mesh animation type");
		return false;
	}
	#endif

	subMesh->AddResourceListener(this, m_impl->subMeshes.size());

	m_impl->aabbUpdated = false; // On invalide l'AABB
	m_impl->subMeshes.push_back(subMesh);

	return true;
}

bool NzMesh::AddSubMesh(const NzString& identifier, NzSubMesh* subMesh)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}

	if (identifier.IsEmpty())
	{
		NazaraError("Identifier is empty");
		return false;
	}

	auto it = m_impl->subMeshMap.find(identifier);
	if (it != m_impl->subMeshMap.end())
	{
		NazaraError("SubMesh identifier \"" + identifier + "\" is already used");
		return false;
	}

	if (!subMesh)
	{
		NazaraError("Invalid submesh");
		return false;
	}

	if (m_impl->animationType != subMesh->GetAnimationType())
	{
		NazaraError("Submesh animation type must match mesh animation type");
		return false;
	}
	#endif

	int index = m_impl->subMeshes.size();

	subMesh->AddResourceListener(this, index);

	m_impl->aabbUpdated = false; // On invalide l'AABB
	m_impl->subMeshes.push_back(subMesh);
	m_impl->subMeshMap[identifier] = index;

	return true;
}

void NzMesh::BuildSubMesh(const NzPrimitive& primitive, const NzMeshParams& params)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (m_impl->animationType != nzAnimationType_Static)
	{
		NazaraError("Mesh must be static");
		return;
	}

	if (!params.IsValid())
	{
		NazaraError("Parameters must be valid");
		return;
	}
	#endif

	NzBoxf aabb;
	std::unique_ptr<NzIndexBuffer> indexBuffer;
	std::unique_ptr<NzVertexBuffer> vertexBuffer;

	NzMatrix4f matrix(primitive.matrix);
	matrix.ApplyScale(params.scale);

	switch (primitive.type)
	{
		case nzPrimitiveType_Box:
		{
			unsigned int indexCount;
			unsigned int vertexCount;
			NzComputeBoxIndexVertexCount(primitive.box.subdivision, &indexCount, &vertexCount);

			indexBuffer.reset(new NzIndexBuffer(indexCount, vertexCount > std::numeric_limits<nzUInt16>::max(), params.storage, nzBufferUsage_Static));
			indexBuffer->SetPersistent(false);

			vertexBuffer.reset(new NzVertexBuffer(GetDeclaration(), vertexCount, params.storage, nzBufferUsage_Static));
			vertexBuffer->SetPersistent(false);

			NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
			NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_WriteOnly);

			NzGenerateBox(primitive.box.lengths, primitive.box.subdivision, matrix, static_cast<NzMeshVertex*>(vertexMapper.GetPointer()), indexMapper.begin(), &aabb);
			break;
		}

		case nzPrimitiveType_Plane:
		{
			unsigned int indexCount;
			unsigned int vertexCount;
			NzComputePlaneIndexVertexCount(primitive.plane.subdivision, &indexCount, &vertexCount);

			indexBuffer.reset(new NzIndexBuffer(indexCount, vertexCount > std::numeric_limits<nzUInt16>::max(), params.storage, nzBufferUsage_Static));
			indexBuffer->SetPersistent(false);

			vertexBuffer.reset(new NzVertexBuffer(GetDeclaration(), vertexCount, params.storage, nzBufferUsage_Static));
			vertexBuffer->SetPersistent(false);

			NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
			NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_WriteOnly);

			NzGeneratePlane(primitive.plane.subdivision, primitive.plane.size, matrix, static_cast<NzMeshVertex*>(vertexMapper.GetPointer()), indexMapper.begin(), &aabb);
			break;
		}

		case nzPrimitiveType_Sphere:
		{
			switch (primitive.sphere.type)
			{
				case nzSphereType_Cubic:
				{
					unsigned int indexCount;
					unsigned int vertexCount;
					NzComputeCubicSphereIndexVertexCount(primitive.sphere.cubic.subdivision, &indexCount, &vertexCount);

					indexBuffer.reset(new NzIndexBuffer(indexCount, vertexCount > std::numeric_limits<nzUInt16>::max(), params.storage, nzBufferUsage_Static));
					indexBuffer->SetPersistent(false);

					vertexBuffer.reset(new NzVertexBuffer(GetDeclaration(), vertexCount, params.storage, nzBufferUsage_Static));
					vertexBuffer->SetPersistent(false);

					NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
					NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_WriteOnly);

					NzGenerateCubicSphere(primitive.sphere.size, primitive.sphere.cubic.subdivision, matrix, static_cast<NzMeshVertex*>(vertexMapper.GetPointer()), indexMapper.begin(), &aabb);
					break;
				}

				case nzSphereType_Ico:
				{
					unsigned int indexCount;
					unsigned int vertexCount;
					NzComputeIcoSphereIndexVertexCount(primitive.sphere.ico.recursionLevel, &indexCount, &vertexCount);

					indexBuffer.reset(new NzIndexBuffer(indexCount, vertexCount > std::numeric_limits<nzUInt16>::max(), params.storage, nzBufferUsage_Static));
					indexBuffer->SetPersistent(false);

					vertexBuffer.reset(new NzVertexBuffer(GetDeclaration(), vertexCount, params.storage, nzBufferUsage_Static));
					vertexBuffer->SetPersistent(false);

					NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
					NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_WriteOnly);

					NzGenerateIcoSphere(primitive.sphere.size, primitive.sphere.ico.recursionLevel, matrix, static_cast<NzMeshVertex*>(vertexMapper.GetPointer()), indexMapper.begin(), &aabb);
					break;
				}

				case nzSphereType_UV:
				{
					unsigned int indexCount;
					unsigned int vertexCount;
					NzComputeUvSphereIndexVertexCount(primitive.sphere.uv.sliceCount, primitive.sphere.uv.stackCount, &indexCount, &vertexCount);

					indexBuffer.reset(new NzIndexBuffer(indexCount, vertexCount > std::numeric_limits<nzUInt16>::max(), params.storage, nzBufferUsage_Static));
					indexBuffer->SetPersistent(false);

					vertexBuffer.reset(new NzVertexBuffer(GetDeclaration(), vertexCount, params.storage, nzBufferUsage_Static));
					vertexBuffer->SetPersistent(false);

					NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
					NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_WriteOnly);

					NzGenerateUvSphere(primitive.sphere.size, primitive.sphere.uv.sliceCount, primitive.sphere.uv.stackCount, matrix, static_cast<NzMeshVertex*>(vertexMapper.GetPointer()), indexMapper.begin(), &aabb);
					break;
				}
			}
			break;
		}
	}

	std::unique_ptr<NzStaticMesh> subMesh(new NzStaticMesh(this));
	if (!subMesh->Create(vertexBuffer.get()))
	{
		NazaraError("Failed to create StaticMesh");
		return;
	}
	vertexBuffer.release();

	if (params.optimizeIndexBuffers)
		indexBuffer->Optimize();

	subMesh->SetIndexBuffer(indexBuffer.get());
	indexBuffer.release();

	subMesh->SetAABB(aabb);

	if (AddSubMesh(subMesh.get()))
		subMesh.release();
}

void NzMesh::BuildSubMeshes(const NzPrimitiveList& list, const NzMeshParams& params)
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

bool NzMesh::CreateSkeletal(unsigned int jointCount)
{
	Destroy();

	m_impl = new NzMeshImpl;
	m_impl->animationType = nzAnimationType_Skeletal;
	m_impl->jointCount = jointCount;
	if (!m_impl->skeleton.Create(jointCount))
	{
		delete m_impl;
		m_impl = nullptr;

		NazaraError("Failed to create skeleton");
		return false;
	}

	NotifyCreated();
	return true;
}

bool NzMesh::CreateStatic()
{
	Destroy();

	m_impl = new NzMeshImpl;
	m_impl->animationType = nzAnimationType_Static;

	NotifyCreated();
	return true;
}

void NzMesh::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		for (NzSubMesh* subMesh : m_impl->subMeshes)
			subMesh->RemoveResourceListener(this);

		delete m_impl;
		m_impl = nullptr;
	}
}

void NzMesh::GenerateNormals()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}
	#endif

	for (NzSubMesh* subMesh : m_impl->subMeshes)
		subMesh->GenerateNormals();
}

void NzMesh::GenerateNormalsAndTangents()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}
	#endif

	for (NzSubMesh* subMesh : m_impl->subMeshes)
		subMesh->GenerateNormalsAndTangents();
}

void NzMesh::GenerateTangents()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}
	#endif

	for (NzSubMesh* subMesh : m_impl->subMeshes)
		subMesh->GenerateTangents();
}

const NzBoxf& NzMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");

		static NzBoxf dummy;
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

NzString NzMesh::GetAnimation() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return NzString();
	}
	#endif

	return m_impl->animationPath;
}

nzAnimationType NzMesh::GetAnimationType() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return nzAnimationType_Static;
	}
	#endif

	return m_impl->animationType;
}

unsigned int NzMesh::GetJointCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}

	if (m_impl->animationType != nzAnimationType_Skeletal)
	{
		NazaraError("Mesh's animation type is not skeletal");
		return 0;
	}
	#endif

	return m_impl->jointCount;
}

NzString NzMesh::GetMaterial(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return NzString();
	}

	if (index >= m_impl->materials.size())
	{
		NazaraError("Material index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->materials.size()) + ')');
		return NzString();
	}
	#endif

	return m_impl->materials[index];
}

unsigned int NzMesh::GetMaterialCount() const
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

NzSkeleton* NzMesh::GetSkeleton()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (m_impl->animationType != nzAnimationType_Skeletal)
	{
		NazaraError("Mesh's animation type is not skeletal");
		return nullptr;
	}
	#endif

	return &m_impl->skeleton;
}

const NzSkeleton* NzMesh::GetSkeleton() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (m_impl->animationType != nzAnimationType_Skeletal)
	{
		NazaraError("Mesh's animation type is not skeletal");
		return nullptr;
	}
	#endif

	return &m_impl->skeleton;
}

NzSubMesh* NzMesh::GetSubMesh(const NzString& identifier)
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

NzSubMesh* NzMesh::GetSubMesh(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return nullptr;
	}

	if (index >= m_impl->subMeshes.size())
	{
		NazaraError("SubMesh index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->subMeshes.size()) + ')');
		return nullptr;
	}
	#endif

	return m_impl->subMeshes[index];
}

const NzSubMesh* NzMesh::GetSubMesh(const NzString& identifier) const
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

const NzSubMesh* NzMesh::GetSubMesh(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return nullptr;
	}

	if (index >= m_impl->subMeshes.size())
	{
		NazaraError("SubMesh index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->subMeshes.size()) + ')');
		return nullptr;
	}
	#endif

	return m_impl->subMeshes[index];
}

unsigned int NzMesh::GetSubMeshCount() const
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

int NzMesh::GetSubMeshIndex(const NzString& identifier) const
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

unsigned int NzMesh::GetTriangleCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}
	#endif

	unsigned int triangleCount = 0;
	for (NzSubMesh* subMesh : m_impl->subMeshes)
		triangleCount += subMesh->GetTriangleCount();

	return triangleCount;
}

unsigned int NzMesh::GetVertexCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}
	#endif

	unsigned int vertexCount = 0;
	for (NzSubMesh* subMesh : m_impl->subMeshes)
		vertexCount += subMesh->GetVertexCount();

	return vertexCount;
}

void NzMesh::InvalidateAABB() const
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

bool NzMesh::HasSubMesh(const NzString& identifier) const
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

bool NzMesh::HasSubMesh(unsigned int index) const
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

bool NzMesh::IsAnimable() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}
	#endif

	return m_impl->animationType != nzAnimationType_Static;
}

bool NzMesh::IsValid() const
{
	return m_impl != nullptr;
}

bool NzMesh::LoadFromFile(const NzString& filePath, const NzMeshParams& params)
{
	return NzMeshLoader::LoadFromFile(this, filePath, params);
}

bool NzMesh::LoadFromMemory(const void* data, std::size_t size, const NzMeshParams& params)
{
	return NzMeshLoader::LoadFromMemory(this, data, size, params);
}

bool NzMesh::LoadFromStream(NzInputStream& stream, const NzMeshParams& params)
{
	return NzMeshLoader::LoadFromStream(this, stream, params);
}

void NzMesh::Recenter()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (m_impl->animationType != nzAnimationType_Static)
	{
		NazaraError("Mesh must be static");
		return;
	}
	#endif

	// Le centre de notre mesh est le centre de l'AABB *globale*
	NzVector3f center = GetAABB().GetCenter();

	for (NzSubMesh* subMesh : m_impl->subMeshes)
	{
		NzStaticMesh* staticMesh = static_cast<NzStaticMesh*>(subMesh);

		NzBufferMapper<NzVertexBuffer> mapper(staticMesh->GetVertexBuffer(), nzBufferAccess_ReadWrite);
		NzMeshVertex* vertices = static_cast<NzMeshVertex*>(mapper.GetPointer());

		unsigned int vertexCount = staticMesh->GetVertexCount();
		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			vertices->position -= center;
			vertices++;
		}

		// l'AABB ne change pas de dimensions mais seulement de position, appliquons-lui le même procédé
		NzBoxf aabb = staticMesh->GetAABB();
		aabb.Translate(-center);

		staticMesh->SetAABB(aabb);
	}

	// Il ne faut pas oublier d'invalider notre AABB
	m_impl->aabbUpdated = false;
}

void NzMesh::RemoveSubMesh(const NzString& identifier)
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
	std::advance(it, index);

	// On libère la ressource
	(*it2)->RemoveResourceListener(this);
	m_impl->subMeshes.erase(it2);

	m_impl->aabbUpdated = false; // On invalide l'AABB
}

void NzMesh::RemoveSubMesh(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (index >= m_impl->subMeshes.size())
	{
		NazaraError("SubMesh index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->subMeshes.size()) + ')');
		return;
	}
	#endif

	// On déplace l'itérateur du début de x
	auto it = m_impl->subMeshes.begin();
	std::advance(it, index);

	// On libère la ressource
	(*it)->RemoveResourceListener(this);
	m_impl->subMeshes.erase(it);

	m_impl->aabbUpdated = false; // On invalide l'AABB
}

void NzMesh::SetAnimation(const NzString& animationPath)
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

void NzMesh::SetMaterial(unsigned int matIndex, const NzString& materialPath)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (matIndex >= m_impl->materials.size())
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_impl->materials.size()) + ')');
		return;
	}
	#endif

	m_impl->materials[matIndex] = materialPath;
}

void NzMesh::SetMaterialCount(unsigned int matCount)
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
	for (NzSubMesh* subMesh : m_impl->subMeshes)
	{
		unsigned int matIndex = subMesh->GetMaterialIndex();
		if (matIndex >= matCount)
		{
			subMesh->SetMaterialIndex(0); // Pour empêcher un crash
			NazaraWarning("SubMesh " + NzString::Pointer(subMesh) + " material index is over mesh new material count (" + NzString::Number(matIndex) + " >= " + NzString::Number(matCount) + "), setting it to first material");
		}
	}
	#endif
}

void NzMesh::Transform(const NzMatrix4f& matrix)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (m_impl->animationType != nzAnimationType_Static)
	{
		NazaraError("Mesh must be static");
		return;
	}
	#endif

	if (matrix.IsIdentity())
		return;

	for (NzSubMesh* subMesh : m_impl->subMeshes)
	{
		NzStaticMesh* staticMesh = static_cast<NzStaticMesh*>(subMesh);

		NzBufferMapper<NzVertexBuffer> mapper(staticMesh->GetVertexBuffer(), nzBufferAccess_ReadWrite);
		NzMeshVertex* vertices = static_cast<NzMeshVertex*>(mapper.GetPointer());

		NzBoxf aabb(vertices->position.x, vertices->position.y, vertices->position.z, 0.f, 0.f, 0.f);

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

const NzVertexDeclaration* NzMesh::GetDeclaration()
{
	static NzVertexDeclaration declaration;

	if (!declaration.IsValid())
	{
		// Déclaration correspondant à NzVertexStruct_XYZ_Normal_UV_Tangent
		NzVertexElement elements[4];
		elements[0].offset = 0;
		elements[0].type = nzElementType_Float3;
		elements[0].usage = nzElementUsage_Position;

		elements[1].offset = 3*sizeof(float);
		elements[1].type = nzElementType_Float3;
		elements[1].usage = nzElementUsage_Normal;

		elements[2].offset = 3*sizeof(float) + 3*sizeof(float);
		elements[2].type = nzElementType_Float2;
		elements[2].usage = nzElementUsage_TexCoord;

		elements[3].offset = 3*sizeof(float) + 3*sizeof(float) + 2*sizeof(float);
		elements[3].type = nzElementType_Float3;
		elements[3].usage = nzElementUsage_Tangent;

		declaration.Create(elements, 4);
	}

	return &declaration;
}

void NzMesh::OnResourceReleased(const NzResource* resource, int index)
{
	NazaraUnused(resource);

	RemoveSubMesh(index);
}

NzMeshLoader::LoaderList NzMesh::s_loaders;
