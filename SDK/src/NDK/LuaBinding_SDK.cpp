// Copyright (C) 2016 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds SDK module to Lua
	*/

	void LuaBinding::BindSDK()
	{
		/*********************************** Ndk::Application **********************************/

		#ifndef NDK_SERVER
		//application.SetMethod("AddWindow", &Application::AddWindow);

		application.BindMethod("EnableConsole", &Application::EnableConsole);
		application.BindMethod("EnableFPSCounter", &Application::EnableFPSCounter);

		application.BindMethod("IsConsoleEnabled", &Application::IsConsoleEnabled);
		application.BindMethod("IsFPSCounterEnabled", &Application::IsFPSCounterEnabled);
		#endif

		application.BindMethod("AddWorld", [] (Nz::LuaInstance& instance, Application* application) -> int
		{
			instance.Push(application->AddWorld().CreateHandle());
			return 1;
		});

		application.BindMethod("GetUpdateTime", &Application::GetUpdateTime);
		application.BindMethod("Quit", &Application::Quit);

		/*********************************** Ndk::Console **********************************/
		#ifndef NDK_SERVER
		consoleClass.Inherit<Nz::Node>(nodeClass, [] (ConsoleHandle* handle) -> Nz::Node*
		{
			return handle->GetObject();
		});

		consoleClass.BindMethod("AddLine", &Console::AddLine, Nz::Color::White);
		consoleClass.BindMethod("Clear", &Console::Clear);
		consoleClass.BindMethod("GetCharacterSize", &Console::GetCharacterSize);
		consoleClass.BindMethod("GetHistory", &Console::GetHistory);
		consoleClass.BindMethod("GetHistoryBackground", &Console::GetHistoryBackground);
		consoleClass.BindMethod("GetInput", &Console::GetInput);
		consoleClass.BindMethod("GetInputBackground", &Console::GetInputBackground);
		consoleClass.BindMethod("GetSize", &Console::GetSize);
		consoleClass.BindMethod("GetTextFont", &Console::GetTextFont);

		consoleClass.BindMethod("IsVisible", &Console::IsVisible);

		consoleClass.BindMethod("SendCharacter", &Console::SendCharacter);
		//consoleClass.SetMethod("SendEvent", &Console::SendEvent);

		consoleClass.BindMethod("SetCharacterSize", &Console::SetCharacterSize);
		consoleClass.BindMethod("SetSize", &Console::SetSize);
		consoleClass.BindMethod("SetTextFont", &Console::SetTextFont);
		
		consoleClass.BindMethod("Show", &Console::Show, true);
		#endif

		/*********************************** Ndk::Entity **********************************/
		entityClass.BindMethod("Enable", &Entity::Enable, true);
		entityClass.BindMethod("GetId", &Entity::GetId);
		entityClass.BindMethod("GetWorld", &Entity::GetWorld);
		entityClass.BindMethod("Kill", &Entity::Kill);
		entityClass.BindMethod("IsEnabled", &Entity::IsEnabled);
		entityClass.BindMethod("IsValid", &Entity::IsValid);
		entityClass.BindMethod("RemoveAllComponents", &Entity::RemoveAllComponents);
		entityClass.BindMethod("__tostring", &EntityHandle::ToString);

		entityClass.BindMethod("AddComponent", [this] (Nz::LuaInstance& instance, EntityHandle& handle) -> int
		{
			ComponentBinding* binding = QueryComponentIndex(instance);

			return binding->adder(instance, handle);
		});

		entityClass.BindMethod("GetComponent", [this] (Nz::LuaInstance& instance, EntityHandle& handle) -> int
		{
			ComponentBinding* binding = QueryComponentIndex(instance);

			return binding->getter(instance, handle->GetComponent(binding->index));
		});

		entityClass.BindMethod("RemoveComponent", [this] (Nz::LuaInstance& instance, EntityHandle& handle) -> int
		{
			ComponentBinding* binding = QueryComponentIndex(instance);

			handle->RemoveComponent(binding->index);
			return 0;
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
		worldClass.BindMethod("CreateEntity", &World::CreateEntity);
		worldClass.BindMethod("CreateEntities", &World::CreateEntities);
		worldClass.BindMethod("Clear", &World::Clear);


		#ifndef NDK_SERVER
		/*********************************** Ndk::GraphicsComponent **********************************/
		graphicsComponent.BindMethod("Attach", (void(Ndk::GraphicsComponent::*)(Nz::InstancedRenderableRef, int)) &GraphicsComponent::Attach, 0);
		#endif


		// Components functions
		m_componentBinding.resize(BaseComponent::GetMaxComponentIndex());

		BindComponent<NodeComponent>("Node");
		BindComponent<VelocityComponent>("Velocity");

		#ifndef NDK_SERVER
		BindComponent<GraphicsComponent>("Graphics");
		#endif
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the SDK classes
	*/

	void LuaBinding::RegisterSDK(Nz::LuaInstance& instance)
	{
		// Classes
		application.Register(instance);
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
		instance.PushTable(0, m_componentBinding.size());
		{
			for (const ComponentBinding& entry : m_componentBinding)
			{
				if (entry.name.IsEmpty())
					continue;

				instance.PushField(entry.name, entry.index);
			}
		}
		instance.SetGlobal("ComponentType");
	}

	/*!
	* \brief Gets the index of the component
	* \return A pointer to the binding linked to a component
	*
	* \param instance Lua instance that will interact with the component
	* \param argIndex Index of the component
	*/

	LuaBinding::ComponentBinding* LuaBinding::QueryComponentIndex(Nz::LuaInstance& instance, int argIndex)
	{
		switch (instance.GetType(argIndex))
		{
			case Nz::LuaType_Number:
			{
				ComponentIndex componentIndex = instance.Check<ComponentIndex>(&argIndex);
				if (componentIndex > m_componentBinding.size())
				{
					instance.Error("Invalid component index");
					return nullptr;
				}

				ComponentBinding& binding = m_componentBinding[componentIndex];
				if (binding.name.IsEmpty())
				{
					instance.Error("Invalid component index");
					return nullptr;
				}

				return &binding;
			}

			case Nz::LuaType_String:
			{
				const char* key = instance.CheckString(argIndex);
				auto it = m_componentBindingByName.find(key);
				if (it == m_componentBindingByName.end())
				{
					instance.Error("Invalid component name");
					return nullptr;
				}

				return &m_componentBinding[it->second];
			}

			default:
				break;
		}

		instance.Error("Invalid component index at #" + Nz::String::Number(argIndex));
		return nullptr;
	}
}
