// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_HPP
#define NDK_LUABINDING_HPP

#include <Nazara/Core.hpp>
#include <Nazara/Lua.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/Entity.hpp>
#include <NDK/Systems.hpp>
#include <NDK/World.hpp>

#ifndef NDK_SERVER
#include <Nazara/Audio.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <NDK/Console.hpp>
#endif

namespace Ndk
{
	class NDK_API LuaBinding
	{
		public:
			LuaBinding();
			~LuaBinding() = default;

			template<typename T> void BindComponent(const Nz::String& name);

			void RegisterClasses(Nz::LuaInstance& instance);

			// Core
			Nz::LuaClass<Nz::Clock> clockClass;
			Nz::LuaClass<Nz::Directory> directoryClass;
			Nz::LuaClass<Nz::File> fileClass;
			Nz::LuaClass<Nz::Stream> streamClass;

			// Math
			Nz::LuaClass<Nz::EulerAnglesd> eulerAnglesClass;
			Nz::LuaClass<Nz::Matrix4d> matrix4dClass;
			Nz::LuaClass<Nz::Quaterniond> quaternionClass;
			Nz::LuaClass<Nz::Rectd> rectClass;
			Nz::LuaClass<Nz::Vector2d> vector2dClass;
			Nz::LuaClass<Nz::Vector3d> vector3dClass;

			// Network
			Nz::LuaClass<Nz::AbstractSocket> abstractSocketClass;
			Nz::LuaClass<Nz::IpAddress> ipAddressClass;

			// Utility
			Nz::LuaClass<Nz::AbstractImage*> abstractImage;
			Nz::LuaClass<Nz::FontRef> fontClass;
			Nz::LuaClass<Nz::Node> nodeClass;

			// SDK
			Nz::LuaClass<Application*> application;
			Nz::LuaClass<EntityHandle> entityClass;
			Nz::LuaClass<NodeComponentHandle> nodeComponent;
			Nz::LuaClass<VelocityComponentHandle> velocityComponent;
			Nz::LuaClass<WorldHandle> worldClass;

			#ifndef NDK_SERVER
			// Audio
			Nz::LuaClass<Nz::Music> musicClass;
			Nz::LuaClass<Nz::Sound> soundClass;
			Nz::LuaClass<Nz::SoundBufferRef> soundBuffer;
			Nz::LuaClass<Nz::SoundEmitter> soundEmitter;

			// Graphics
			Nz::LuaClass<Nz::InstancedRenderableRef> instancedRenderable;
			Nz::LuaClass<Nz::ModelRef> modelClass;

			// SDK
			Nz::LuaClass<ConsoleHandle> consoleClass;
			Nz::LuaClass<GraphicsComponentHandle> graphicsComponent;
			#endif

		private:
			void BindCore();
			void BindMath();
			void BindNetwork();
			void BindSDK();
			void BindUtility();

			void RegisterCore(Nz::LuaInstance& instance);
			void RegisterMath(Nz::LuaInstance& instance);
			void RegisterNetwork(Nz::LuaInstance& instance);
			void RegisterSDK(Nz::LuaInstance& instance);
			void RegisterUtility(Nz::LuaInstance& instance);

			#ifndef NDK_SERVER
			void BindAudio();
			void BindGraphics();
			void BindRenderer();

			void RegisterAudio(Nz::LuaInstance& instance);
			void RegisterGraphics(Nz::LuaInstance& instance);
			void RegisterRenderer(Nz::LuaInstance& instance);
			#endif


			using AddComponentFunc = int(*)(Nz::LuaInstance&, EntityHandle&);
			using GetComponentFunc = int(*)(Nz::LuaInstance&, BaseComponent&);

			struct ComponentBinding
			{
				AddComponentFunc adder;
				ComponentIndex index;
				GetComponentFunc getter;
				Nz::String name;
			};

			ComponentBinding* QueryComponentIndex(Nz::LuaInstance& lua, int argIndex = 1);

			std::vector<ComponentBinding> m_componentBinding;
			std::unordered_map<Nz::String, ComponentIndex> m_componentBindingByName;
	};

	template<typename T>
	int AddComponentOfType(Nz::LuaInstance& lua, EntityHandle& handle);

	template<typename T>
	int PushComponentOfType(Nz::LuaInstance& lua, BaseComponent& component);
}

#include <NDK/LuaBinding.inl>

#endif // NDK_LUABINDING_HPP
