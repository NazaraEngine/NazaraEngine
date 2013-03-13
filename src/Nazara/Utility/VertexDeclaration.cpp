// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

#if NAZARA_UTILITY_THREADSAFE && NAZARA_THREADSAFETY_VERTEXDECLARATION
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Utility/Debug.hpp>

namespace
{
	const unsigned int elementCount[] =
	{
		4, // nzElementType_Color
		1, // nzElementType_Double1
		2, // nzElementType_Double2
		3, // nzElementType_Double3
		4, // nzElementType_Double4
		1, // nzElementType_Float1
		2, // nzElementType_Float2
		3, // nzElementType_Float3
		4  // nzElementType_Float4
	};

	const unsigned int elementSize[] =
	{
		4*sizeof(nzUInt8), // nzElementType_Color
		1*sizeof(double), // nzElementType_Double1
		2*sizeof(double), // nzElementType_Double2
		3*sizeof(double), // nzElementType_Double3
		4*sizeof(double), // nzElementType_Double4
		1*sizeof(float),  // nzElementType_Float1
		2*sizeof(float),  // nzElementType_Float2
		3*sizeof(float),  // nzElementType_Float3
		4*sizeof(float)	  // nzElementType_Float4
	};

	bool VertexElementCompare(const NzVertexElement& elementA, const NzVertexElement& elementB)
	{
		// Nous classons d'abord par stream
		if (elementA.stream == elementB.stream)
		{
			// Ensuite par usage
			if (elementA.usage == elementB.usage)
				// Et finalement par usageIndex
				return elementA.usageIndex < elementB.usageIndex;
			else
				return elementA.usage < elementB.usage;
		}
		else
			return elementA.stream < elementB.stream;
	}
}

struct NzVertexDeclarationImpl
{
	std::vector<NzVertexElement> elements;
	int elementPos[nzElementStream_Max+1][nzElementUsage_Max+1];
	int streamPos[nzElementStream_Max+1];
	unsigned int stride[nzElementStream_Max+1] = {0};

	unsigned short refCount = 1;
	NazaraMutex(mutex)
};

NzVertexDeclaration::NzVertexDeclaration(const NzVertexElement* elements, unsigned int elementCount)
{
	#ifdef NAZARA_DEBUG
	if (!Create(elements, elementCount))
	{
		NazaraError("Failed to create declaration");
		throw std::runtime_error("Constructor failed");
	}
	#else
	Create(elements, elementCount);
	#endif
}

NzVertexDeclaration::NzVertexDeclaration(const NzVertexDeclaration& declaration) :
NzResource(),
m_sharedImpl(declaration.m_sharedImpl)
{
	if (m_sharedImpl)
	{
		NazaraMutexLock(m_sharedImpl->mutex);
		m_sharedImpl->refCount++;
		NazaraMutexUnlock(m_sharedImpl->mutex);
	}
}

NzVertexDeclaration::NzVertexDeclaration(NzVertexDeclaration&& declaration) noexcept :
m_sharedImpl(declaration.m_sharedImpl)
{
	declaration.m_sharedImpl = nullptr;
}

NzVertexDeclaration::~NzVertexDeclaration()
{
	Destroy();
}

bool NzVertexDeclaration::Create(const NzVertexElement* elements, unsigned int elementCount)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (!elements || elementCount == 0)
	{
		NazaraError("No element");
		return false;
	}
	#endif

	NzVertexDeclarationImpl* impl = new NzVertexDeclarationImpl;
	std::memset(&impl->elementPos, -1, (nzElementStream_Max+1)*(nzElementUsage_Max+1)*sizeof(int));
	std::memset(&impl->streamPos, -1, (nzElementStream_Max+1)*sizeof(int));

	// On copie et trie les éléments
	impl->elements.resize(elementCount);
	std::memcpy(&impl->elements[0], elements, elementCount*sizeof(NzVertexElement));
	std::sort(impl->elements.begin(), impl->elements.end(), VertexElementCompare);

	for (unsigned int i = 0; i < elementCount; ++i)
	{
		NzVertexElement& current = impl->elements[i];
		#if NAZARA_UTILITY_SAFE
		// Notre tableau étant trié, s'il y a collision, les deux éléments identiques se suivent...
		if (i > 0)
		{
			NzVertexElement& previous = impl->elements[i-1]; // On accède à l'élément précédent
			if (previous.usage == current.usage && previous.usageIndex == current.usageIndex && previous.stream == current.stream)
			{
				// Les deux éléments sont identiques là où ils ne devraient pas, nous avons une collision...
				NazaraError("Element usage 0x" + NzString::Number(current.usage, 16) + " collision with usage index " + NzString::Number(current.usageIndex) + " on stream 0x" + NzString::Number(current.stream, 16));
				delete impl;

				return false;
			}
		}
		#endif

		if (current.usageIndex == 0)
			impl->elementPos[current.stream][current.usage] = i;

		if (impl->streamPos[current.stream] == -1)
			impl->streamPos[current.stream] = i; // Premier élément du stream (via le triage)

		impl->stride[current.stream] += elementSize[current.type];
	}

	#if NAZARA_UTILITY_FORCE_DECLARATION_STRIDE_MULTIPLE_OF_32
	for (unsigned int& stride : impl->stride)
		stride = ((static_cast<int>(stride)-1)/32+1)*32;
	#endif

	m_sharedImpl = impl;

	NotifyCreated();
	return true;
}

