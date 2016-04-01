// Copyright (C) 2016 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	namespace
	{
		int AddNilComponent(Nz::LuaInstance& lua, EntityHandle&)
		{
			lua.PushNil();
			return 1;
		}

		template<typename T>
		int AddComponentOfType(Nz::LuaInstance& lua, EntityHandle& handle)
		{
			static_assert(std::is_base_of<BaseComponent, T>::value, "ComponentType must inherit BaseComponent");

			T& component = handle->AddComponent<T>();
			lua.Push(component.CreateHandle());
			return 1;
		}

		int PushNilComponent(Nz::LuaInstance& lua, BaseComponent&)
		{
			lua.PushNil();
			return 1;
		}

		template<typename T>
		int PushComponentOfType(Nz::LuaInstance& lua, BaseComponent& component)
		{
			static_assert(std::is_base_of<BaseComponent, T>::value, "ComponentType must inherit BaseComponent");

			T& rightComponent = static_cast<T&>(component);
			lua.Push(rightComponent.CreateHandle());
			return 1;
		}
	}

	void LuaBinding::BindSDK()
	{

		/*********************************** Ndk::Console **********************************/
		consoleClass.Inherit<Nz::Node>(nodeClass, [] (ConsoleHandle* handle) -> Nz::Node*
		{
			return handle->GetObject();
		});

		consoleClass.SetMethod("AddLine", &Console::AddLine, Nz::Color::White);
		consoleClass.SetMethod("Clear", &Console::Clear);
		consoleClass.SetMethod("GetCharacterSize", &Console::GetCharacterSize);
		consoleClass.SetMethod("GetHistory", &Console::GetHistory);
		consoleClass.SetMethod("GetHistoryBackground", &Console::GetHistoryBackground);
		consoleClass.SetMethod("GetInput", &Console::GetInput);
		consoleClass.SetMethod("GetInputBackground", &Console::GetInputBackground);
		consoleClass.SetMethod("GetSize", &Console::GetSize);
		//consoleClass.SetMethod("GetTextFont", &Console::GetTextFont);

		consoleClass.SetMethod("IsVisible", &Console::IsVisible);

		consoleClass.SetMethod("SendCharacter", &Console::SendCharacter);
		//consoleClass.SetMethod("SendEvent", &Console::SendEvent);

		consoleClass.SetMethod("SetCharacterSize", &Console::SetCharacterSize);
		consoleClass.SetMethod("SetSize", &Console::SetSize);
		//consoleClass.SetMethod("SetTextFont", &Console::SetTextFont);
		
		consoleClass.SetMethod("Show", &Console::Show, true);

		/*********************************** Ndk::Entity **********************************/
		entityClass.SetMethod("Enable", &Entity::Enable);
		entityClass.SetMethod("GetId", &Entity::GetId);
		entityClass.SetMethod("GetWorld", &Entity::GetWorld);
		entityClass.SetMethod("Kill", &Entity::Kill);
		entityClass.SetMethod("IsEnabled", &Entity::IsEnabled);
		entityClass.SetMethod("IsValid", &Entity::IsValid);
		entityClass.SetMethod("RemoveComponent", (void(Entity::*)(ComponentIndex)) &Entity::RemoveComponent);
		entityClass.SetMethod("RemoveAllComponents", &Entity::RemoveAllComponents);
		entityClass.SetMethod("__tostring", &EntityHandle::ToString);

		entityClass.SetMethod("AddComponent", [this] (Nz::LuaInstance& lua, EntityHandle& handle) -> int
		{
			int index = 1;
			ComponentIndex componentIndex = lua.Check<ComponentIndex>(&index);

			if (componentIndex > m_componentBinding.size())
			{
				lua.Error("Invalid component index");
				return 0;
			}

			ComponentBinding& binding = m_componentBinding[componentIndex];
			if (!binding.valid)
			{
				lua.Error("This component is not available to the LuaAPI");
				return 0;
			}

			return binding.adder(lua, handle);
		});

		entityClass.SetMethod("GetComponent", [this] (Nz::LuaInstance& lua, EntityHandle& handle) -> int
		{
			int index = 1;
			ComponentIndex componentIndex = lua.Check<ComponentIndex>(&index);

			if (!handle->HasComponent(componentIndex))
			{
				lua.PushNil();
				return 1;
			}

			if (componentIndex > m_componentBinding.size())
			{
				lua.Error("Invalid component index");
				return 0;
			}

			ComponentBinding& binding = m_componentBinding[componentIndex];
			if (!binding.valid)
			{
				lua.Error("This component is not available to the LuaAPI");
				return 0;
			}

			return binding.getter(lua, handle->GetComponent(componentIndex));
		});

		/*********************************** Ndk::NodeComponent **********************************/
		nodeComponent.Inherit<Nz::Node>(nodeClass, [] (NodeComponentHandle* handle) -> Nz::Node*
		{
			return handle->GetObject();
		});

		/*********************************** Ndk::VelocityComponent **********************************/
		velocityComponent.SetGetter([] (Nz::LuaInstance& lua, VelocityComponentHandle& instance)
		{
			std::size_t length;
			const char* member = lua.CheckString(1, &length);

			if (std::strcmp(member, "Linear") == 0)
			{
				lua.Push(instance->linearVelocity);
				return true;
			}

			return false;
		});

		velocityComponent.SetSetter([] (Nz::LuaInstance& lua, VelocityComponentHandle& instance)
		{
			std::size_t length;
			const char* member = lua.CheckString(1, &length);

			int argIndex = 2;
			if (std::strcmp(member, "Linear") == 0)
			{
				instance->linearVelocity = lua.Check<Nz::Vector3f>(&argIndex);
				return true;
			}

			return false;
		});

		/*********************************** Ndk::World **********************************/
		worldClass.SetMethod("CreateEntity", &World::CreateEntity);
		worldClass.SetMethod("CreateEntities", &World::CreateEntities);
		worldClass.SetMethod("Clear", &World::Clear);


		#ifndef NDK_SERVER
		/*********************************** Ndk::GraphicsComponent **********************************/
		graphicsComponent.SetMethod("Attach", &GraphicsComponent::Attach, 0);
		#endif


		// Components functions
		m_componentBinding.resize(BaseComponent::GetMaxComponentIndex() + 1);

		EnableComponentBinding<NodeComponent>();
		EnableComponentBinding<VelocityComponent>();

		#ifndef NDK_SERVER
		EnableComponentBinding<GraphicsComponent>();
		#endif
	}

	template<typename T>
	void LuaBinding::EnableComponentBinding()
	{
		ComponentBinding binding;
		binding.adder = &AddComponentOfType<T>;
		binding.getter = &PushComponentOfType<T>;
		binding.valid = true;

		NazaraAssert(T::componentIndex < m_componentBinding.size(), "Component index is over component binding size");

		m_componentBinding[T::componentIndex] = std::move(binding);
	}

	void LuaBinding::RegisterSDK(Nz::LuaInstance& instance)
	{
		// Classes
		entityClass.Register(instance);
		nodeComponent.Register(instance);
		velocityComponent.Register(instance);
		worldClass.Register(instance);

		#ifndef NDK_SERVER
		consoleClass.Register(instance);
		graphicsComponent.Register(instance);
		#endif

		// Enums

		// ComponentType (fake enumeration to expose component indexes)
		instance.PushTable();
		{
			#ifndef NDK_SERVER
			instance.PushInteger(GraphicsComponent::componentIndex);
			instance.SetField("Graphics");
			#endif

			instance.PushInteger(NodeComponent::componentIndex);
			instance.SetField("Node");

			instance.PushInteger(VelocityComponent::componentIndex);
			instance.SetField("Velocity");
		}
		instance.SetGlobal("ComponentType");
	}
}