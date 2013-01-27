// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/KeyframeMesh.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <cstring>
#include <deque>
#include <map>
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

	return true;
}

struct NzMeshImpl
{
	std::map<NzString, unsigned int> subMeshMap;
	std::vector<NzString> materials;
	std::vector<NzSubMesh*> subMeshes;
	nzAnimationType animationType;
	NzAxisAlignedBox aabb;
	NzSkeleton skeleton; // Uniquement pour les animations squelettiques
	NzString animationPath;
	unsigned int jointCount; // Uniquement pour les animations squelettiques
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
	subMesh->Finish();

	m_impl->aabb.SetNull(); // On invalide l'AABB
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
	subMesh->Finish();

	m_impl->aabb.SetNull(); // On invalide l'AABB
	m_impl->subMeshes.push_back(subMesh);
	m_impl->subMeshMap[identifier] = index;

	return true;
}

void NzMesh::Animate(const NzAnimation* animation, unsigned int frameA, unsigned int frameB, float interpolation, NzSkeleton* skeleton) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (!animation || !animation->IsValid())
	{
		NazaraError("Animation must be valid");
		return;
	}

	if (animation->GetType() != m_impl->animationType)
	{
		NazaraError("Animation type must match mesh animation type");
		return;
	}

	unsigned int frameCount = animation->GetFrameCount();
	if (frameA >= frameCount)
	{
		NazaraError("Frame A is out of range (" + NzString::Number(frameA) + " >= " + NzString::Number(frameCount) + ')');
		return;
	}

	if (frameB >= frameCount)
	{
		NazaraError("Frame B is out of range (" + NzString::Number(frameB) + " >= " + NzString::Number(frameCount) + ')');
		return;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return;
	}
	#endif

	switch (m_impl->animationType)
	{
		case nzAnimationType_Keyframe:
			for (NzSubMesh* subMesh : m_impl->subMeshes)
			{
				NzKeyframeMesh* keyframeMesh = static_cast<NzKeyframeMesh*>(subMesh);
				keyframeMesh->InterpolateImpl(frameA, frameB, interpolation);
			}
			break;

		case nzAnimationType_Skeletal:
			#if NAZARA_UTILITY_SAFE
			if (!skeleton)
			{
				NazaraError("Skeleton must be valid for skeletal animation");
				return;
			}
			#endif

			animation->AnimateSkeleton(skeleton, frameA, frameB, interpolation);
			for (NzSubMesh* subMesh : m_impl->subMeshes)
			{
				NzSkeletalMesh* skeletalMesh = static_cast<NzSkeletalMesh*>(subMesh);
				skeletalMesh->Skin(skeleton);
			}
			break;

		case nzAnimationType_Static:
			// Le safe mode est censé nous protéger de cet appel
			NazaraInternalError("Static mesh has no animation, please enable safe mode");
			break;
	}

	m_impl->aabb.SetNull(); // On invalide l'AABB
}

bool NzMesh::CreateKeyframe()
{
	Destroy();

	m_impl = new NzMeshImpl;
	m_impl->animationType = nzAnimationType_Keyframe;

	NotifyCreated();
	return true;
}

