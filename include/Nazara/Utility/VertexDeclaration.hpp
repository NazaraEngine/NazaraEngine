// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXDECLARATION_HPP
#define NAZARA_VERTEXDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <array>

namespace Nz
{
	class VertexDeclaration;

	using VertexDeclarationConstRef = ObjectRef<const VertexDeclaration>;
	using VertexDeclarationLibrary = ObjectLibrary<VertexDeclaration>;
	using VertexDeclarationRef = ObjectRef<VertexDeclaration>;

	class NAZARA_UTILITY_API VertexDeclaration : public RefCounted
	{
		friend VertexDeclarationLibrary;
		friend class Utility;

		public:
			VertexDeclaration();
			VertexDeclaration(const VertexDeclaration& declaration);
			~VertexDeclaration();

			void DisableComponent(VertexComponent component);
			void EnableComponent(VertexComponent component, ComponentType type, std::size_t offset);

			void GetComponent(VertexComponent component, bool* enabled, ComponentType* type, std::size_t* offset) const;
			template<typename T> bool HaveComponent(VertexComponent component) const;
			std::size_t GetStride() const;

			void SetStride(std::size_t stride);

			VertexDeclaration& operator=(const VertexDeclaration& declaration);

			static VertexDeclaration* Get(VertexLayout layout);
			static bool IsTypeSupported(ComponentType type);
			template<typename... Args> static VertexDeclarationRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnVertexDeclarationRelease, const VertexDeclaration* /*vertexDeclaration*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			struct Component
			{
				ComponentType type; // Le type de donnée
				bool enabled = false; // Ce composant est-il activé ?/
				std::size_t offset;  // La position, en octets, de la première donnée

				/*
				** -Lynix:
				** Il serait aussi possible de préciser le stride de façon indépendante, ce que je ne permets pas
				** pour décomplexifier l'interface en enlevant quelque chose que je juge inutile.
				** Si vous pensez que ça peut être utile, n'hésitez pas à me le faire savoir !
				** PS: Même cas pour le diviseur (instancing)
				*/
			};

			std::array<Component, VertexComponent_Max + 1> m_components;
			std::size_t m_stride;

			static std::array<VertexDeclaration, VertexLayout_Max + 1> s_declarations;
			static VertexDeclarationLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Utility/VertexDeclaration.inl>

#endif // NAZARA_VERTEXDECLARATION_HPP
