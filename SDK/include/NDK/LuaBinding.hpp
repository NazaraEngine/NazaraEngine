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
			Nz::LuaClass<Nz::Clock> clock;
			Nz::LuaClass<Nz::Directory> directory;
			Nz::LuaClass<Nz::File> file;
			Nz::LuaClass<Nz::Stream> stream;

			// Math
			Nz::LuaClass<Nz::EulerAnglesd> eulerAngles;
			Nz::LuaClass<Nz::Matrix4d> matrix4d;
			Nz::LuaClass<Nz::Quaterniond> quaternion;
			Nz::LuaClass<Nz::Rectd> rect;
			Nz::LuaClass<Nz::Vector2d> vector2d;
			Nz::LuaClass<Nz::Vector3d> vector3d;

			// Network
			Nz::LuaClass<Nz::AbstractSocket> abstractSocket;
			Nz::LuaClass<Nz::IpAddress> ipAddress;

			// Utility
			Nz::LuaClass<Nz::AbstractImageRef> abstractImage;
			Nz::LuaClass<Nz::FontRef> font;
			Nz::LuaClass<Nz::Node> node;

			// SDK
			Nz::LuaClass<Application*> application;
			Nz::LuaClass<EntityHandle> entity;
			Nz::LuaClass<NodeComponentHandle> nodeComponent;
			Nz::LuaClass<VelocityComponentHandle> velocityComponent;
			Nz::LuaClass<WorldHandle> world;

			#ifndef NDK_SERVER
			// Audio
			Nz::LuaClass<Nz::Music> music;
			Nz::LuaClass<Nz::Sound> sound;
			Nz::LuaClass<Nz::SoundBufferRef> soundBuffer;
			Nz::LuaClass<Nz::SoundEmitter> soundEmitter;

			// Graphics
			Nz::LuaClass<Nz::InstancedRenderableRef> instancedRenderable;
			Nz::LuaClass<Nz::ModelRef> model;
			Nz::LuaClass<Nz::SpriteRef> sprite;
			Nz::LuaClass<Nz::SpriteLibrary> spriteLibrary;

			// Renderer
			Nz::LuaClass<Nz::TextureRef> texture;

			// SDK
			Nz::LuaClass<ConsoleHandle> console;
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
