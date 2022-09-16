// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void BasicMaterial::EnableAlphaTest(bool alphaTest)
	{
		if (m_forwardPass)
			m_forwardPass->EnableAlphaTest(alphaTest);

		if (m_depthPass)
			m_depthPass->EnableAlphaTest(alphaTest);
	}

	inline void BasicMaterial::EnableBlending(bool blending)
	{
		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().EnableBlending(blending);
	}

	inline void BasicMaterial::EnableDepthBuffer(bool depthBuffer)
	{
		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().EnableDepthBuffer(depthBuffer);

		UpdatePasses();
	}

	inline void BasicMaterial::EnableDepthClamp(bool depthClamp)
	{
		if (m_depthPass)
			m_depthPass->GetMaterialPass().EnableDepthClamp(depthClamp);

		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().EnableDepthClamp(depthClamp);
	}

	inline void BasicMaterial::EnableDepthPass(bool depthPass)
	{
		if (m_depthPass)
			m_depthPass->GetMaterialPass().Enable(depthPass);
	}

	inline void BasicMaterial::EnableDepthWrite(bool depthWrite)
	{
		if (m_depthPass)
		{
			MaterialPass& depthPass = m_depthPass->GetMaterialPass();
			depthPass.Enable(IsDepthBufferEnabled() && depthWrite);
		}

		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().EnableDepthWrite(depthWrite);
	}

	inline void BasicMaterial::EnableFaceCulling(bool faceCulling)
	{
		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().EnableFaceCulling(faceCulling);

		if (m_depthPass)
			m_depthPass->GetMaterialPass().EnableFaceCulling(faceCulling);
	}

	inline void BasicMaterial::EnableForwardPass(bool forwardPass)
	{
		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().Enable(forwardPass);
	}

	inline ColorComponentMask BasicMaterial::GetColorWriteMask() const
	{
		if (m_forwardPass)
			return m_forwardPass->GetMaterialPass().GetColorWriteMask();
	}

	inline const std::shared_ptr<Texture>& BasicMaterial::GetAlphaMap() const
	{
		if (m_forwardPass)
			return m_forwardPass->GetAlphaMap();
		else if (m_depthPass)
			return m_depthPass->GetAlphaMap();
	}

	inline const TextureSamplerInfo& BasicMaterial::GetAlphaSampler() const
	{
		if (m_forwardPass)
			return m_forwardPass->GetAlphaSampler();
		else if (m_depthPass)
			return m_depthPass->GetAlphaSampler();
	}

	inline float BasicMaterial::GetAlphaTestThreshold() const
	{
		if (m_forwardPass)
			return m_forwardPass->GetAlphaTestThreshold();
		else if (m_depthPass)
			return m_depthPass->GetAlphaTestThreshold();

		return 0.0f;
	}

	inline void BasicMaterial::UpdatePasses()
	{
		if (m_depthPass)
			m_depthPass->GetMaterialPass().Enable(m_isDepthPassEnabled && IsDepthBufferEnabled() && IsDepthWriteEnabled());

		if (m_forwardPass)
			m_forwardPass->GetMaterialPass().Enable(m_isForwardPassEnabled && GetColorWriteMask() != 0);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
