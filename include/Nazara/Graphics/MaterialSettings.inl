// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	template<typename T> class Vector2;
	template<typename T> class Vector3;
	template<typename T> class Vector4;

	template<typename T>
	struct TypeToMaterialPropertyType
	{
		static_assert(AlwaysFalse<T>(), "T is not a valid type for material property");
	};

	template<> struct TypeToMaterialPropertyType<bool>            { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Bool;  };
	template<> struct TypeToMaterialPropertyType<Vector2<bool>>   { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Bool2; };
	template<> struct TypeToMaterialPropertyType<Vector3<bool>>   { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Bool3; };
	template<> struct TypeToMaterialPropertyType<Vector4<bool>>   { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Bool4; };

	template<> struct TypeToMaterialPropertyType<Color>           { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Color; };

	template<> struct TypeToMaterialPropertyType<float>           { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Float;  };
	template<> struct TypeToMaterialPropertyType<Vector2<float>>  { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Float2; };
	template<> struct TypeToMaterialPropertyType<Vector3<float>>  { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Float3; };
	template<> struct TypeToMaterialPropertyType<Vector4<float>>  { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Float4; };

	template<> struct TypeToMaterialPropertyType<Int32>           { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int; };
	template<> struct TypeToMaterialPropertyType<Vector2<Int32>>  { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int2; };
	template<> struct TypeToMaterialPropertyType<Vector3<Int32>>  { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int3; };
	template<> struct TypeToMaterialPropertyType<Vector4<Int32>>  { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int4; };
	
	template<> struct TypeToMaterialPropertyType<UInt32>          { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int; };
	template<> struct TypeToMaterialPropertyType<Vector2<UInt32>> { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int2; };
	template<> struct TypeToMaterialPropertyType<Vector3<UInt32>> { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int3; };
	template<> struct TypeToMaterialPropertyType<Vector4<UInt32>> { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::Int4; };
}

#include <Nazara/Graphics/DebugOff.hpp>