void NzVertexDeclaration::Destroy()
{
	if (!m_sharedImpl)
		return;

	NotifyDestroy();

	NazaraMutexLock(m_sharedImpl->mutex);
	bool freeSharedImpl = (--m_sharedImpl->refCount == 0);
	NazaraMutexUnlock(m_sharedImpl->mutex);

	if (freeSharedImpl)
		delete m_sharedImpl;

	m_sharedImpl = nullptr;
}

const NzVertexElement* NzVertexDeclaration::GetElement(unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return nullptr;
	}

	if (i >= m_sharedImpl->elements.size())
	{
		NazaraError("Element index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_sharedImpl->elements.size()) + ')');
		return nullptr;
	}
	#endif

	return &m_sharedImpl->elements[i];
}

const NzVertexElement* NzVertexDeclaration::GetElement(nzElementStream stream, unsigned int i) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return nullptr;
	}

	int streamPos = m_sharedImpl->streamPos[stream];
	if (streamPos == -1)
	{
		NazaraError("Declaration has no stream 0x" + NzString::Number(stream, 16));
		return nullptr;
	}

	unsigned int upperLimit = GetElementCount(stream);
	if (i >= upperLimit)
	{
		NazaraError("Element index out of range (" + NzString::Number(i) + " >= " + NzString::Number(upperLimit) + ')');
		return nullptr;
	}
	#endif

	return &m_sharedImpl->elements[m_sharedImpl->streamPos[stream]+i];
}

const NzVertexElement* NzVertexDeclaration::GetElement(nzElementStream stream, nzElementUsage usage, unsigned int usageIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return nullptr;
	}
	#endif

	int elementPos = m_sharedImpl->elementPos[stream][usage];
	if (elementPos == -1)
		return nullptr;

	if (usageIndex == 0) // Si l'usage index vaut zéro, alors nous sommes certains d'être sur le bon élément (Majorité des cas)
		return &m_sharedImpl->elements[elementPos];
	else
	{
		elementPos += usageIndex;
		if (static_cast<unsigned int>(elementPos) >= m_sharedImpl->elements.size())
			return nullptr;

		NzVertexElement& element = m_sharedImpl->elements[elementPos];
		if (element.stream != stream || element.usage != usage || element.usageIndex != usageIndex)
			return nullptr;

		return &element;
	}
}

unsigned int NzVertexDeclaration::GetElementCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return 0;
	}
	#endif

	return m_sharedImpl->elements.size();
}

unsigned int NzVertexDeclaration::GetElementCount(nzElementStream stream) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return 0;
	}
	#endif

	int streamPos = m_sharedImpl->streamPos[stream];
	if (streamPos == -1)
		return 0;

	unsigned int upperLimit = 0;
	if (stream == nzElementStream_Max)
		upperLimit = m_sharedImpl->elements.size();
	else
	{
		for (unsigned int upperStream = stream+1; upperStream <= nzElementStream_Max; ++upperStream)
		{
			if (m_sharedImpl->streamPos[upperStream] != -1)
			{
				upperLimit = m_sharedImpl->streamPos[upperStream];
				break;
			}
			else if (upperStream == nzElementStream_Max) // Dernier stream, toujours pas de limite
				upperLimit = m_sharedImpl->elements.size();
		}
	}

	return upperLimit-streamPos;
}

unsigned int NzVertexDeclaration::GetStride(nzElementStream stream) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return 0;
	}
	#endif

	return m_sharedImpl->stride[stream];
}

bool NzVertexDeclaration::HasElement(unsigned int i) const
{
	return i < m_sharedImpl->elements.size();
}

bool NzVertexDeclaration::HasElement(nzElementStream stream, unsigned int i) const
{
	return i < GetElementCount(stream);
}

bool NzVertexDeclaration::HasElement(nzElementStream stream, nzElementUsage usage, unsigned int usageIndex) const
{
	int elementPos = m_sharedImpl->elementPos[stream][usage];
	if (elementPos == -1)
		return false;

	elementPos += usageIndex;
	if (static_cast<unsigned int>(elementPos) >= m_sharedImpl->elements.size())
		return false;

	NzVertexElement& element = m_sharedImpl->elements[elementPos];
	if (element.stream != stream || element.usage != usage || element.usageIndex != usageIndex)
		return false;

	return true;
}

bool NzVertexDeclaration::HasStream(nzElementStream stream) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_sharedImpl)
	{
		NazaraError("Declaration not created");
		return false;
	}
	#endif

	return m_sharedImpl->streamPos[stream] != -1;
}

bool NzVertexDeclaration::IsValid() const
{
	return m_sharedImpl != nullptr;
}

NzVertexDeclaration& NzVertexDeclaration::operator=(const NzVertexDeclaration& declaration)
{
	Destroy();

	m_sharedImpl = declaration.m_sharedImpl;
	if (m_sharedImpl)
	{
		NazaraMutexLock(m_sharedImpl->mutex);
		m_sharedImpl->refCount++;
		NazaraMutexUnlock(m_sharedImpl->mutex);
	}

	return *this;
}

NzVertexDeclaration& NzVertexDeclaration::operator=(NzVertexDeclaration&& declaration) noexcept
{
	Destroy();

	m_sharedImpl = declaration.m_sharedImpl;
	declaration.m_sharedImpl = nullptr;

	return *this;
}

unsigned int NzVertexDeclaration::GetElementCount(nzElementType type)
{
	return elementCount[type];
}

unsigned int NzVertexDeclaration::GetElementSize(nzElementType type)
{
	return elementSize[type];
}
