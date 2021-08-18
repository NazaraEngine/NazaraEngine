// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NODECOMPONENT_HPP
#define NAZARA_NODECOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Utility/Node.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API NodeComponent : public Node
	{
		public:
			NodeComponent() = default;
			NodeComponent(const NodeComponent&) = default;
			NodeComponent(NodeComponent&&) noexcept = default;
			~NodeComponent() = default;

			void SetParent(entt::registry& registry, entt::entity entity, bool keepDerived = false);
			using Node::SetParent;

			NodeComponent& operator=(const NodeComponent&) = default;
			NodeComponent& operator=(NodeComponent&&) noexcept = default;
	};
}

#include <Nazara/Utility/Components/NodeComponent.inl>

#endif
