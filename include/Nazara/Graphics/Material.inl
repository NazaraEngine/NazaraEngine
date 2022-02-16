// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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

	inline const std::shared_ptr<MaterialPass>& Material::FindPass(const std::string& passName) const
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return GetPass(registry.GetPassIndex(passName));
	}

	template<typename F>
	void Material::ForEachPass(F&& callback)
	{
		for (std::size_t i = 0; i < m_passes.size(); ++i)
		{
			if (m_passes[i])
				callback(i, m_passes[i]);
		}
	}

	inline const std::shared_ptr<MaterialPass>& Material::GetPass(std::size_t passIndex) const
	{
		if (passIndex >= m_passes.size())
		{
			static std::shared_ptr<MaterialPass> dummy;
			return dummy;
		}

		return m_passes[passIndex];
	}

	inline bool Material::HasPass(std::size_t passIndex) const
	{
		if (passIndex >= m_passes.size())
			return false;

		return m_passes[passIndex] != nullptr;
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
