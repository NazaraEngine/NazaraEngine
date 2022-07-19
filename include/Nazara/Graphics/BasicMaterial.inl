// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline BasicMaterial::BasicMaterial(MaterialPass& material, NoInit) :
	m_material(material)
	{
	}

	/*!
	* \brief Enable/Disable alpha test for this material
	*
	* When enabled, all objects using this material will be rendered using alpha testing,
	* rejecting pixels if their alpha component is under a defined threshold.
	* This allows some kind of transparency with a much cheaper cost as it doesn't prevent any optimization (as deferred rendering or batching).
	*
	* \param alphaTest Defines if this material will use alpha testing
	*
	* \remark Invalidates the pipeline
	*
	* \see IsAlphaTestEnabled
	* \see SetAlphaThreshold
	*/
	inline void BasicMaterial::EnableAlphaTest(bool alphaTest)
	{
		NazaraAssert(HasAlphaTest(), "Material has no alpha test option");
		m_material.SetOptionValue(m_basicOptionIndexes.alphaTest, alphaTest);
	}

	inline const std::shared_ptr<Texture>& BasicMaterial::GetAlphaMap() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha texture slot");
		return m_material.GetTexture(m_basicTextureIndexes.alpha);
	}

	inline const TextureSamplerInfo& BasicMaterial::GetAlphaSampler() const
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha texture slot");
		return m_material.GetTextureSampler(m_basicTextureIndexes.alpha);
	}

	inline const std::shared_ptr<Texture>& BasicMaterial::GetBaseColorMap() const
	{
		NazaraAssert(HasBaseColorMap(), "Material has no alpha texture slot");
		return m_material.GetTexture(m_basicTextureIndexes.baseColor);
	}

	inline const TextureSamplerInfo& BasicMaterial::GetBaseColorSampler() const
	{
		NazaraAssert(HasBaseColorMap(), "Material has no alpha texture slot");
		return m_material.GetTextureSampler(m_basicTextureIndexes.baseColor);
	}

	inline bool BasicMaterial::IsAlphaTestEnabled() const
	{
		NazaraAssert(HasAlphaTest(), "Material has no alpha test option");
		const auto& optionOpt = m_material.GetOptionValue(m_basicOptionIndexes.alphaTest);
		if (std::holds_alternative<nzsl::Ast::NoValue>(optionOpt))
			return false;

		return std::get<bool>(optionOpt);
	}

	inline bool BasicMaterial::HasAlphaMap() const
	{
		return m_basicTextureIndexes.alpha != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasAlphaTest() const
	{
		return m_basicOptionIndexes.alphaTest != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasAlphaTestThreshold() const
	{
		return m_basicUniformOffsets.alphaThreshold != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasBaseColor() const
	{
		return m_basicUniformOffsets.baseColor != MaterialSettings::InvalidIndex;
	}

	inline bool BasicMaterial::HasBaseColorMap() const
	{
		return m_basicTextureIndexes.baseColor != MaterialSettings::InvalidIndex;
	}

	inline void BasicMaterial::SetAlphaMap(std::shared_ptr<Texture> alphaMap)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		bool hasAlphaMap = (alphaMap != nullptr);
		m_material.SetTexture(m_basicTextureIndexes.alpha, std::move(alphaMap));

		if (m_basicOptionIndexes.hasBaseColorMap != MaterialSettings::InvalidIndex)
			m_material.SetOptionValue(m_basicOptionIndexes.hasAlphaMap, hasAlphaMap);
	}

	inline void BasicMaterial::SetAlphaSampler(TextureSamplerInfo alphaSampler)
	{
		NazaraAssert(HasAlphaMap(), "Material has no alpha map slot");
		m_material.SetTextureSampler(m_basicTextureIndexes.alpha, std::move(alphaSampler));
	}

	inline void BasicMaterial::SetBaseColorMap(std::shared_ptr<Texture> baseColorMap)
	{
		NazaraAssert(HasBaseColorMap(), "Material has no diffuse map slot");
		bool hasBaseColorMap = (baseColorMap != nullptr);
		m_material.SetTexture(m_basicTextureIndexes.baseColor, std::move(baseColorMap));

		if (m_basicOptionIndexes.hasBaseColorMap != MaterialSettings::InvalidIndex)
			m_material.SetOptionValue(m_basicOptionIndexes.hasBaseColorMap, hasBaseColorMap);
	}

	inline void BasicMaterial::SetBaseColorSampler(TextureSamplerInfo diffuseSampler)
	{
		NazaraAssert(HasBaseColorMap(), "Material has no diffuse map slot");
		m_material.SetTextureSampler(m_basicTextureIndexes.baseColor, std::move(diffuseSampler));
	}

	inline MaterialPass& BasicMaterial::GetMaterial()
	{
		return m_material;
	}

	inline const MaterialPass& BasicMaterial::GetMaterial() const
	{
		return m_material;
	}

	inline const std::shared_ptr<MaterialSettings>& BasicMaterial::GetSettings()
	{
		return s_basicMaterialSettings;
	}

	inline auto BasicMaterial::GetOffsets() -> const BasicUniformOffsets&
	{
		return s_basicUniformOffsets;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
