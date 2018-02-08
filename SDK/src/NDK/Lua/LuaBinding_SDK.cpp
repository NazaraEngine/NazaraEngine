// Copyright (C) 2017 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Lua/LuaBinding_SDK.hpp>
#include <NDK/LuaAPI.hpp>
#include <NDK/Lua/LuaBinding.hpp>
#include <NDK/Lua/LuaBinding_Utility.hpp>

#ifndef NDK_SERVER
#include <NDK/Lua/LuaBinding_Graphics.hpp>
#endif

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindSDK(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_SDK>(binding);
	}

	LuaBinding_SDK::LuaBinding_SDK(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		#ifndef NDK_SERVER
		LuaBinding_Graphics& graphics = static_cast<LuaBinding_Graphics&>(*m_binding.graphics);
		#endif

		LuaBinding_Utility& utility = static_cast<LuaBinding_Utility&>(*m_binding.utility);

		/*********************************** Ndk::Application **********************************/
		application.Reset("Application");
		{
			#ifndef NDK_SERVER
			//application.SetMethod("AddWindow", &Application::AddWindow);

			application.BindMethod("EnableConsole",    &Application::EnableConsole);
			application.BindMethod("EnableFPSCounter", &Application::EnableFPSCounter);

			application.BindMethod("IsConsoleEnabled",    &Application::IsConsoleEnabled);
			application.BindMethod("IsFPSCounterEnabled", &Application::IsFPSCounterEnabled);
			#endif

			application.BindMethod("AddWorld", [] (Nz::LuaState& lua, Application* instance, std::size_t /*argumentCount*/) -> int
			{
				lua.Push(instance->AddWorld().CreateHandle());
				return 1;
			});

			application.BindMethod("GetUpdateTime", &Application::GetUpdateTime);
			application.BindMethod("Quit", &Application::Quit);
		}

		/*********************************** Ndk::Console **********************************/
		#ifndef NDK_SERVER
		console.Reset("Console");
		{
			console.Inherit<Nz::Node>(utility.node, [] (ConsoleHandle* handle) -> Nz::Node*
			{
				return handle->GetObject();
			});

			console.BindMethod("AddLine", &Console::AddLine, Nz::Color::White);
			console.BindMethod("Clear", &Console::Clear);
			console.BindMethod("GetCharacterSize", &Console::GetCharacterSize);
			console.BindMethod("GetHistory", &Console::GetHistory);
			console.BindMethod("GetHistoryBackground", &Console::GetHistoryBackground);
			console.BindMethod("GetInput", &Console::GetInput);
			console.BindMethod("GetInputBackground", &Console::GetInputBackground);
			console.BindMethod("GetSize", &Console::GetSize);
			console.BindMethod("GetTextFont", &Console::GetTextFont);

			console.BindMethod("IsValidHandle", &ConsoleHandle::IsValid);
			console.BindMethod("IsVisible", &Console::IsVisible);

			console.BindMethod("SendCharacter", &Console::SendCharacter);
			//consoleClass.SetMethod("SendEvent", &Console::SendEvent);

			console.BindMethod("SetCharacterSize", &Console::SetCharacterSize);
			console.BindMethod("SetSize", &Console::SetSize);
			console.BindMethod("SetTextFont", &Console::SetTextFont);

			console.BindMethod("Show", &Console::Show, true);
		}
		#endif

		/*********************************** Ndk::Entity **********************************/
		entity.Reset("Entity");
		{
			entity.BindMethod("Enable", &Entity::Enable, true);
			entity.BindMethod("GetId", &Entity::GetId);
			entity.BindMethod("GetWorld", &Entity::GetWorld);
			entity.BindMethod("Kill", &Entity::Kill);
			entity.BindMethod("IsEnabled", &Entity::IsEnabled);
			entity.BindMethod("IsValid", &Entity::IsValid);
			entity.BindMethod("IsValidHandle", &EntityHandle::IsValid);
			entity.BindMethod("RemoveAllComponents", &Entity::RemoveAllComponents);
			entity.BindMethod("__tostring", &EntityHandle::ToString);

			entity.BindMethod("AddComponent", [this] (Nz::LuaState& state, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
			{
				LuaBinding::ComponentBinding* bindingComponent = m_binding.QueryComponentIndex(state);

				return bindingComponent->adder(state, handle);
			});

			entity.BindMethod("HasComponent", [this](Nz::LuaState& state, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
			{
				LuaBinding::ComponentBinding* bindingComponent = m_binding.QueryComponentIndex(state);

				state.PushBoolean(handle->HasComponent(bindingComponent->index));
				return 1;
			});

			entity.BindMethod("GetComponent", [this] (Nz::LuaState& state, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
			{
				LuaBinding::ComponentBinding* bindingComponent = m_binding.QueryComponentIndex(state);

				return bindingComponent->getter(state, handle->GetComponent(bindingComponent->index));
			});

			entity.BindMethod("RemoveComponent", [this] (Nz::LuaState& state, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
			{
				LuaBinding::ComponentBinding* bindingComponent = m_binding.QueryComponentIndex(state);

				handle->RemoveComponent(bindingComponent->index);
				return 0;
			});
		}

		/*********************************** Ndk::NodeComponent **********************************/
		nodeComponent.Reset("NodeComponent");
		{
			nodeComponent.BindMethod("IsValidHandle", &NodeComponentHandle::IsValid);

			nodeComponent.Inherit<Nz::Node>(utility.node, [] (NodeComponentHandle* handle) -> Nz::Node*
			{
				return handle->GetObject();
			});
		}

		/*********************************** Ndk::VelocityComponent **********************************/
		velocityComponent.Reset("VelocityComponent");
		{
			velocityComponent.BindMethod("IsValidHandle", &VelocityComponentHandle::IsValid);

			velocityComponent.SetGetter([] (Nz::LuaState& lua, VelocityComponentHandle& instance)
			{
				std::size_t length;
				const char* member = lua.CheckString(2, &length);

				if (std::strcmp(member, "Linear") == 0)
				{
					lua.Push(instance->linearVelocity);
					return true;
				}

				return false;
			});

			velocityComponent.SetSetter([] (Nz::LuaState& lua, VelocityComponentHandle& instance)
			{
				std::size_t length;
				const char* member = lua.CheckString(2, &length);

				int argIndex = 3;
				if (std::strcmp(member, "Linear") == 0)
				{
					instance->linearVelocity = lua.Check<Nz::Vector3f>(&argIndex);
					return true;
				}

				return false;
			});
		}

		/*********************************** Ndk::World **********************************/
		world.Reset("World");
		{
			world.BindMethod("CreateEntity", &World::CreateEntity);
			world.BindMethod("CreateEntities", &World::CreateEntities);
			world.BindMethod("Clear", &World::Clear);
			world.BindMethod("DisableProfiler", &World::DisableProfiler);
			world.BindMethod("EnableProfiler", &World::EnableProfiler);
			world.BindMethod("IsProfilerEnabled", &World::IsProfilerEnabled);
			world.BindMethod("ResetProfiler", &World::ResetProfiler);

			world.BindMethod("IsValidHandle", &WorldHandle::IsValid);
		}

		#ifndef NDK_SERVER
		/*********************************** Ndk::CameraComponent **********************************/
		cameraComponent.Reset("CameraComponent");
		{
			cameraComponent.Inherit<Nz::AbstractViewer>(graphics.abstractViewer, [] (CameraComponentHandle* handle) -> Nz::AbstractViewer*
			{
				return handle->GetObject();
			});

			cameraComponent.BindMethod("GetFOV", &CameraComponent::GetFOV);
			cameraComponent.BindMethod("GetLayer", &CameraComponent::GetLayer);

			cameraComponent.BindMethod("IsValidHandle", &CameraComponentHandle::IsValid);

			cameraComponent.BindMethod("SetFOV", &CameraComponent::SetFOV);
			cameraComponent.BindMethod("SetLayer", &CameraComponent::SetLayer);
			cameraComponent.BindMethod("SetProjectionType", &CameraComponent::SetProjectionType);
			cameraComponent.BindMethod("SetSize", (void(CameraComponent::*)(const Nz::Vector2f&)) &CameraComponent::SetSize);
			//cameraComponent.BindMethod("SetTarget", &CameraComponent::SetTarget);
			cameraComponent.BindMethod("SetTargetRegion", &CameraComponent::SetTargetRegion);
			cameraComponent.BindMethod("SetViewport", &CameraComponent::SetViewport);
			cameraComponent.BindMethod("SetZFar", &CameraComponent::SetZFar);
			cameraComponent.BindMethod("SetZNear", &CameraComponent::SetZNear);
		}

		/*********************************** Ndk::GraphicsComponent **********************************/
		graphicsComponent.Reset("GraphicsComponent");
		{
			graphicsComponent.BindMethod("Attach", [] (Nz::LuaState& lua, Ndk::GraphicsComponent* instance, std::size_t argumentCount) -> int
			{
				/*
				void Attach(Nz::InstancedRenderableRef renderable, int renderOrder = 0);
				void Attach(Nz::InstancedRenderableRef renderable, const Nz::Matrix4f& localMatrix, int renderOrder = 0);
				*/

				std::size_t argCount = std::min<std::size_t>(argumentCount, 3U);

				switch (argCount)
				{
					case 1:
					{
						int argIndex = 2;
						instance->Attach(lua.Check<Nz::InstancedRenderableRef>(&argIndex));
						return 0;
					}

					case 2:
					{
						int argIndex = 2;
						Nz::InstancedRenderableRef renderable = lua.Check<Nz::InstancedRenderableRef>(&argIndex);

						if (lua.IsOfType(argIndex, Nz::LuaType_Number))
						{
							int renderOrder = lua.Check<int>(&argIndex);

							instance->Attach(renderable, renderOrder);
						}
						else if (lua.IsOfType(argIndex, "Matrix4"))
						{
							Nz::Matrix4f localMatrix = lua.Check<Nz::Matrix4f>(&argIndex);

							instance->Attach(renderable, localMatrix);
						}
						else
							break;

						return 0;
					}

					case 3:
					{
						int argIndex = 2;
						Nz::InstancedRenderableRef renderable = lua.Check<Nz::InstancedRenderableRef>(&argIndex);
						Nz::Matrix4f localMatrix = lua.Check<Nz::Matrix4f>(&argIndex);
						int renderOrder = lua.Check<int>(&argIndex);

						instance->Attach(renderable, localMatrix, renderOrder);
						return 0;
					}
				}

				lua.Error("No matching overload for method GetMemoryUsage");
				return 0;
			});

			graphicsComponent.BindMethod("IsValidHandle", &GraphicsComponentHandle::IsValid);
		}
		#endif

		// Components functions
		m_binding.BindComponent<NodeComponent>("Node");
		m_binding.BindComponent<VelocityComponent>("Velocity");

		#ifndef NDK_SERVER
		m_binding.BindComponent<CameraComponent>("Camera");
		m_binding.BindComponent<GraphicsComponent>("Graphics");
		#endif
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the SDK classes
	*/
	void LuaBinding_SDK::Register(Nz::LuaState& state)
	{
		// Classes
		application.Register(state);
		entity.Register(state);
		nodeComponent.Register(state);
		velocityComponent.Register(state);
		world.Register(state);

		#ifndef NDK_SERVER
		cameraComponent.Register(state);
		console.Register(state);
		graphicsComponent.Register(state);
		#endif

		// Enums
	}

	/*!
	* \brief Gets the index of the component
	* \return A pointer to the binding linked to a component
	*
	* \param instance Lua instance that will interact with the component
	* \param argIndex Index of the component
	*/
	LuaBinding::ComponentBinding* LuaBinding::QueryComponentIndex(Nz::LuaState& state, int argIndex)
	{
		switch (state.GetType(argIndex))
		{
			case Nz::LuaType_Number:
			{
				ComponentIndex componentIndex = state.Check<ComponentIndex>(&argIndex);
				if (componentIndex > m_componentBinding.size())
				{
					state.Error("Invalid component index");
					return nullptr;
				}

				ComponentBinding& binding = m_componentBinding[componentIndex];
				if (binding.name.IsEmpty())
				{
					state.Error("Invalid component index");
					return nullptr;
				}

				return &binding;
			}

			case Nz::LuaType_String:
			{
				const char* key = state.CheckString(argIndex);
				auto it = m_componentBindingByName.find(key);
				if (it == m_componentBindingByName.end())
				{
					state.Error("Invalid component name");
					return nullptr;
				}

				return &m_componentBinding[it->second];
			}

			default:
				break;
		}

		state.Error("Invalid component index at #" + Nz::String::Number(argIndex));
		return nullptr;
	}
}
