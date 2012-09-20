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
	std::deque<NzString> skins;
	std::map<NzString, unsigned int> subMeshMap;
	std::vector<NzSubMesh*> subMeshes;
	nzAnimationType animationType;
	NzAxisAlignedBox aabb;
	const NzAnimation* animation = nullptr;
};

NzMesh::~NzMesh()
{
	Destroy();
}

bool NzMesh::AddSkin(const NzString& skin, bool setDefault)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Mesh not created");
		return false;
	}

	if (skin.IsEmpty())
	{
		NazaraError("Skin is empty");
		return false;
	}
	#endif

	if (setDefault)
		m_impl->skins.push_front(skin);
	else
		m_impl->skins.push_back(skin);

	return true;
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
	#endif

	subMesh->AddResourceListener(this, m_impl->subMeshes.size());

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
	#endif

	int index = m_impl->subMeshes.size();

	subMesh->AddResourceListener(this, index);

	m_impl->aabb.SetNull(); // On invalide l'AABB
	m_impl->subMeshes.push_back(subMesh);
	m_impl->subMeshMap[identifier] = index;

	return true;
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

	NotifyCreated();

	return true;
}

void NzMesh::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		if (m_impl->animation)
			m_impl->animation->RemoveResourceListener(this);

		for (NzSubMesh* subMesh : m_impl->subMeshes)
			subMesh->RemoveResourceListener(this);

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

	auto it = m_impl->subMeshMap.find(identifier);
	if (it == m_impl->subMeshMap.end())
	{
		NazaraError("SubMesh not found");
		return -1;
	}

	return it->second;
	#else
	return m_impl->subMeshMap[identifier];
	#endif
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

	// On accède à l'itérateur correspondant à l'entrée #index
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
		m_impl->animation->RemoveResourceListener(this);

	if (animation)
	{
		#if NAZARA_UTILITY_SAFE
		if (animation->GetType() != m_impl->animationType)
		{
			NazaraError("Animation's type must match mesh animation type");
			return false;
		}
		#endif

		animation->AddResourceListener(this);
	}

	m_impl->animation = animation;

	return true;
}

void NzMesh::OnResourceCreated(const NzResource* resource, int index)
{
	NazaraUnused(index);

	if (resource == m_impl->animation)
	{
		#if NAZARA_UTILITY_SAFE
		if (m_impl->animation->GetType() != m_impl->animationType)
		{
			NazaraError("Animation's type must match mesh animation type");

			m_impl->animation->RemoveResourceListener(this);
			m_impl->animation = nullptr;
		}
		#endif
	}
}

void NzMesh::OnResourceReleased(const NzResource* resource, int index)
{
	if (resource == m_impl->animation)
		SetAnimation(nullptr);
	else
		RemoveSubMesh(index);
}

NzMeshLoader::LoaderList NzMesh::s_loaders;
