// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_NODECOMPONENT_HPP
#define NDK_COMPONENTS_NODECOMPONENT_HPP

#include <Nazara/Utility/Node.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class Entity;
	class NodeComponent;

	using NodeComponentHandle = Nz::ObjectHandle<NodeComponent>;

	class NDK_API NodeComponent : public Component<NodeComponent>, public Nz::Node
	{
		public:
			NodeComponent() = default;
			~NodeComponent() = default;

			void SetParent(Entity* entity, bool keepDerived = false);
			using Nz::Node::SetParent;

			static ComponentIndex componentIndex;
	};
}

#include <NDK/Components/NodeComponent.inl>

#endif // NDK_COMPONENTS_NODECOMPONENT_HPP
