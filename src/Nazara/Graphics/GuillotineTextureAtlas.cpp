// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	UInt32 GuillotineTextureAtlas::GetStorage() const
	{
		return DataStorage_Hardware;
	}

	AbstractImage* GuillotineTextureAtlas::ResizeImage(AbstractImage* oldImage, const Vector2ui& size) const
	{
		std::unique_ptr<Texture> newTexture(new Texture);
		if (newTexture->Create(ImageType_2D, PixelFormatType_A8, size.x, size.y, 1))
		{
			if (oldImage)
			{
				Texture* oldTexture = static_cast<Texture*>(oldImage);

				// Copie des anciennes données
				///TODO: Copie de texture à texture
				Image image;
				if (!oldTexture->Download(&image))
				{
					NazaraError("Failed to download old texture");
					return nullptr;
				}

				if (!newTexture->Update(image, Rectui(0, 0, image.GetWidth(), image.GetHeight())))
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
}
