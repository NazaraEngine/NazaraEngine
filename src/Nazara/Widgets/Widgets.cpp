// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup widgets
	* \class Nz::Widgets
	* \brief Widgets class that represents the module initializer of Widgets
	*/
	Widgets::Widgets(Config /*config*/) :
	ModuleBase("Widgets", this)
	{
		CreateDefaultMaterials();
	}

	void Widgets::CreateDefaultMaterials()
	{
		const auto& defaultMaterials = Graphics::Instance()->GetDefaultMaterials();

		const MaterialPassRegistry& materialPassRegistry = Graphics::Instance()->GetMaterialPassRegistry();
		std::size_t depthPassIndex = materialPassRegistry.GetPassIndex("DepthPass");
		std::size_t forwardPassIndex = materialPassRegistry.GetPassIndex("ForwardPass");

		m_opaqueMaterial = defaultMaterials.basicMaterial->CreateInstance();
		for (std::size_t passIndex : { depthPassIndex, forwardPassIndex })
		{
			m_opaqueMaterial->UpdatePassStates(passIndex, [](RenderStates& renderStates)
			{
				renderStates.scissorTest = true;
			});
		}

		m_transparentMaterial = defaultMaterials.basicMaterial->CreateInstance();
		m_transparentMaterial->DisablePass(depthPassIndex);

		m_transparentMaterial->UpdatePassStates(forwardPassIndex, [](RenderStates& renderStates)
		{
			renderStates.blending = true;
			renderStates.blend.modeColor = BlendEquation::Add;
			renderStates.blend.modeAlpha = BlendEquation::Add;
			renderStates.blend.srcColor = BlendFunc::SrcAlpha;
			renderStates.blend.dstColor = BlendFunc::InvSrcAlpha;
			renderStates.blend.srcAlpha = BlendFunc::One;
			renderStates.blend.dstAlpha = BlendFunc::One;
			renderStates.depthWrite = false;
			renderStates.scissorTest = true;
		});
	}

	Widgets* Widgets::s_instance = nullptr;
}
