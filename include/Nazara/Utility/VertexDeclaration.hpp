// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_VERTEXDECLARATION_HPP
#define NAZARA_VERTEXDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Utility/Enums.hpp>

struct NzVertexElement
{
	unsigned int offset;
	unsigned int usageIndex = 0;
	nzElementStream stream = nzElementStream_VertexData;
	nzElementType type;
	nzElementUsage usage;
};

struct NzVertexDeclarationImpl;

class NAZARA_API NzVertexDeclaration : public NzResource
{
	public:
		NzVertexDeclaration() = default;
		NzVertexDeclaration(const NzVertexElement* elements, unsigned int elementCount);
		NzVertexDeclaration(const NzVertexDeclaration& declaration);
		NzVertexDeclaration(NzVertexDeclaration&& declaration) noexcept;
		~NzVertexDeclaration();

		bool Create(const NzVertexElement* elements, unsigned int elementCount);
		void Destroy();

		const NzVertexElement* GetElement(unsigned int i) const;
		const NzVertexElement* GetElement(nzElementStream stream, unsigned int i) const;
		const NzVertexElement* GetElement(nzElementStream stream, nzElementUsage usage, unsigned int usageIndex = 0) const;
		unsigned int GetElementCount() const;
		unsigned int GetElementCount(nzElementStream stream) const;
		unsigned int GetStride(nzElementStream stream) const;

		bool HasStream(nzElementStream stream) const;

		bool IsValid() const;

		NzVertexDeclaration& operator=(const NzVertexDeclaration& declaration);
		NzVertexDeclaration& operator=(NzVertexDeclaration&& declaration) noexcept;

	private:
		NzVertexDeclarationImpl* m_sharedImpl = nullptr;
};

#endif // NAZARA_VERTEXDECLARATION_HPP
