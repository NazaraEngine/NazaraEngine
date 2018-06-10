// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		Ternary Check(Stream& stream, const MaterialParams& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeTextureLoader", &skip) && skip)
				return Ternary_False;

			return Ternary_Unknown;
		}

		bool Load(Material* material, Stream& stream, const MaterialParams& parameters)
		{
			NazaraUnused(parameters);

			TextureRef texture = Texture::New();
			if (!texture->LoadFromStream(stream))
			{
				NazaraError("Failed to load diffuse map");
				return false;
			}

			material->Reset();
			material->SetDiffuseMap(texture);
			material->SetShader(parameters.shaderName);

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterTexture()
		{
			MaterialLoader::RegisterLoader(ImageLoader::IsExtensionSupported, Check, Load);
		}

		void UnregisterTexture()
		{
			MaterialLoader::UnregisterLoader(ImageLoader::IsExtensionSupported, Check, Load);
		}
	}
}
