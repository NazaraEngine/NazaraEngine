// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <map>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

struct NzAnimationImpl
{
	std::map<NzString, unsigned int> sequenceMap;
	std::vector<NzSequence> sequences;
	nzAnimationType type;
	unsigned int frameCount;
};

bool NzAnimationParams::IsValid() const
{
	if (startFrame > endFrame)
	{
		NazaraError("Start frame must be lower than end frame");
		return false;
	}

	return true;
}

NzAnimation::~NzAnimation()
{
	Destroy();
}

unsigned int NzAnimation::AddSequence(const NzSequence& sequence)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return 0;
	}
	#endif

	unsigned int index = m_impl->sequences.size();

	if (!sequence.name.IsEmpty())
	{
		#if NAZARA_UTILITY_SAFE
		auto it = m_impl->sequenceMap.find(sequence.name);
		if (it != m_impl->sequenceMap.end())
		{
			NazaraError("Sequence name \"" + sequence.name + "\" is already used");
			return 0;
		}
		#endif

		m_impl->sequenceMap[sequence.name] = index;
	}

	m_impl->sequences.push_back(sequence);

	return index;
}

bool NzAnimation::Create(nzAnimationType type, unsigned int frameCount)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (type == nzAnimationType_Static)
	{
		NazaraError("Invalid type");
		return false;
	}

	if (frameCount == 0)
	{
		NazaraError("Frame count must be over zero");
		return false;
	}
	#endif

	m_impl = new NzAnimationImpl;
	m_impl->frameCount = frameCount;
	m_impl->type = type;

	return true;
}

void NzAnimation::Destroy()
{
	if (m_impl)
	{
		delete m_impl;
		m_impl = nullptr;
	}
}

unsigned int NzAnimation::GetFrameCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return false;
	}
	#endif

	return m_impl->frameCount;
}

NzSequence* NzAnimation::GetSequence(const NzString& sequenceName)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	auto it = m_impl->sequenceMap.find(sequenceName);
	if (it == m_impl->sequenceMap.end())
	{
		NazaraError("Sequence not found");
		return nullptr;
	}

	return &m_impl->sequences[it->second];
	#else
	return &m_impl->sequences[m_impl->sequenceMap[sequenceName]];
	#endif
}

NzSequence* NzAnimation::GetSequence(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (index >= m_impl->sequences.size())
	{
		NazaraError("Sequence index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->sequences.size()) + ')');
		return nullptr;
	}
	#endif

	return &m_impl->sequences[index];
}

const NzSequence* NzAnimation::GetSequence(const NzString& sequenceName) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	auto it = m_impl->sequenceMap.find(sequenceName);
	if (it == m_impl->sequenceMap.end())
	{
		NazaraError("Sequence not found");
		return nullptr;
	}

	return &m_impl->sequences[it->second];
	#else
	return &m_impl->sequences[m_impl->sequenceMap[sequenceName]];
	#endif
}

const NzSequence* NzAnimation::GetSequence(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (index >= m_impl->sequences.size())
	{
		NazaraError("Sequence index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->sequences.size()) + ')');
		return nullptr;
	}
	#endif

	return &m_impl->sequences[index];
}

unsigned int NzAnimation::GetSequenceCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return 0;
	}
	#endif

	return m_impl->sequences.size();
}

nzAnimationType NzAnimation::GetType() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nzAnimationType_Static; // Ce qui est une valeur invalide pour NzAnimation
	}
	#endif

	return m_impl->type;
}

bool NzAnimation::HasSequence(const NzString& sequenceName) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return false;
	}
	#endif

	return m_impl->sequenceMap.find(sequenceName) != m_impl->sequenceMap.end();
}

bool NzAnimation::HasSequence(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return false;
	}
	#endif

	return index >= m_impl->sequences.size();
}

bool NzAnimation::IsValid() const
{
	return m_impl != nullptr;
}

bool NzAnimation::LoadFromFile(const NzString& filePath, const NzAnimationParams& params)
{
	return NzAnimationLoader::LoadFromFile(this, filePath, params);
}

bool NzAnimation::LoadFromMemory(const void* data, std::size_t size, const NzAnimationParams& params)
{
	return NzAnimationLoader::LoadFromMemory(this, data, size, params);
}

bool NzAnimation::LoadFromStream(NzInputStream& stream, const NzAnimationParams& params)
{
	return NzAnimationLoader::LoadFromStream(this, stream, params);
}

void NzAnimation::RemoveSequence(const NzString& identifier)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return;
	}

	auto it = m_impl->sequenceMap.find(identifier);
	if (it == m_impl->sequenceMap.end())
	{
		NazaraError("SubMesh not found");
		return;
	}

	unsigned int index = it->second;
	#else
	unsigned int index = m_impl->sequenceMap[identifier];
	#endif

	auto it2 = m_impl->sequences.begin();
	std::advance(it2, index);

	m_impl->sequences.erase(it2);
}

void NzAnimation::RemoveSequence(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return;
	}

	if (index >= m_impl->sequences.size())
	{
		NazaraError("Sequence index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->sequences.size()) + ')');
		return;
	}
	#endif

	auto it = m_impl->sequences.begin();
	std::advance(it, index);

	m_impl->sequences.erase(it);
}

NzAnimationLoader::LoaderList NzAnimation::s_loaders;
