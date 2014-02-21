// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Loaders/Texture.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	nzTernary Check(NzInputStream& stream, const NzMaterialParams& parameters)
	{
		NazaraUnused(stream);
		NazaraUnused(parameters);

		return nzTernary_Unknown;
	}

	bool Load(NzMaterial* material, NzInputStream& stream, const NzMaterialParams& parameters)
	{
		NazaraUnused(parameters);

		std::unique_ptr<NzTexture> texture(new NzTexture);
		texture->SetPersistent(false);

		if (!texture->LoadFromStream(stream))
		{
			NazaraError("Failed to load diffuse map");
			return false;
		}

		material->Reset();
		material->SetDiffuseMap(texture.get());
		texture.release();

		return true;
	}
}

void NzLoaders_Texture_Register()
{
	NzMaterialLoader::RegisterLoader(NzImageLoader::IsExtensionSupported, Check, Load);
}

void NzLoaders_Texture_Unregister()
{
	NzMaterialLoader::UnregisterLoader(NzImageLoader::IsExtensionSupported, Check, Load);
}
