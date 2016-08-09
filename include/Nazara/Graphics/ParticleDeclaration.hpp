// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEDECLARATION_HPP
#define NAZARA_PARTICLEDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <array>

namespace Nz
{
	class ParticleDeclaration;

	using ParticleDeclarationConstRef = ObjectRef<const ParticleDeclaration>;
	using ParticleDeclarationLibrary = ObjectLibrary<ParticleDeclaration>;
	using ParticleDeclarationRef = ObjectRef<ParticleDeclaration>;

	class NAZARA_GRAPHICS_API ParticleDeclaration : public RefCounted
	{
		friend ParticleDeclarationLibrary;
		friend class Graphics;

		public:
			ParticleDeclaration();
			ParticleDeclaration(const ParticleDeclaration& declaration);
			~ParticleDeclaration();

			void DisableComponent(ParticleComponent component);
			void EnableComponent(ParticleComponent component, ComponentType type, unsigned int offset);

			void GetComponent(ParticleComponent component, bool* enabled, ComponentType* type, unsigned int* offset) const;
			unsigned int GetStride() const;

			void SetStride(unsigned int stride);

			ParticleDeclaration& operator=(const ParticleDeclaration& declaration);

			static ParticleDeclaration* Get(ParticleLayout layout);
			static bool IsTypeSupported(ComponentType type);
			template<typename... Args> static ParticleDeclarationRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnParticleDeclarationRelease, const ParticleDeclaration* /*particleDeclaration*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			struct Component
			{
				ComponentType type;
				bool enabled = false;
				unsigned int offset;

				/*
				** -Lynix:
				** It would be also possible to precise the stride by an independant way, what I don't allow
				** to decomplexify the interface of something I consider useless.
				** If you think that could be useful, don't hesitate to make me aware !
				*/
			};

			std::array<Component, ParticleComponent_Max + 1> m_components;
			unsigned int m_stride;

			static std::array<ParticleDeclaration, ParticleLayout_Max + 1> s_declarations;
			static ParticleDeclarationLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/ParticleDeclaration.inl>

#endif // NAZARA_PARTICLEDECLARATION_HPP
