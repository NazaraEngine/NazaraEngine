// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PropertyHandler/UniformValuePropertyHandler.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <type_traits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool UniformValuePropertyHandler::NeedsUpdateOnValueUpdate(std::size_t updatedPropertyIndex) const
	{
		return m_propertyIndex == updatedPropertyIndex;
	}

	void UniformValuePropertyHandler::Setup(const Material& material, const ShaderReflection& reflection)
	{
		m_propertyIndex = MaterialSettings::InvalidPropertyIndex;

		const MaterialSettings& settings = material.GetSettings();

		std::size_t propertyIndex = settings.FindValueProperty(m_propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const auto& valueProperty = settings.GetValueProperty(propertyIndex);

		m_uniformBlockIndex = material.FindUniformBlockByTag(m_blockTag);
		if (m_uniformBlockIndex == Material::InvalidIndex)
			return;

		const auto& uniformBlockData = material.GetUniformBlockData(m_uniformBlockIndex);

		const ShaderReflection::StructData* structData = reflection.GetStructByIndex(uniformBlockData.structIndex);
		NazaraAssertFmt(structData, "invalid struct index {0}", uniformBlockData.structIndex);

		auto it = structData->members.find(m_memberTag);
		if (it == structData->members.end())
			return;

		m_offset = it->second.offset;
		m_size = it->second.size;
		m_propertyIndex = propertyIndex;

		// TODO: Check if member type matches property type
	}

	void UniformValuePropertyHandler::Update(MaterialInstance& materialInstance) const
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

				BufferType value = MaterialPropertyTypeInfo<PropertyType>::EncodeToBuffer(arg);
				assert(sizeof(value) == m_size);
				materialInstance.UpdateUniformBufferData(m_uniformBlockIndex, m_offset, m_size, &value);
			}
			else
				throw std::runtime_error("value properties must have a default value");
		}, value);
	}
}
