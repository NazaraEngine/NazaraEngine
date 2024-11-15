// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/TypeTraits.hpp>
#include <stdexcept>

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

	template<> struct TypeToMaterialPropertyType<UInt32>          { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::UInt; };
	template<> struct TypeToMaterialPropertyType<Vector2<UInt32>> { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::UInt2; };
	template<> struct TypeToMaterialPropertyType<Vector3<UInt32>> { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::UInt3; };
	template<> struct TypeToMaterialPropertyType<Vector4<UInt32>> { static constexpr MaterialPropertyType PropertyType = MaterialPropertyType::UInt4; };

	template<MaterialPropertyType P>
	struct MaterialPropertyTypeInfo
	{
	};

	template<typename T>
	struct MaterialPropertyTypeInfoPrimitive
	{
		using Type = T;
		using BufferType = T;
		using OptionType = T;

		static Type DecodeFromBuffer(BufferType value) { return value; }
		static Type DecodeFromOption(OptionType value) { return value; }
		static BufferType EncodeToBuffer(Type value) { return value; }
		static OptionType EncodeToOption(Type value) { return value; }
	};

	template<typename T, std::size_t N>
	struct MaterialPropertyTypeInfoVector;

	template<typename T>
	struct MaterialPropertyTypeInfoVector<T, 2>
	{
		using Type = Vector2<T>;
		using BufferType = nzsl::Vector2<T>;
		using OptionType = nzsl::Vector2<T>;

		static Type DecodeFromBuffer(BufferType value) { return { value.x(), value.y() }; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.x, value.y }; }
		static OptionType EncodeToOption(Type value) { return OptionType{ value.x, value.y }; }
	};

	template<typename T>
	struct MaterialPropertyTypeInfoVector<T, 3>
	{
		using Type = Vector3<T>;
		using BufferType = nzsl::Vector3<T>;
		using OptionType = nzsl::Vector3<T>;

		static Type DecodeFromBuffer(BufferType value) { return { value.x(), value.y(), value.z() }; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y(), value.z() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.x, value.y, value.z }; }
		static OptionType EncodeToOption(Type value) { return OptionType{ value.x, value.y, value.z }; }
	};

	template<typename T>
	struct MaterialPropertyTypeInfoVector<T, 4>
	{
		using Type = Vector4<T>;
		using BufferType = nzsl::Vector4<T>;
		using OptionType = nzsl::Vector4<T>;

		static Type DecodeFromBuffer(BufferType value) { return { value.x(), value.y(), value.z(), value.w() }; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y(), value.z(), value.w() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.x, value.y, value.z, value.w }; }
		static OptionType EncodeToOption(Type value) { return OptionType{ value.x, value.y, value.z, value.w }; }
	};

	template<>
	struct MaterialPropertyTypeInfo<MaterialPropertyType::Bool>
	{
		using Type = bool;
		using BufferType = UInt32;
		using OptionType = bool;

		static Type DecodeFromBuffer(BufferType value) { return value != 0; }
		static Type DecodeFromOption(OptionType value) { return value; }
		static BufferType EncodeToBuffer(Type value) { return value; }
		static OptionType EncodeToOption(Type value) { return value; }
	};

	template<>
	struct MaterialPropertyTypeInfo<MaterialPropertyType::Bool2>
	{
		using Type = Vector2<bool>;
		using BufferType = nzsl::Vector2u32;
		using OptionType = nzsl::Vector2<bool>;

		static Type DecodeFromBuffer(BufferType value) { return { value.x() != 0, value.y() != 0 }; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.x, value.y }; }
		static OptionType EncodeToOption(Type value) { return OptionType{ value.x, value.y }; }
	};

	template<>
	struct MaterialPropertyTypeInfo<MaterialPropertyType::Bool3>
	{
		using Type = Vector3<bool>;
		using BufferType = nzsl::Vector3u32;
		using OptionType = nzsl::Vector3<bool>;

		static Type DecodeFromBuffer(BufferType value) { return { value.x() != 0, value.y() != 0, value.z() != 0 }; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y(), value.z() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.x, value.y, value.z }; }
		static OptionType EncodeToOption(Type value) { return OptionType{ value.x, value.y, value.z }; }
	};

	template<>
	struct MaterialPropertyTypeInfo<MaterialPropertyType::Bool4>
	{
		using Type = Vector4<bool>;
		using BufferType = nzsl::Vector4u32;
		using OptionType = nzsl::Vector4<bool>;

		static Type DecodeFromBuffer(BufferType value) { return { value.x() != 0, value.y() != 0, value.z() != 0, value.w() != 0}; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y(), value.z(), value.w() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.x, value.y, value.z, value.w }; }
		static OptionType EncodeToOption(Type value) { return OptionType{ value.x, value.y, value.z, value.w }; }
	};

	template<>
	struct MaterialPropertyTypeInfo<MaterialPropertyType::Color>
	{
		using Type = Color;
		using BufferType = nzsl::Vector4f32;
		using OptionType = nzsl::Vector4f32;

		static Type DecodeFromBuffer(BufferType value) { return { value.x(), value.y(), value.z(), value.w() }; }
		static Type DecodeFromOption(OptionType value) { return { value.x(), value.y(), value.z(), value.w() }; }
		static BufferType EncodeToBuffer(Type value) { return BufferType{ value.r, value.g, value.b, value.a }; }
		static OptionType EncodeToOption(Type value) { return BufferType{ value.r, value.g, value.b, value.a }; }
	};

	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Float>  : MaterialPropertyTypeInfoPrimitive<float> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Float2> : MaterialPropertyTypeInfoVector<float, 2> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Float3> : MaterialPropertyTypeInfoVector<float, 3> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Float4> : MaterialPropertyTypeInfoVector<float, 4> {};

	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Int>  : MaterialPropertyTypeInfoPrimitive<Int32> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Int2> : MaterialPropertyTypeInfoVector<Int32, 2> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Int3> : MaterialPropertyTypeInfoVector<Int32, 3> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::Int4> : MaterialPropertyTypeInfoVector<Int32, 4> {};

	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::UInt>  : MaterialPropertyTypeInfoPrimitive<UInt32> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::UInt2> : MaterialPropertyTypeInfoVector<UInt32, 2> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::UInt3> : MaterialPropertyTypeInfoVector<UInt32, 3> {};
	template<> struct MaterialPropertyTypeInfo<MaterialPropertyType::UInt4> : MaterialPropertyTypeInfoVector<UInt32, 4> {};


	inline void MaterialSettings::AddBufferProperty(std::string propertyName)
	{
		return AddBufferProperty(std::move(propertyName), nullptr, 0, 0);
	}

	inline void MaterialSettings::AddPass(std::size_t passIndex, MaterialPass materialPass)
	{
		if (passIndex >= m_materialPasses.size())
			m_materialPasses.resize(passIndex + 1);

		m_materialPasses[passIndex] = std::move(materialPass);
	}

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

	inline std::size_t MaterialSettings::FindBufferProperty(std::string_view propertyName) const
	{
		for (std::size_t i = 0; i < m_bufferProperties.size(); ++i)
		{
			if (m_bufferProperties[i].name == propertyName)
				return i;
		}

		return InvalidPropertyIndex;
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

	inline auto MaterialSettings::GetBufferProperty(std::size_t bufferProperty) const -> const BufferProperty&
	{
		assert(bufferProperty < m_bufferProperties.size());
		return m_bufferProperties[bufferProperty];
	}

	inline std::size_t MaterialSettings::GetBufferPropertyCount() const
	{
		return m_bufferProperties.size();
	}

	inline const MaterialPass* MaterialSettings::GetPass(std::size_t passIndex) const
	{
		if (passIndex > m_materialPasses.size() || !m_materialPasses[passIndex].has_value())
			return nullptr;

		return &m_materialPasses[passIndex].value();
	}

	inline const std::vector<std::optional<MaterialPass>>& MaterialSettings::GetPasses() const
	{
		return m_materialPasses;
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
