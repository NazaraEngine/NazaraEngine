// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <cstring>
#include <deque>
#include <map>
#include <Nazara/Utility/Debug.hpp>

bool NzMeshParams::IsValid() const
{
	if (!animation.IsValid())
	{
		NazaraError("Invalid animation parameters");
		return false;
	}

	if (!NzBuffer::IsSupported(storage))
	{
		NazaraError("Storage not supported");
		return false;
	}

	return true;
}

struct NzMeshImpl
{
	std::map<NzString, nzUInt8> subMeshMap;
	std::deque<NzString> skins;
	std::vector<NzSubMesh*> subMeshes;
	nzAnimationType animationType;
	NzAxisAlignedBox aabb;
	const NzAnimation* animation = nullptr;
};

NzMesh::~NzMesh()
{
	Destroy();
}

unsigned int NzMesh::AddSkin(const NzString& skin, bool setDefault)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}

	if (skin.IsEmpty())
	{
		NazaraError("Skin is empty");
		return 0;
	}
	#endif

	if (setDefault)
		m_impl->skins.push_front(skin);
	else
		m_impl->skins.push_back(skin);

	return m_impl->skins.size()-1;
}

nzUInt8 NzMesh::AddSubMesh(NzSubMesh* subMesh)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}

	if (!subMesh)
	{
		NazaraError("Invalid submesh");
		return 0;
	}
	#endif

	subMesh->AddResourceReference();

	m_impl->aabb.SetNull(); // On invalide l'AABB
	m_impl->subMeshes.push_back(subMesh);

	return m_impl->subMeshes.size()-1;
}

nzUInt8 NzMesh::AddSubMesh(const NzString& identifier, NzSubMesh* subMesh)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}

	if (identifier.IsEmpty())
	{
		NazaraError("Identifier is empty");
		return 0;
	}

	auto it = m_impl->subMeshMap.find(identifier);
	if (it != m_impl->subMeshMap.end())
	{
		NazaraError("SubMesh identifier \"" + identifier + "\" is already used");
		return it->second;
	}

	if (!subMesh)
	{
		NazaraError("Invalid submesh");
		return 0;
	}
	#endif

	nzUInt8 index = m_impl->subMeshes.size();

	subMesh->AddResourceReference();

	m_impl->aabb.SetNull(); // On invalide l'AABB
	m_impl->subMeshes.push_back(subMesh);
	m_impl->subMeshMap[identifier] = index;

	return index;
}

void NzMesh::Animate(unsigned int frameA, unsigned int frameB, float interpolation)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (!m_impl->animation)
	{
		NazaraError("Mesh has no animation");
		return;
	}

	unsigned int frameCount = m_impl->animation->GetFrameCount();
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

	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return;
	}
	#endif

	for (NzSubMesh* subMesh : m_impl->subMeshes)
		subMesh->AnimateImpl(frameA, frameB, interpolation);

	m_impl->aabb.SetNull(); // On invalide l'AABB
}

bool NzMesh::Create(nzAnimationType type)
{
	Destroy();

	m_impl = new NzMeshImpl;
	m_impl->animationType = type;

	return true;
}

void NzMesh::Destroy()
{
	if (m_impl)
	{
		if (m_impl->animation)
			m_impl->animation->RemoveResourceReference();

		for (NzSubMesh* subMesh : m_impl->subMeshes)
			subMesh->RemoveResourceReference();

		delete m_impl;
		m_impl = nullptr;
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

const NzAnimation* NzMesh::GetAnimation() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->animation;
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

unsigned int NzMesh::GetFrameCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}

	if (!m_impl->animation)
	{
		NazaraError("Mesh has no animation");
		return 0;
	}
	#endif

	return m_impl->animation->GetFrameCount();
}

NzString NzMesh::GetSkin(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return NzString();
	}

	if (index >= m_impl->skins.size())
	{
		NazaraError("Skin index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->skins.size()) + ')');
		return NzString();
	}
	#endif

	return m_impl->skins[index];
}

unsigned int NzMesh::GetSkinCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return 0;
	}
	#endif

	return m_impl->skins.size();
}

NzSubMesh* NzMesh::GetSubMesh(const NzString& identifier)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return nullptr;
	}

	auto it = m_impl->subMeshMap.find(identifier);
	if (it == m_impl->subMeshMap.end())
	{
		NazaraError("SubMesh not found");
		return nullptr;
	}

	return m_impl->subMeshes[it->second];
	#else
	return m_impl->subMeshes[m_impl->subMeshMap[identifier]];
	#endif
}

NzSubMesh* NzMesh::GetSubMesh(nzUInt8 index)
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

	auto it = m_impl->subMeshMap.find(identifier);
	if (it == m_impl->subMeshMap.end())
	{
		NazaraError("SubMesh not found");
		return nullptr;
	}

	return m_impl->subMeshes[it->second];
	#else
	return m_impl->subMeshes[m_impl->subMeshMap[identifier]];
	#endif
}

const NzSubMesh* NzMesh::GetSubMesh(nzUInt8 index) const
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

nzUInt8 NzMesh::GetSubMeshCount() const
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

bool NzMesh::HasAnimation() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}
	#endif

	return m_impl->animation != nullptr;
}

bool NzMesh::HasSkin(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}
	#endif

	return m_impl->skins.size() > index;
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

bool NzMesh::HasSubMesh(nzUInt8 index) const
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

void NzMesh::RemoveSkin(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return;
	}

	if (m_impl->skins.size() <= index)
	{
		NazaraError("Skin index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->skins.size()) + ')');
		return;
	}
	#endif

	auto it = m_impl->skins.begin();
	std::advance(it, index);

	m_impl->skins.erase(it);
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

	auto it2 = m_impl->subMeshes.begin();
	std::advance(it2, index);

	m_impl->subMeshes.erase(it2);

	m_impl->aabb.SetNull(); // On invalide l'AABB
}

void NzMesh::RemoveSubMesh(nzUInt8 index)
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

	auto it = m_impl->subMeshes.begin();
	std::advance(it, index);

	m_impl->subMeshes.erase(it);

	m_impl->aabb.SetNull(); // On invalide l'AABB
}

bool NzMesh::SetAnimation(const NzAnimation* animation)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}

	if (m_impl->animationType == nzAnimationType_Static)
	{
		NazaraError("Static meshes cannot have animation");
		return false;
	}
	#endif

	if (animation == m_impl->animation)
		return true;

	if (m_impl->animation)
		m_impl->animation->RemoveResourceReference();

	if (animation)
	{
		#if NAZARA_UTILITY_SAFE
		if (animation->GetType() != m_impl->animationType)
		{
			NazaraError("Animation's type must match mesh animation type");
			return false;
		}
		#endif

		m_impl->animation = animation;
		m_impl->animation->AddResourceReference();
	}
	else
		m_impl->animation = nullptr;

	return true;
}

NzMeshLoader::LoaderList NzMesh::s_loaders;
