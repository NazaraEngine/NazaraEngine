// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_DEBUGCOMPONENT_HPP
#define NDK_COMPONENTS_DEBUGCOMPONENT_HPP

#include <Nazara/Core/Flags.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	enum class DebugDraw
	{
		//TODO: Collider2D
		Collider3D,
		GraphicsAABB,
		GraphicsOBB,

		Max = GraphicsOBB
	};
}

namespace Nz
{
	template<>
	struct EnumAsFlags<Ndk::DebugDraw>
	{
		static constexpr Ndk::DebugDraw max = Ndk::DebugDraw::GraphicsOBB;
	};
}

namespace Ndk
{
	using DebugDrawFlags = Nz::Flags<DebugDraw>;

	constexpr DebugDrawFlags DebugDraw_None = 0;

	class DebugComponent;

	using DebugComponentHandle = Nz::ObjectHandle<DebugComponent>;

	class NDK_API DebugComponent : public Component<DebugComponent>
	{
		friend class DebugSystem;

		public:
			inline DebugComponent(DebugDrawFlags flags = DebugDraw_None);
			inline DebugComponent(const DebugComponent& debug);
			~DebugComponent() = default;

			inline void Disable(DebugDrawFlags flags);
			inline void Enable(DebugDrawFlags flags);

			inline DebugDrawFlags GetFlags() const;

			inline bool IsEnabled(DebugDrawFlags flags) const;

			inline DebugComponent& operator=(const DebugComponent& debug);

			static ComponentIndex componentIndex;

		private:
			inline const Nz::InstancedRenderableRef& GetDebugRenderable(DebugDraw option) const;
			inline DebugDrawFlags GetEnabledFlags() const;
			inline void UpdateDebugRenderable(DebugDraw option, Nz::InstancedRenderableRef renderable);
			inline void UpdateEnabledFlags(DebugDrawFlags flags);

			static constexpr std::size_t DebugModeCount = static_cast<std::size_t>(DebugDraw::Max) + 1;

			std::array<Nz::InstancedRenderableRef, DebugModeCount> m_debugRenderables;
			DebugDrawFlags m_enabledFlags;
			DebugDrawFlags m_flags;
	};
}

#include <NDK/Components/DebugComponent.inl>

#endif // NDK_COMPONENTS_DEBUGCOMPONENT_HPP
#endif // NDK_SERVER