bool NzMesh::CreateSkeletal(unsigned int jointCount)
{
	Destroy();

	m_impl = new NzMeshImpl;
	m_impl->animationType = nzAnimationType_Skeletal;
	m_impl->jointCount = jointCount;
	if (!m_impl->skeleton.Create(jointCount))
	{
		NazaraError("Failed to create skeleton");

		delete m_impl;
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
	{
		NzVertexMapper mapper1(subMesh);
		unsigned int vertexCount = mapper1.GetVertexCount();
		for (unsigned int i = 0; i < vertexCount; ++i)
			mapper1.SetNormal(i, NzVector3f::Zero());

		mapper1.Unmap();

		NzTriangleIterator iterator(subMesh);
		do
		{
			NzVector3f pos0 = iterator.GetPosition(0);

			NzVector3f dv[2];
			dv[0] = iterator.GetPosition(1) - pos0;
			dv[1] = iterator.GetPosition(2) - pos0;

			NzVector3f normal = dv[0].CrossProduct(dv[1]);

			for (unsigned int i = 0; i < 3; ++i)
				iterator.SetNormal(i, iterator.GetNormal(i) + normal);
		}
		while (iterator.Advance());

		NzVertexMapper mapper2(subMesh);
		for (unsigned int i = 0; i < vertexCount; ++i)
			mapper2.SetNormal(i, NzVector3f::Normalize(mapper2.GetNormal(i)));
	}
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
	{
		NzVertexMapper mapper1(subMesh);
		unsigned int vertexCount = mapper1.GetVertexCount();
		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			mapper1.SetNormal(i, NzVector3f::Zero());
			mapper1.SetTangent(i, NzVector3f::Zero());
		}

		mapper1.Unmap();

		NzTriangleIterator iterator(subMesh);
		do
		{
			NzVector3f pos0 = iterator.GetPosition(0);
			NzVector2f uv0 = iterator.GetTexCoords(0);

			NzVector3f dv[2];
			dv[0] = iterator.GetPosition(1) - pos0;
			dv[1] = iterator.GetPosition(2) - pos0;

			NzVector3f normal = dv[0].CrossProduct(dv[1]);

			NzVector2f duv[2];
			duv[0] = iterator.GetTexCoords(1) - uv0;
			duv[1] = iterator.GetTexCoords(2) - uv0;

			float coef = 1.f / (duv[0].x*duv[1].y - duv[1].x*duv[0].y);

			NzVector3f tangent;
			tangent.x = coef * (dv[0].x*duv[1].y + dv[1].x*(-duv[0].y));
			tangent.y = coef * (dv[0].y*duv[1].y + dv[1].y*(-duv[0].y));
			tangent.z = coef * (dv[0].z*duv[1].y + dv[1].z*(-duv[0].y));

			for (unsigned int i = 0; i < 3; ++i)
			{
				iterator.SetNormal(i, iterator.GetNormal(i) + normal);
				iterator.SetTangent(i, iterator.GetTangent(i) + tangent);
			}
		}
		while (iterator.Advance());

		NzVertexMapper mapper2(subMesh);
		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			mapper2.SetNormal(i, NzVector3f::Normalize(mapper2.GetNormal(i)));
			mapper2.SetTangent(i, NzVector3f::Normalize(mapper2.GetTangent(i)));
		}
	}
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
	{
		NzTriangleIterator iterator(subMesh);
		do
		{
			NzVector3f pos0 = iterator.GetPosition(0);
			NzVector2f uv0 = iterator.GetTexCoords(0);

			NzVector3f dv[2];
			dv[0] = iterator.GetPosition(1) - pos0;
			dv[1] = iterator.GetPosition(2) - pos0;

			NzVector2f duv[2];
			duv[0] = iterator.GetTexCoords(1) - uv0;
			duv[1] = iterator.GetTexCoords(2) - uv0;

			float ds[2];
			ds[0] = iterator.GetTexCoords(1).x - uv0.x;
			ds[1] = iterator.GetTexCoords(2).x - uv0.x;

			NzVector3f ppt;
			ppt.x = ds[0]*dv[1].x - dv[0].x*ds[1];
			ppt.y = ds[0]*dv[1].y - dv[0].y*ds[1];
			ppt.z = ds[0]*dv[1].z - dv[0].z*ds[1];
			ppt.Normalize();

			for (unsigned int i = 0; i < 3; ++i)
			{
				NzVector3f normal = iterator.GetNormal(i);
				float d = ppt.DotProduct(normal);

				NzVector3f tangent = ppt - (d * normal);

				iterator.SetTangent(i, tangent);
			}
		}
		while (iterator.Advance());
	}
}

const NzAxisAlignedBox& NzMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return NzAxisAlignedBox::Null;
	}
	#endif

	if (m_impl->aabb.IsNull())
	{
		for (NzSubMesh* subMesh : m_impl->subMeshes)
			m_impl->aabb.ExtendTo(subMesh->GetAABB());
	}

	return m_impl->aabb;
}

NzString NzMesh::GetAnimation() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return nzAnimationType_Static;
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

	m_impl->aabb.SetNull();
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

	m_impl->aabb.SetNull(); // On invalide l'AABB
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

	m_impl->aabb.SetNull(); // On invalide l'AABB
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
	#endif

	m_impl->materials.resize(matCount);

	#ifdef NAZARA_DEBUG
	for (NzSubMesh* subMesh : m_impl->subMeshes)
	{
		unsigned int matIndex = subMesh->GetMaterialIndex();
		if (matIndex >= matCount)
			NazaraWarning("SubMesh " + NzString::Pointer(subMesh) + " material index is over mesh new material count (" + NzString::Number(matIndex) + " >= " + NzString::Number(matCount));
	}
	#endif
}

void NzMesh::Skin(const NzSkeleton* skeleton) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (m_impl->animationType != nzAnimationType_Skeletal)
	{
		NazaraError("Mesh's animation type is not skeletal");
		return;
	}
	#endif

	for (NzSubMesh* subMesh : m_impl->subMeshes)
	{
		NzSkeletalMesh* skeletalMesh = static_cast<NzSkeletalMesh*>(subMesh);
		skeletalMesh->Skin(skeleton);
	}
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
