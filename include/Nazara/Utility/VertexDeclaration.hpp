// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXDECLARATION_HPP
#define NAZARA_VERTEXDECLARATION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/SparsePtr.hpp>
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
			struct Component;
			struct ComponentEntry;

			VertexDeclaration(VertexInputRate inputRate, std::initializer_list<ComponentEntry> components);
			VertexDeclaration(const VertexDeclaration&) = delete;
			VertexDeclaration(VertexDeclaration&&) = default;
			~VertexDeclaration() = default;

			inline const Component* FindComponent(VertexComponent vertexComponent, std::size_t componentIndex) const;

			template<typename T> const Component* GetComponentByType(VertexComponent vertexComponent, std::size_t componentIndex = 0) const;

			inline const Component& GetComponent(std::size_t componentIndex) const;
			inline std::size_t GetComponentCount() const;
			inline const std::vector<Component>& GetComponents() const;
			inline VertexInputRate GetInputRate() const;
			inline std::size_t GetStride() const;

			inline bool HasComponent(VertexComponent component, std::size_t componentIndex = 0) const;
			template<typename T> bool HasComponentOfType(VertexComponent vertexComponent, std::size_t componentIndex = 0) const;

			VertexDeclaration& operator=(const VertexDeclaration&) = delete;
			VertexDeclaration& operator=(VertexDeclaration&&) = default;

			static inline const VertexDeclarationRef& Get(VertexLayout layout);
			static bool IsTypeSupported(ComponentType type);
			template<typename... Args> static VertexDeclarationRef New(Args&&... args);

			struct Component
			{
				ComponentType type;
				VertexComponent component;
				std::size_t componentIndex;
				std::size_t offset;
			};

			struct ComponentEntry
			{
				VertexComponent component;
				ComponentType type;
				std::size_t componentIndex;
			};

		private:
			static bool Initialize();
			static void Uninitialize();

			std::vector<Component> m_components;
			std::size_t m_stride;
			VertexInputRate m_inputRate;

			static std::array<VertexDeclarationRef, VertexLayout_Max + 1> s_declarations;
			static VertexDeclarationLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Utility/VertexDeclaration.inl>

#endif // NAZARA_VERTEXDECLARATION_HPP
