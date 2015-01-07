// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

bool NzGuillotineTextureAtlas::ResizeImage(Layer& layer, const NzVector2ui& size) const
{
	NzTexture newTexture;
	if (newTexture.Create(nzImageType_2D, nzPixelFormat_A8, size.x, size.y, 1, 0xFF))
	{
		newTexture.EnableMipmapping(true);

		if (layer.image)
		{
			NzTexture& texture = *static_cast<NzTexture*>(layer.image.get());

			// Copie des anciennes données
			///TODO: Copie de texture à texture
			NzImage image;
			if (!texture.Download(&image))
			{
				NazaraError("Failed to download old texture");
				return false;
			}

			if (!newTexture.Update(image, NzRectui(0, 0, image.GetWidth(), image.GetHeight())))
			{
				NazaraError("Failed to update texture");
				return false;
			}

			texture = std::move(newTexture);
		}
		else
			layer.image.reset(new NzTexture(std::move(newTexture)));

		return true;
	}
	else
	{
		NazaraError("Failed to create texture");
		return false;
	}
}

unsigned int NzGuillotineTextureAtlas::GetMaxAtlasSize() const
{
	///FIXME: D'après la documentation OpenGL, cette valeur n'est qu'une approximation et les texture proxies sont une meilleure solution
	///       Cependant le test ne se fait pas au même moment, penser à adapter le code pour gérer ce cas ?
	///       (Cela permettrait au passage de gérer le cas où une image ne peut être allouée car il n'y a pas assez de mémoire contigüe pour la contenir)

	return NzRenderer::GetMaxTextureSize();
}
