#pragma once

#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename T>
		struct IsSuitableForComponent
		{
			constexpr static bool value = false;
		};
	}

	template<typename T> constexpr ComponentType ComponentTypeId()
	{
		static_assert(Detail::IsSuitableForComponent<T>::value, "This type cannot be used as a component.");
		return ComponentType{};
	}

	template<> constexpr ComponentType ComponentTypeId<Color>() { return ComponentType_Color; }
	template<> constexpr ComponentType ComponentTypeId<double>() { return ComponentType_Double1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2d>() { return ComponentType_Double2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3d>() { return ComponentType_Double3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4d>() { return ComponentType_Double4; }
	template<> constexpr ComponentType ComponentTypeId<float>() { return ComponentType_Float1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2f>() { return ComponentType_Float2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3f>() { return ComponentType_Float3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4f>() { return ComponentType_Float4; }
	template<> constexpr ComponentType ComponentTypeId<int>() { return ComponentType_Int1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2i>() { return ComponentType_Int2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3i>() { return ComponentType_Int3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4i>() { return ComponentType_Int4; }
	template<> constexpr ComponentType ComponentTypeId<Quaternionf>() { return ComponentType_Quaternion; }

	template<typename T>
	constexpr ComponentType GetComponentTypeOf()
	{ 
		return ComponentTypeId<std::decay_t<T>>(); 
	}
}

#include <Nazara/Graphics/DebugOff.hpp>