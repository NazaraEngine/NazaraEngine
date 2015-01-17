// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

nzUInt32 NzGuillotineTextureAtlas::GetStorage() const
{
	return nzDataStorage_Hardware;
}

NzAbstractImage* NzGuillotineTextureAtlas::ResizeImage(NzAbstractImage* oldImage, const NzVector2ui& size) const
{
	std::unique_ptr<NzTexture> newTexture(new NzTexture);
	if (newTexture->Create(nzImageType_2D, nzPixelFormat_A8, size.x, size.y, 1, 0xFF))
	{
		if (oldImage)
		{
			NzTexture* oldTexture = static_cast<NzTexture*>(oldImage);

			// Copie des anciennes données
			///TODO: Copie de texture à texture
			NzImage image;
			if (!oldTexture->Download(&image))
			{
				NazaraError("Failed to download old texture");
				return nullptr;
			}

			if (!newTexture->Update(image, NzRectui(0, 0, image.GetWidth(), image.GetHeight())))
			{
				NazaraError("Failed to update texture");
				return nullptr;
			}
		}

		return newTexture.release();
	}
	else
	{
		// Si on arrive ici c'est que la taille demandée est trop grande pour la carte graphique
		// ou que nous manquons de mémoire
		return nullptr;
	}
}
