// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Material::Material()
	{
	}

	void Material::AddPass(std::string name, std::shared_ptr<MaterialPass> pass)
	{
		if (HasPass(name))
			return;

		m_passes.emplace(std::move(name), std::move(pass));
	}

	bool Material::HasPass(const std::string& name) const
	{
		return m_passes.find(name) != m_passes.end();
	}

	void Material::RemovePass(const std::string& name)
	{
		m_passes.erase(name);
	}
}
