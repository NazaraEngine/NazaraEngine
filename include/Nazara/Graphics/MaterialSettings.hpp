// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALSETTINGS_HPP
#define NAZARA_GRAPHICS_MATERIALSETTINGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utils/TypeList.hpp>
#include <limits>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Nz
{
	template<typename T> struct TypeToMaterialPropertyType;

	template<typename T> constexpr MaterialPropertyType TypeToMaterialPropertyType_v = TypeToMaterialPropertyType<T>::PropertyType;

	template<MaterialPropertyType P> struct MaterialPropertyTypeInfo;

	struct MaterialPropertyNoValue {};

	using MaterialPropertyTypeList = TypeList<
		MaterialPropertyNoValue,
		Color,
		bool, Vector2<bool>, Vector3<bool>, Vector4<bool>,
		float, Vector2<float>, Vector3<float>, Vector4<float>,
		Int32, Vector2<Int32>, Vector3<Int32>, Vector4<Int32>,
		UInt32, Vector2<UInt32>, Vector3<UInt32>, Vector4<UInt32>
	>;

	class NAZARA_GRAPHICS_API MaterialSettings
	{
		public:
			struct TextureProperty;
			struct ValueProperty;

			using Value = TypeListInstantiate<MaterialPropertyTypeList, std::variant>;

			MaterialSettings() = default;
			MaterialSettings(const MaterialSettings&) = delete;
			MaterialSettings(MaterialSettings&&) noexcept = default;
			~MaterialSettings() = default;

			void AddPass(std::string_view passName, MaterialPass materialPass);
			inline void AddPass(std::size_t passIndex, MaterialPass materialPass);
			inline void AddPropertyHandler(std::unique_ptr<PropertyHandler> propertyHandler);
			inline void AddTextureProperty(std::string propertyName, ImageType propertyType);
			inline void AddTextureProperty(std::string propertyName, ImageType propertyType, std::shared_ptr<Texture> defaultTexture);
			inline void AddTextureProperty(std::string propertyName, ImageType propertyType, std::shared_ptr<Texture> defaultTexture, const TextureSamplerInfo& defaultSamplerInfo);
			inline void AddValueProperty(std::string propertyName, MaterialPropertyType propertyType, Value defaultValue);
			template<typename T> void AddValueProperty(std::string propertyName);
			template<typename T, typename U> void AddValueProperty(std::string propertyName, U&& defaultValue);

			inline std::size_t FindTextureProperty(std::string_view propertyName) const;
			inline std::size_t FindValueProperty(std::string_view propertyName) const;

			const MaterialPass* GetPass(std::string_view passName) const;
			inline const MaterialPass* GetPass(std::size_t passIndex) const;
			inline const std::vector<std::optional<MaterialPass>>& GetPasses() const;
			inline const std::vector<std::unique_ptr<PropertyHandler>>& GetPropertyHandlers() const;
			inline const TextureProperty& GetTextureProperty(std::size_t texturePropertyIndex) const;
			inline std::size_t GetTexturePropertyCount() const;
			inline const ValueProperty& GetValueProperty(std::size_t valuePropertyIndex) const;
			inline std::size_t GetValuePropertyCount() const;

			MaterialSettings& operator=(const MaterialSettings&) = delete;
			MaterialSettings& operator=(MaterialSettings&&) = default;

			struct TextureProperty
			{
				std::shared_ptr<Texture> defaultTexture;
				std::string name;
				ImageType type;
				TextureSamplerInfo defaultSamplerInfo;
			};

			struct ValueProperty
			{
				Value defaultValue;
				std::string name;
				MaterialPropertyType type;
			};

			static constexpr std::size_t InvalidPropertyIndex = std::numeric_limits<std::size_t>::max();

		private:
			std::vector<std::unique_ptr<PropertyHandler>> m_propertyHandlers;
			std::vector<std::optional<MaterialPass>> m_materialPasses;
			std::vector<TextureProperty> m_textureProperties;
			std::vector<ValueProperty> m_valueProperties;
	};
}

#include <Nazara/Graphics/MaterialSettings.inl>

#endif // NAZARA_GRAPHICS_MATERIALSETTINGS_HPP
