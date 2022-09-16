// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	BasicMaterial::BasicMaterial(Material& material)
	{
		if (auto forwardPass = material.FindPass("ForwardPass"))
			m_forwardPass.emplace(*forwardPass);

		if (auto depthPass = material.FindPass("DepthPass"))
			m_depthPass.emplace(*depthPass);
	}

	std::shared_ptr<Material> BasicMaterial::Build()
	{
		return std::shared_ptr<Material>();
	}
}
