// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

namespace Ndk
{
	namespace Detail
	{
		template<bool HasDefaultConstructor>
		struct AddComponentIf;

		template<>
		struct AddComponentIf<true>
		{
			template<typename T>
			static int AddComponent(Nz::LuaState& lua, EntityHandle& handle)
			{
				T& component = handle->AddComponent<T>();
				lua.Push(component.CreateHandle());
				return 1;
			}
		};

		template<>
		struct AddComponentIf<false>
		{
			template<typename T>
			static int AddComponent(Nz::LuaState& lua, EntityHandle& /*handle*/)
			{
				lua.Error("Component has no default constructor and cannot be created from Lua yet");
				return 0;
			}
		};
	}

	/*!
	* \brief Binds a component to a name
	*
	* \param name Name used to retrieve the component
	*
	* \remark Produces a NazaraAssert if name is empty
	*/
	template<typename T>
	void LuaBinding::BindComponent(const Nz::String& name)
	{
		NazaraAssert(!name.IsEmpty(), "Component name cannot be empty");

		static_assert(std::is_base_of<BaseComponent, T>::value, "ComponentType must inherit BaseComponent");

		ComponentBinding binding;
		binding.adder = &Detail::AddComponentIf<std::is_default_constructible<T>::value>::template AddComponent<T>;
		binding.getter = &PushComponentOfType<T>;
		binding.index = T::componentIndex;
		binding.name = name;

		if (m_componentBinding.size() <= T::componentIndex)
			m_componentBinding.resize(T::componentIndex + 1);

		m_componentBinding[T::componentIndex] = std::move(binding);
		m_componentBindingByName[name] = T::componentIndex;
	}

	template<typename T>
	int LuaBinding::PushComponentOfType(Nz::LuaState& lua, BaseComponent& component)
	{
		T& rightComponent = static_cast<T&>(component);
		lua.Push(rightComponent.CreateHandle());
		return 1;
	}
}
