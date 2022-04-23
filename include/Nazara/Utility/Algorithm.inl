// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	template<typename T> constexpr ComponentType ComponentTypeId()
	{
		static_assert(AlwaysFalse<T>::value, "This type cannot be used as a component.");
		return ComponentType{};
	}

	template<> constexpr ComponentType ComponentTypeId<Color>()       { return ComponentType::Color; }
	template<> constexpr ComponentType ComponentTypeId<double>()      { return ComponentType::Double1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2d>()    { return ComponentType::Double2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3d>()    { return ComponentType::Double3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4d>()    { return ComponentType::Double4; }
	template<> constexpr ComponentType ComponentTypeId<float>()       { return ComponentType::Float1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2f>()    { return ComponentType::Float2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3f>()    { return ComponentType::Float3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4f>()    { return ComponentType::Float4; }
	template<> constexpr ComponentType ComponentTypeId<int>()         { return ComponentType::Int1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2i>()    { return ComponentType::Int2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3i>()    { return ComponentType::Int3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4i>()    { return ComponentType::Int4; }

	template<typename T>
	constexpr ComponentType GetComponentTypeOf()
	{ 
		return ComponentTypeId<std::decay_t<T>>(); 
	}
}

#include <Nazara/Utility/DebugOff.hpp>
