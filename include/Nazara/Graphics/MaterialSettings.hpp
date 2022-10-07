// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALSETTINGS_HPP
#define NAZARA_GRAPHICS_MATERIALSETTINGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utils/TypeList.hpp>
#include <array>
#include <limits>
#include <string>
#include <vector>

namespace Nz
{
	template<typename T> struct TypeToMaterialPropertyType;

	template<typename T> constexpr MaterialPropertyType TypeToMaterialPropertyType_v = TypeToMaterialPropertyType<T>::PropertyType;

	using MaterialPropertyTypeList = TypeList<
		Color,
		bool, Vector2<bool>, Vector3<bool>, Vector4<bool>,
		float, Vector2<float>, Vector3<float>, Vector4<float>,
		Int32, Vector2<Int32>, Vector3<Int32>, Vector4<Int32>,
		UInt32, Vector2<UInt32>, Vector3<UInt32>, Vector4<UInt32>
	>;

	class MaterialSettings
	{
		public:
			struct TextureProperty;
			struct ValueProperty;

			using Value = TypeListInstantiate<MaterialPropertyTypeList, std::variant>;

			MaterialSettings();
			MaterialSettings(const MaterialSettings&) = default;
			MaterialSettings(MaterialSettings&&) = delete;
			~MaterialSettings() = default;

			inline void AddProperty(std::string propertyName, MaterialPropertyType propertyType);
			inline void AddTextureProperty(std::string propertyName, ImageType propertyType);
			template<typename T> void AddValueProperty(std::string propertyName);
			template<typename T, typename U> void AddValueProperty(std::string propertyName, U&& defaultValue);

			MaterialSettings& operator=(const MaterialSettings&) = delete;
			MaterialSettings& operator=(MaterialSettings&&) = delete;

			struct ValueProperty
			{
				std::string name;
				MaterialPropertyType type;
			};

		private:
	};
}

#include <Nazara/Graphics/MaterialSettings.inl>

#endif // NAZARA_GRAPHICS_MATERIALSETTINGS_HPP
