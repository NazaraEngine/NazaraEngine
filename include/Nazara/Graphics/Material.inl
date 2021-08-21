// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void Material::AddPass(std::size_t passIndex, std::shared_ptr<MaterialPass> pass)
	{
		if (passIndex >= m_passes.size())
			m_passes.resize(passIndex + 1);

		m_passes[passIndex] = std::move(pass);
	}

	inline void Material::AddPass(std::string passName, std::shared_ptr<MaterialPass> pass)
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return AddPass(registry.GetPassIndex(passName), std::move(pass));
	}

	inline MaterialPass* Material::GetPass(std::size_t passIndex) const
	{
		if (passIndex >= m_passes.size())
			return nullptr;

		return m_passes[passIndex].get();
	}

	inline bool Material::HasPass(std::size_t passIndex) const
	{
		return GetPass(passIndex) != nullptr;
	}

	inline void Material::RemovePass(std::size_t passIndex)
	{
		if (passIndex >= m_passes.size())
			return;

		m_passes[passIndex].reset();
	}

	inline void Material::RemovePass(const std::string& passName)
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return RemovePass(registry.GetPassIndex(passName));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
