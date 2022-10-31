// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PropertyHandler/OptionValuePropertyHandler.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <type_traits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool OptionValuePropertyHandler::NeedsUpdateOnValueUpdate(std::size_t updatedPropertyIndex) const
	{
		return m_propertyIndex == updatedPropertyIndex;
	}

	void OptionValuePropertyHandler::Setup(const Material& material, const ShaderReflection& reflection)
	{
		m_propertyIndex = MaterialSettings::InvalidPropertyIndex;

		const MaterialSettings& settings = material.GetSettings();

		std::size_t propertyIndex = settings.FindValueProperty(m_propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const auto& valueProperty = settings.GetValueProperty(propertyIndex);

		if (const ShaderReflection::OptionData* optionData = reflection.GetOptionByName(m_optionName))
		{
			// TODO: Check if option type matches property type
			NazaraAssert(optionData->hash != 0, "unexpected option hash");
			m_optionHash = optionData->hash;
			m_propertyIndex = propertyIndex;
		}
	}

	void OptionValuePropertyHandler::Update(MaterialInstance& materialInstance) const
	{
		if (m_propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const MaterialSettings::Value& value = materialInstance.GetValueProperty(m_propertyIndex);
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (!std::is_same_v<T, MaterialPropertyNoValue>)
			{
				constexpr MaterialPropertyType PropertyType = TypeToMaterialPropertyType_v<T>;

				using BufferType = typename MaterialPropertyTypeInfo<PropertyType>::BufferType;

				materialInstance.UpdateOptionValue(m_optionHash, MaterialPropertyTypeInfo<PropertyType>::EncodeToOption(arg));
			}
			else
				throw std::runtime_error("value properties must have a default value");
		}, value);
	}
}
