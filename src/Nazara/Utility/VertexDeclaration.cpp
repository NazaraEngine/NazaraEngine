// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	const unsigned int size[] =
	{
		4,	// nzElementType_Color
		8,	// nzElementType_Double1
		16,	// nzElementType_Double2
		24,	// nzElementType_Double3
		32,	// nzElementType_Double4
		4,	// nzElementType_Float1
		8,	// nzElementType_Float2
		12,	// nzElementType_Float3
		16	// nzElementType_Float4
	};
}

bool NzVertexDeclaration::Create(const NzVertexElement* elements, unsigned int elementCount)
{
	for (unsigned int i = 0; i < elementCount; ++i)
	{
		unsigned int stream = elements[i].stream;

		if (stream >= m_streams.size())
			m_streams.resize(stream+1);
		#if NAZARA_UTILITY_SAFE
		else // Seulement si le stream ne vient pas d'être créé (Autrement c'est inutile)
		{
			for (unsigned int j = 0; j < i; ++j)
			{
				if (elements[j].stream == stream && elements[j].usage == elements[i].usage && elements[j].usageIndex == elements[i].usageIndex)
				{
					NazaraError("Element usage (" + NzString::Number(elements[j].usage, 16) + ") collision on stream " + NzString::Number(stream) + " with usage index " + NzString::Number(elements[j].usageIndex));
					return false;
				}
			}
		}
		#endif

		Element element;
		element.offset = elements[i].offset;
		element.type = elements[i].type;
		element.usage = elements[i].usage;
		element.usageIndex = elements[i].usageIndex;

		m_streams[stream].elements.push_back(element);
	}

	for (Stream& stream : m_streams)
	{
		stream.stride = 0;
		for (const Element& element : stream.elements)
			stream.stride += size[element.type];

		#if NAZARA_RENDERER_FORCE_DECLARATION_STRIDE_MULTIPLE_OF_32
		stream.stride = ((static_cast<int>(stream.stride)-1)/32+1)*32;
		#endif
	}

	return true;
}

const NzVertexDeclaration::Element* NzVertexDeclaration::GetElement(unsigned int i, unsigned int stream) const
{
	#if NAZARA_UTILITY_SAFE
	if (stream >= m_streams.size())
	{
		NazaraError("Stream out of range");
		return nullptr;
	}

	if (i >= m_streams[stream].elements.size())
	{
		NazaraError("Index out of range");
		return nullptr;
	}
	#endif

	return &m_streams[stream].elements[i];
}

unsigned int NzVertexDeclaration::GetElementCount(unsigned int stream) const
{
	#if NAZARA_UTILITY_SAFE
	if (stream >= m_streams.size())
	{
		NazaraError("Stream out of range");
		return 0;
	}
	#endif

	return m_streams[stream].elements.size();
}

unsigned int NzVertexDeclaration::GetStreamCount() const
{
	return m_streams.size();
}

unsigned int NzVertexDeclaration::GetStride(unsigned int stream) const
{
	#if NAZARA_UTILITY_SAFE
	if (stream >= m_streams.size())
	{
		NazaraError("Stream out of range");
		return 0;
	}
	#endif

	return m_streams[stream].stride;
}
