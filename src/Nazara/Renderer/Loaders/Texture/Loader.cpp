// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Loaders/Texture.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool Check(NzInputStream& stream, const NzMaterialParams& parameters)
	{
		NazaraUnused(stream);
		NazaraUnused(parameters);

		return true; ///FIXME: Pas bon
	}

	bool Load(NzMaterial* material, NzInputStream& stream, const NzMaterialParams& parameters)
	{
		NazaraUnused(parameters);

		NzTexture* texture = new NzTexture;
		if (!texture->LoadFromStream(stream))
		{
			NazaraError("Failed to load diffuse map");
			return false;
		}

		material->Reset();
		material->SetDiffuseMap(texture);
		texture->SetPersistent(false);

		return true;
	}
}

void NzLoaders_Texture_Register()
{
	///FIXME: Pas bon
	NzMaterialLoader::RegisterLoader("bmp,gif,hdr,jpg,jpeg,pic,png,psd,tga", Check, Load);
}

void NzLoaders_Texture_Unregister()
{
	NzMaterialLoader::UnregisterLoader("bmp,gif,hdr,jpg,jpeg,pic,png,psd,tga", Check, Load);
}
