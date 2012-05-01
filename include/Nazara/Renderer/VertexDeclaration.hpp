// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_VERTEXDECLARATION_HPP
#define NAZARA_VERTEXDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <vector>

enum nzElementType
{
	nzElementType_Color,
	nzElementType_Double1,
	nzElementType_Double2,
	nzElementType_Double3,
	nzElementType_Double4,
	nzElementType_Float1,
	nzElementType_Float2,
	nzElementType_Float3,
	nzElementType_Float4
};

enum nzElementUsage
{
	nzElementType_Diffuse,
	nzElementType_Normal,
	nzElementType_Position,
	nzElementType_Tangent,
	nzElementType_TexCoord
};

struct NzVertexElement
{
	NzVertexElement() : stream(0), usageIndex(0) {}

	unsigned int offset;
	unsigned int stream;
	unsigned int usageIndex;
	nzElementType type;
	nzElementUsage usage;
};

class NAZARA_API NzVertexDeclaration
{
	public:
		struct Element
		{
			unsigned int offset;
			unsigned int usageIndex;
			nzElementType type;
			nzElementUsage usage;
		};

		NzVertexDeclaration() = default;
		~NzVertexDeclaration() = default;

		bool Create(const NzVertexElement* elements, unsigned int elementCount);

		const Element* GetElement(unsigned int i, unsigned int stream = 0) const;
		unsigned int GetElementCount(unsigned int stream = 0) const;
		unsigned int GetStreamCount() const;
		unsigned int GetStride(unsigned int stream = 0) const;

	private:
		struct Stream
		{
			std::vector<Element> elements;
			unsigned int stride;
		};

		std::vector<Stream> m_streams;
};

#endif // NAZARA_VERTEXDECLARATION_HPP
