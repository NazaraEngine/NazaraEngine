// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_VERTEXDECLARATION_HPP
#define NAZARA_CORE_VERTEXDECLARATION_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <array>
#include <vector>

namespace Nz
{
	class VertexDeclaration;

	using VertexDeclarationLibrary = ObjectLibrary<VertexDeclaration>;

	class NAZARA_CORE_API VertexDeclaration
	{
		friend VertexDeclarationLibrary;
		friend class Core;

		public:
			struct Component;
			struct ComponentEntry;

			VertexDeclaration(VertexInputRate inputRate, std::initializer_list<ComponentEntry> components);
			VertexDeclaration(const VertexDeclaration&) = delete;
			VertexDeclaration(VertexDeclaration&&) = delete;
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
			VertexDeclaration& operator=(VertexDeclaration&&) = delete;

			static inline const std::shared_ptr<VertexDeclaration>& Get(VertexLayout layout);
			static bool IsTypeSupported(ComponentType type);

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

			static EnumArray<VertexLayout, std::shared_ptr<VertexDeclaration>> s_declarations;
	};
}

#include <Nazara/Core/VertexDeclaration.inl>

#endif // NAZARA_CORE_VERTEXDECLARATION_HPP
