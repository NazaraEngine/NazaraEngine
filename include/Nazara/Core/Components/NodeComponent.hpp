// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_COMPONENTS_NODECOMPONENT_HPP
#define NAZARA_CORE_COMPONENTS_NODECOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Node.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_CORE_API NodeComponent : public Node
	{
		public:
			using Node::Node;
			NodeComponent(const NodeComponent&) = default;
			NodeComponent(NodeComponent&&) noexcept = default;
			~NodeComponent() = default;

			void SetParent(entt::handle entity, bool keepDerived = false);
			void SetParentJoint(entt::handle entity, std::string_view jointName, bool keepDerived = false);
			void SetParentJoint(entt::handle entity, std::size_t jointIndex, bool keepDerived = false);
			using Node::SetParent;

			NodeComponent& operator=(const NodeComponent&) = default;
			NodeComponent& operator=(NodeComponent&&) noexcept = default;
	};
}

#include <Nazara/Core/Components/NodeComponent.inl>

#endif // NAZARA_CORE_COMPONENTS_NODECOMPONENT_HPP
