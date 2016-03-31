// Copyright (C) 2016 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp


#include <NDK/LuaAPI.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Lua/LuaClass.hpp>
#include <NDK/Components.hpp>
#include <NDK/World.hpp>

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

		int AddComponent(Nz::LuaInstance& lua, EntityHandle& handle, ComponentIndex index)
		{
			std::vector<int(*)(Nz::LuaInstance&, EntityHandle&)> componentAdder(BaseComponent::GetMaxComponentIndex(), AddNilComponent);
			componentAdder[GraphicsComponent::componentIndex] = &AddComponentOfType<GraphicsComponent>;
			componentAdder[NodeComponent::componentIndex] = &AddComponentOfType<NodeComponent>;

			if (index > componentAdder.size())
			{
				lua.Error("Invalid component index");
				return 0;
			}

			return componentAdder[index](lua, handle);
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

		int PushComponent(Nz::LuaInstance& lua, BaseComponent& component)
		{
			std::vector<int(*)(Nz::LuaInstance&, BaseComponent&)> componentConverter(BaseComponent::GetMaxComponentIndex(), PushNilComponent);
			componentConverter[GraphicsComponent::componentIndex] = &PushComponentOfType<GraphicsComponent>;
			componentConverter[NodeComponent::componentIndex] = &PushComponentOfType<NodeComponent>;

			ComponentIndex index = component.GetIndex();

			if (index > componentConverter.size())
			{
				lua.Error("Invalid component index");
				return 0;
			}

			return componentConverter[index](lua, component);
		}
	}

	void LuaAPI::Register_SDK(Nz::LuaInstance& instance)
	{
		Nz::LuaClass<GraphicsComponentHandle> graphicsComponent("GraphicsComponent");

		graphicsComponent.SetMethod("Attach", &GraphicsComponent::Attach, 0);

		graphicsComponent.Register(instance);

		Nz::LuaClass<Nz::Node> nodeClass("Node"); //< TODO: Move to Utility

		nodeClass.SetMethod("GetBackward", &Nz::Node::GetBackward);
		//nodeClass.SetMethod("GetChilds", &Nz::Node::GetChilds);
		nodeClass.SetMethod("GetDown", &Nz::Node::GetDown);
		nodeClass.SetMethod("GetForward", &Nz::Node::GetForward);
		nodeClass.SetMethod("GetInheritPosition", &Nz::Node::GetInheritPosition);
		nodeClass.SetMethod("GetInheritRotation", &Nz::Node::GetInheritRotation);
		nodeClass.SetMethod("GetInheritScale", &Nz::Node::GetInheritScale);
		nodeClass.SetMethod("GetInitialPosition", &Nz::Node::GetInitialPosition);
		//nodeClass.SetMethod("GetInitialRotation", &Nz::Node::GetInitialRotation);
		nodeClass.SetMethod("GetInitialScale", &Nz::Node::GetInitialScale);
		nodeClass.SetMethod("GetLeft", &Nz::Node::GetLeft);
		nodeClass.SetMethod("GetNodeType", &Nz::Node::GetNodeType);
		//nodeClass.SetMethod("GetParent", &Nz::Node::GetParent);
		nodeClass.SetMethod("GetPosition", &Nz::Node::GetPosition, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetRight", &Nz::Node::GetRight);
		//nodeClass.SetMethod("GetRotation", &Nz::Node::GetRotation, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetScale", &Nz::Node::GetScale, Nz::CoordSys_Global);
		//nodeClass.SetMethod("GetTransformMatrix", &Nz::Node::GetTransformMatrix);
		nodeClass.SetMethod("GetUp", &Nz::Node::GetUp);

		nodeClass.SetMethod("HasChilds", &Nz::Node::HasChilds);

		nodeClass.SetMethod("GetBackward", &Nz::Node::GetBackward);
		nodeClass.SetMethod("GetDown", &Nz::Node::GetDown);
		nodeClass.SetMethod("GetForward", &Nz::Node::GetForward);
		nodeClass.SetMethod("GetInheritPosition", &Nz::Node::GetInheritPosition);
		nodeClass.SetMethod("GetInheritRotation", &Nz::Node::GetInheritRotation);
		nodeClass.SetMethod("GetInheritScale", &Nz::Node::GetInheritScale);
		nodeClass.SetMethod("GetInitialPosition", &Nz::Node::GetInitialPosition);
		nodeClass.SetMethod("GetInitialRotation", &Nz::Node::GetInitialRotation);
		nodeClass.SetMethod("GetInitialScale", &Nz::Node::GetInitialScale);
		nodeClass.SetMethod("GetLeft", &Nz::Node::GetLeft);
		nodeClass.SetMethod("GetNodeType", &Nz::Node::GetNodeType);
		nodeClass.SetMethod("GetPosition", &Nz::Node::GetPosition, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetRight", &Nz::Node::GetRight);
		nodeClass.SetMethod("GetRotation", &Nz::Node::GetRotation, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetScale", &Nz::Node::GetScale, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetUp", &Nz::Node::GetUp);

		nodeClass.SetMethod("SetInitialPosition", (void(Nz::Node::*)(const Nz::Vector3f&)) &Nz::Node::SetInitialPosition);
		nodeClass.SetMethod("SetInitialRotation", (void(Nz::Node::*)(const Nz::Quaternionf&)) &Nz::Node::SetInitialRotation);

		nodeClass.SetMethod("SetPosition", (void(Nz::Node::*)(const Nz::Vector3f&, Nz::CoordSys)) &Nz::Node::SetPosition, Nz::CoordSys_Local);
		nodeClass.SetMethod("SetRotation", (void(Nz::Node::*)(const Nz::Quaternionf&, Nz::CoordSys)) &Nz::Node::SetRotation, Nz::CoordSys_Local);

		nodeClass.SetMethod("Move", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			int argIndex = 1;

			Nz::Vector3f offset = lua.Check<Nz::Vector3f>(&argIndex);
			Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
			node.Move(offset, coordSys);

			return 0;
		});

		nodeClass.SetMethod("Rotate", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			int argIndex = 1;

			Nz::Quaternionf rotation = lua.Check<Nz::Quaternionf>(&argIndex);
			Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
			node.Rotate(rotation, coordSys);

			return 0;
		});

		nodeClass.SetMethod("Scale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);

			int argIndex = 1;
			switch (argCount)
			{
				case 1:
				{
					if (lua.IsOfType(argIndex, Nz::LuaType_Number))
						node.Scale(lua.Check<float>(&argIndex));
					else
						node.Scale(lua.Check<Nz::Vector3f>(&argIndex));

					return 0;
				}

				case 3:
					node.Scale(lua.Check<Nz::Vector3f>(&argIndex));
					return 0;
			}

			lua.Error("No matching overload for method Scale");
			return 0;
		});

		nodeClass.SetMethod("SetScale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);

			int argIndex = 1;
			switch (argCount)
			{
				case 1:
				case 2:
				{
					if (lua.IsOfType(argIndex, Nz::LuaType_Number))
					{
						float scale = lua.Check<float>(&argIndex);
						Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
						node.SetScale(scale, coordSys);
					}
					else
						node.SetScale(lua.Check<Nz::Vector3f>(&argIndex));

					return 0;
				}

				case 3:
				case 4:
				{
					Nz::Vector3f scale = lua.Check<Nz::Vector3f>(&argIndex);
					Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);

					node.SetScale(scale, coordSys);
					return 0;
				}
			}

			lua.Error("No matching overload for method SetScale");
			return 0;
		});

		nodeClass.SetMethod("SetInitialScale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);

			int argIndex = 1;
			switch (argCount)
			{
				case 1:
				{
					if (lua.IsOfType(argIndex, Nz::LuaType_Number))
						node.SetInitialScale(lua.Check<float>(&argIndex));
					else
						node.SetInitialScale(lua.Check<Nz::Vector2f>(&argIndex));

					return 0;
				}

				case 2:
				case 3:
					node.SetInitialScale(lua.Check<Nz::Vector3f>(&argIndex));
					return 0;
			}

			lua.Error("No matching overload for method SetInitialScale");
			return 0;
		});

		nodeClass.Register(instance);

		Nz::LuaClass<NodeComponentHandle> nodeComponent("NodeComponent");
		nodeComponent.Inherit<Nz::Node>(nodeClass, [] (NodeComponentHandle* handle) -> Nz::Node*
		{
			return handle->GetObject();
		});

		nodeComponent.Register(instance);

		Nz::LuaClass<EntityHandle> entityClass("Entity");

		entityClass.SetMethod("Enable", &Entity::Enable);
		entityClass.SetMethod("GetId", &Entity::GetId);
		entityClass.SetMethod("GetWorld", &Entity::GetWorld);
		entityClass.SetMethod("Kill", &Entity::Kill);
		entityClass.SetMethod("IsEnabled", &Entity::IsEnabled);
		entityClass.SetMethod("IsValid", &Entity::IsValid);
		entityClass.SetMethod("RemoveComponent", (void(Entity::*)(ComponentIndex)) &Entity::RemoveComponent);
		entityClass.SetMethod("RemoveAllComponents", &Entity::RemoveAllComponents);
		entityClass.SetMethod("__tostring", &EntityHandle::ToString);

		entityClass.SetMethod("AddComponent", [] (Nz::LuaInstance& lua, EntityHandle& handle) -> int
		{
			int index = 1;
			ComponentIndex componentIndex = lua.Check<ComponentIndex>(&index);

			return AddComponent(lua, handle, componentIndex);
		});

		entityClass.SetMethod("GetComponent", [] (Nz::LuaInstance& lua, EntityHandle& handle) -> int
		{
			int index = 1;
			ComponentIndex componentIndex = lua.Check<ComponentIndex>(&index);

			if (!handle->HasComponent(componentIndex))
			{
				lua.PushNil();
				return 1;
			}

			return PushComponent(lua, handle->GetComponent(componentIndex));
		});

		entityClass.Register(instance);

		Nz::LuaClass<WorldHandle> worldClass("World");

		worldClass.SetMethod("CreateEntity", &World::CreateEntity);
		worldClass.SetMethod("CreateEntities", &World::CreateEntities);
		worldClass.SetMethod("Clear", &World::Clear);

		worldClass.Register(instance);

		instance.PushTable(0, 2);
		{
			instance.PushInteger(GraphicsComponent::componentIndex);
			instance.SetField("Graphics");

			instance.PushInteger(NodeComponent::componentIndex);
			instance.SetField("Node");
		}
		instance.SetGlobal("ComponentType");
	}
}