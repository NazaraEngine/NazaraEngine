// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXDECLARATION_HPP
#define NAZARA_VERTEXDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzVertexDeclaration;

using NzVertexDeclarationConstRef = NzResourceRef<const NzVertexDeclaration>;
using NzVertexDeclarationRef = NzResourceRef<NzVertexDeclaration>;

struct NzVertexDeclarationImpl;

class NAZARA_API NzVertexDeclaration : public NzResource
{
	friend class NzUtility;

	public:
		NzVertexDeclaration();
		NzVertexDeclaration(NzVertexDeclaration& declaration);
		~NzVertexDeclaration();

		void DisableAttribute(nzAttributeUsage usage);
		void EnableAttribute(nzAttributeUsage usage, nzAttributeType type, unsigned int offset);

		void GetAttribute(nzAttributeUsage usage, bool* enabled, nzAttributeType* type, unsigned int* offset) const;
		unsigned int GetStride() const;

		void SetStride(unsigned int stride);

		NzVertexDeclaration& operator=(const NzVertexDeclaration& declaration);

		static NzVertexDeclaration* Get(nzVertexLayout layout);
		static unsigned int GetAttributeSize(nzAttributeType type);

	private:
		static bool Initialize();
		static void Uninitialize();

		struct Attribute
		{
			nzAttributeType type;
			bool enabled = false;
			unsigned int offset;
			// Il serait aussi possible de préciser le stride de façon indépendante, ce que je ne permets pas
			// pour décomplexifier l'interface en enlevant quelque chose d'inutile.
			// Si vous pensez que ça peut être utile, n'hésitez pas à me le faire savoir !
			// PS: Même cas pour le diviseur (instancing)
		};

		Attribute m_attributes[nzAttributeUsage_Max+1];
		unsigned int m_stride;

		static NzVertexDeclaration s_declarations[nzVertexLayout_Max+1];
};

#endif // NAZARA_VERTEXDECLARATION_HPP
