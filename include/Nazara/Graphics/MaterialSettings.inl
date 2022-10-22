// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <stdexcept>
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

	inline void MaterialSettings::AddPropertyHandler(std::unique_ptr<PropertyHandler> propertyHandler)
	{
		m_propertyHandlers.emplace_back(std::move(propertyHandler));
	}

	inline void MaterialSettings::AddTextureProperty(std::string propertyName, ImageType propertyType)
	{
		auto& textureProperty = m_textureProperties.emplace_back();
		textureProperty.name = std::move(propertyName);
		textureProperty.type = propertyType;
	}

	inline void MaterialSettings::AddTextureProperty(std::string propertyName, ImageType propertyType, std::shared_ptr<Texture> defaultTexture)
	{
		if (defaultTexture && defaultTexture->GetType() != propertyType)
			throw std::runtime_error("default texture type doesn't match property image type");

		auto& textureProperty = m_textureProperties.emplace_back();
		textureProperty.name = std::move(propertyName);
		textureProperty.type = propertyType;
		textureProperty.defaultTexture = std::move(defaultTexture);
	}

	inline void MaterialSettings::AddValueProperty(std::string propertyName, MaterialPropertyType propertyType, Value defaultValue)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (!std::is_same_v<T, MaterialPropertyNoValue>)
			{
				constexpr MaterialPropertyType valueType = TypeToMaterialPropertyType_v<T>;
				if (propertyType != valueType)
					throw std::runtime_error("default value type doesn't match property type");
			}
			else
				throw std::runtime_error("value properties must have a default value");
		}, defaultValue);

		auto& valueProperty = m_valueProperties.emplace_back();
		valueProperty.name = std::move(propertyName);
		valueProperty.type = propertyType;
		valueProperty.defaultValue = std::move(defaultValue);
	}

	inline std::size_t MaterialSettings::FindTextureProperty(std::string_view propertyName) const
	{
		for (std::size_t i = 0; i < m_textureProperties.size(); ++i)
		{
			if (m_textureProperties[i].name == propertyName)
				return i;
		}

		return InvalidPropertyIndex;
	}

	inline std::size_t MaterialSettings::FindValueProperty(std::string_view propertyName) const
	{
		for (std::size_t i = 0; i < m_valueProperties.size(); ++i)
		{
			if (m_valueProperties[i].name == propertyName)
				return i;
		}

		return InvalidPropertyIndex;
	}

	inline const std::vector<std::unique_ptr<PropertyHandler>>& MaterialSettings::GetPropertyHandlers() const
	{
		return m_propertyHandlers;
	}

	inline auto MaterialSettings::GetTextureProperty(std::size_t texturePropertyIndex) const -> const TextureProperty&
	{
		assert(texturePropertyIndex < m_textureProperties.size());
		return m_textureProperties[texturePropertyIndex];
	}

	inline std::size_t MaterialSettings::GetTexturePropertyCount() const
	{
		return m_textureProperties.size();
	}

	inline auto MaterialSettings::GetValueProperty(std::size_t valuePropertyIndex) const -> const ValueProperty&
	{
		assert(valuePropertyIndex < m_valueProperties.size());
		return m_valueProperties[valuePropertyIndex];
	}

	inline std::size_t MaterialSettings::GetValuePropertyCount() const
	{
		return m_valueProperties.size();
	}

	template<typename T>
	void MaterialSettings::AddValueProperty(std::string propertyName)
	{
		return AddValueProperty(std::move(propertyName), TypeToMaterialPropertyType_v<T>);
	}

	template<typename T, typename U>
	void MaterialSettings::AddValueProperty(std::string propertyName, U&& defaultValue)
	{
		constexpr std::size_t TypeIndex = TypeListFind<MaterialPropertyTypeList, T>;

		auto& valueProperty = m_valueProperties.emplace_back();
		valueProperty.name = std::move(propertyName);
		valueProperty.type = TypeToMaterialPropertyType_v<T>;
		valueProperty.defaultValue.emplace<TypeIndex>(std::forward<U>(defaultValue));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
