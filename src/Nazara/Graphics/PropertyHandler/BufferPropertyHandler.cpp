// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PropertyHandler/BufferPropertyHandler.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>

namespace Nz
{
	bool BufferPropertyHandler::NeedsUpdateOnStorageBufferUpdate(std::size_t updatedPropertyIndex) const
	{
		return m_propertyIndex == updatedPropertyIndex;
	}

	void BufferPropertyHandler::Setup(const Material& material, const ShaderReflection& reflection)
	{
		m_propertyIndex = MaterialSettings::InvalidPropertyIndex;

		const MaterialSettings& settings = material.GetSettings();

		std::size_t propertyIndex = settings.FindBufferProperty(m_propertyName);
		if (propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		m_propertyIndex = propertyIndex;

		m_storageBufferIndex = material.FindStorageBufferByTag(m_bufferTag);
		if (m_storageBufferIndex == Material::InvalidIndex)
			return;

		m_optionHash = 0;
		if (!m_optionName.empty())
		{
			if (const ShaderReflection::OptionData* optionData = reflection.GetOptionByName(m_optionName))
			{
				if (IsPrimitiveType(optionData->type) && std::get<nzsl::Ast::PrimitiveType>(optionData->type) == nzsl::Ast::PrimitiveType::Boolean)
				{
					NazaraAssertMsg(optionData->hash != 0, "unexpected option hash");
					m_optionHash = optionData->hash;
				}
				else
					NazaraError("option {0} is not a boolean option (got {1})", m_optionName, nzsl::Ast::ToString(optionData->type));
			}
			else
				NazaraWarning("option {0} not found in shader for property {1}", m_optionName, m_propertyName);
		}
	}

	void BufferPropertyHandler::Update(MaterialInstance& materialInstance) const
	{
		if (m_propertyIndex == MaterialSettings::InvalidPropertyIndex)
			return;

		const MaterialSettings::BufferValue& storageBuffer = materialInstance.GetBufferProperty(m_propertyIndex);
		if (!storageBuffer.buffer)
			return;

		materialInstance.UpdateStorageBufferBinding(m_storageBufferIndex, storageBuffer.buffer, storageBuffer.offset, storageBuffer.size);
		if (m_optionHash != 0)
			materialInstance.UpdateOptionValue(m_optionHash, storageBuffer.buffer != nullptr);
	}
}
