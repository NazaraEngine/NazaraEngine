// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEDECLARATION_HPP
#define NAZARA_PARTICLEDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzParticleDeclaration;

using NzParticleDeclarationConstListener = NzObjectListenerWrapper<const NzParticleDeclaration>;
using NzParticleDeclarationConstRef = NzObjectRef<const NzParticleDeclaration>;
using NzParticleDeclarationLibrary = NzObjectLibrary<NzParticleDeclaration>;
using NzParticleDeclarationListener = NzObjectListenerWrapper<NzParticleDeclaration>;
using NzParticleDeclarationRef = NzObjectRef<NzParticleDeclaration>;

class NAZARA_API NzParticleDeclaration : public NzRefCounted
{
	friend NzParticleDeclarationLibrary;
	friend class NzGraphics;

	public:
		NzParticleDeclaration();
		NzParticleDeclaration(const NzParticleDeclaration& declaration);
		~NzParticleDeclaration();

		void DisableComponent(nzParticleComponent component);
		void EnableComponent(nzParticleComponent component, nzComponentType type, unsigned int offset);

		void GetComponent(nzParticleComponent component, bool* enabled, nzComponentType* type, unsigned int* offset) const;
		unsigned int GetStride() const;

		void SetStride(unsigned int stride);

		NzParticleDeclaration& operator=(const NzParticleDeclaration& declaration);

		static NzParticleDeclaration* Get(nzParticleLayout layout);
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
			*/
		};

		Component m_components[nzParticleComponent_Max+1];
		unsigned int m_stride;

		static NzParticleDeclaration s_declarations[nzParticleLayout_Max+1];
		static NzParticleDeclarationLibrary::LibraryMap s_library;
};

#endif // NAZARA_PARTICLEDECLARATION_HPP
