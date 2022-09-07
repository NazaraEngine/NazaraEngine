// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool MaterialParams::IsValid() const
	{
		return true;
	}

	void Material::AddPass(std::string passName, std::shared_ptr<MaterialPass> pass)
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return AddPass(registry.GetPassIndex(passName), std::move(pass));
	}

	const std::shared_ptr<MaterialPass>& Material::FindPass(const std::string& passName) const
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return GetPass(registry.GetPassIndex(passName));
	}

	void Material::RemovePass(const std::string& passName)
	{
		auto& registry = Graphics::Instance()->GetMaterialPassRegistry();
		return RemovePass(registry.GetPassIndex(passName));
	}

	std::shared_ptr<Material> Material::LoadFromFile(const std::filesystem::path& filePath, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Utility module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Material> Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Utility module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Material> Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Utility module has not been initialized");

		return graphics->GetMaterialLoader().LoadFromStream(stream, params);
	}
}
