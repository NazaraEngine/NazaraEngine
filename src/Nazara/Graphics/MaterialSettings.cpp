// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void MaterialSettings::AddPass(std::string_view passName, MaterialPass materialPass)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return AddPass(passIndex, std::move(materialPass));
	}

	const MaterialPass* MaterialSettings::GetPass(std::string_view passName) const
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return GetPass(passIndex);
	}
}
