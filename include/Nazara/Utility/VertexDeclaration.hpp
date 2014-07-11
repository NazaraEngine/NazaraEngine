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

class NAZARA_API NzVertexDeclaration : public NzResource
{
	friend class NzUtility;

	public:
		NzVertexDeclaration();
		NzVertexDeclaration(const NzVertexDeclaration& declaration);
		~NzVertexDeclaration();

		void DisableComponent(nzVertexComponent component);
		void EnableComponent(nzVertexComponent component, nzComponentType type, unsigned int offset);

		void GetComponent(nzVertexComponent component, bool* enabled, nzComponentType* type, unsigned int* offset) const;
		unsigned int GetStride() const;

		void SetStride(unsigned int stride);

		NzVertexDeclaration& operator=(const NzVertexDeclaration& declaration);

		static NzVertexDeclaration* Get(nzVertexLayout layout);
		static bool IsTypeSupported(nzComponentType type);

	private:
		static bool Initialize();
		static void Uninitialize();

		struct Component
		{
			nzComponentType type;
			bool enabled = false;
			unsigned int offset;

			/*
			** -Lynix:
			** Il serait aussi possible de préciser le stride de façon indépendante, ce que je ne permets pas
			** pour décomplexifier l'interface en enlevant quelque chose que je juge inutile.
			** Si vous pensez que ça peut être utile, n'hésitez pas à me le faire savoir !
			** PS: Même cas pour le diviseur (instancing)
			*/
		};

		Component m_components[nzVertexComponent_Max+1];
		unsigned int m_stride;

		static NzVertexDeclaration s_declarations[nzVertexLayout_Max+1];
};

#endif // NAZARA_VERTEXDECLARATION_HPP
