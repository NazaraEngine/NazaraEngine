// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <cassert>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline auto VertexDeclaration::FindComponent(VertexComponent vertexComponent, std::size_t componentIndex) const -> const Component*
	{
		assert(componentIndex == 0 || vertexComponent == VertexComponent_Userdata);

		for (const Component& component : m_components)
		{
			if (component.component == vertexComponent && component.componentIndex == componentIndex)
				return &component;
		}

		return nullptr;
	}

	inline auto VertexDeclaration::GetComponent(std::size_t componentIndex) const -> const Component&
	{
		return m_components[componentIndex];
	}

	inline std::size_t VertexDeclaration::GetComponentCount() const
	{
		return m_components.size();
	}

	inline auto VertexDeclaration::GetComponents() const -> const std::vector<Component>&
	{
		return m_components;
	}

	inline VertexInputRate VertexDeclaration::GetInputRate() const
	{
		return m_inputRate;
	}

	inline std::size_t VertexDeclaration::GetStride() const
	{
		return m_stride;
	}

	inline bool VertexDeclaration::HasComponent(VertexComponent component, std::size_t componentIndex) const
	{
		return FindComponent(component, componentIndex) != nullptr;
	}

	template<typename T>
	auto VertexDeclaration::GetComponentByType(VertexComponent vertexComponent, std::size_t componentIndex) const -> const Component*
	{
		NazaraAssert(componentIndex == 0 || vertexComponent == VertexComponent_Userdata, "Only userdata vertex component can have component indexes");
		if (const Component* component = FindComponent(vertexComponent, componentIndex))
		{
			if (GetComponentTypeOf<T>() == component->type)
				return component;
		}

		return nullptr;
	}

	template<typename T>
	bool VertexDeclaration::HasComponentOfType(VertexComponent vertexComponent, std::size_t componentIndex) const
	{
		return GetComponentByType<T>(vertexComponent, componentIndex) != nullptr;
	}

	inline const VertexDeclarationRef& VertexDeclaration::Get(VertexLayout layout)
	{
		NazaraAssert(layout <= VertexLayout_Max, "Vertex layout out of enum");

		return s_declarations[layout];
	}

	template<typename... Args>
	VertexDeclarationRef VertexDeclaration::New(Args&&... args)
	{
		std::unique_ptr<VertexDeclaration> object = std::make_unique<VertexDeclaration>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
