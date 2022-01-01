// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup widgets
	* \class Nz::Widgets
	* \brief Widgets class that represents the module initializer of Widgets
	*/
	Widgets::Widgets(Config config) :
	ModuleBase("Widgets", this)
	{
		ECS::RegisterComponents();

		CreateDefaultMaterials();
	}

	void Widgets::CreateDefaultMaterials()
	{
		m_opaqueMaterialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
		m_opaqueMaterialPass->EnableDepthBuffer(true);
		m_opaqueMaterialPass->EnableDepthWrite(false);
		m_opaqueMaterialPass->EnableScissorTest(true);

		m_opaqueMaterial = std::make_shared<Material>();
		m_opaqueMaterial->AddPass("ForwardPass", m_opaqueMaterialPass);

		m_transparentMaterialPass = std::make_shared<MaterialPass>(BasicMaterial::GetSettings());
		m_transparentMaterialPass->EnableDepthBuffer(true);
		m_transparentMaterialPass->EnableDepthWrite(false);
		m_transparentMaterialPass->EnableScissorTest(true);
		m_transparentMaterialPass->EnableBlending(true);
		m_transparentMaterialPass->SetBlendEquation(BlendEquation::Add, BlendEquation::Add);
		m_transparentMaterialPass->SetBlendFunc(BlendFunc::SrcAlpha, BlendFunc::InvSrcAlpha, BlendFunc::One, BlendFunc::One);

		m_transparentMaterial = std::make_shared<Material>();
		m_transparentMaterial->AddPass("ForwardPass", m_transparentMaterialPass);
	}

	Widgets* Widgets::s_instance = nullptr;
}
