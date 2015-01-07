// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/GuillotineImageAtlas.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	const unsigned int s_atlasStartSize = 512;
}

NzGuillotineImageAtlas::NzGuillotineImageAtlas() :
m_rectChoiceHeuristic(NzGuillotineBinPack::RectBestAreaFit),
m_rectSplitHeuristic(NzGuillotineBinPack::SplitMinimizeArea)
{
}

NzGuillotineImageAtlas::~NzGuillotineImageAtlas() = default;

void NzGuillotineImageAtlas::Clear()
{
	m_layers.clear();
	NotifyCleared();
}

void NzGuillotineImageAtlas::Free(NzSparsePtr<const NzRectui> rects, NzSparsePtr<unsigned int> layers, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		#ifdef NAZARA_DEBUG
		if (layers[i] >= m_layers.size())
		{
			NazaraWarning("Rectangle #" + NzString::Number(i) + " belong to an out-of-bounds layer (" + NzString::Number(i) + " >= " + NzString::Number(m_layers.size()) + ")");
			continue;
		}
		#endif

		m_layers[layers[i]].binPack.FreeRectangle(rects[i]);
		m_layers[layers[i]].freedRectangles++;
	}
}

NzGuillotineBinPack::FreeRectChoiceHeuristic NzGuillotineImageAtlas::GetRectChoiceHeuristic() const
{
	return m_rectChoiceHeuristic;
}

NzGuillotineBinPack::GuillotineSplitHeuristic NzGuillotineImageAtlas::GetRectSplitHeuristic() const
{
	return m_rectSplitHeuristic;
}

NzAbstractImage* NzGuillotineImageAtlas::GetLayer(unsigned int layerIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (layerIndex >= m_layers.size())
	{
		NazaraError("Layer index out of range (" + NzString::Number(layerIndex) + " >= " + NzString::Number(m_layers.size()) + ')');
		return nullptr;
	}
	#endif

	Layer& layer = m_layers[layerIndex];
	ProcessGlyphQueue(layer);

	return layer.image.get();
}

unsigned int NzGuillotineImageAtlas::GetLayerCount() const
{
	return m_layers.size();
}

bool NzGuillotineImageAtlas::Insert(const NzImage& image, NzRectui* rect, bool* flipped, unsigned int* layerIndex)
{
	unsigned int maxAtlasSize = GetMaxAtlasSize();

	if (m_layers.empty())
	{
		// On créé une première couche s'il n'y en a pas
		m_layers.resize(1);
		Layer& layer = m_layers.back();
		layer.binPack.Reset(s_atlasStartSize, s_atlasStartSize);
	}

	// Cette fonction ne fait qu'insérer un rectangle de façon virtuelle, l'insertion des images se fait après
	for (unsigned int i = 0; i < m_layers.size(); ++i)
	{
		Layer& layer = m_layers[i];

		// Une fois qu'un certain nombre de rectangles ont étés libérés d'une couche, on fusionne les rectangles libres
		if (layer.freedRectangles > 10) // Valeur totalement arbitraire
		{
			while (layer.binPack.MergeFreeRectangles()); // Tant qu'une fusion est possible
			layer.freedRectangles = 0; // Et on repart de zéro
		}

		if (layer.binPack.Insert(rect, flipped, 1, false, m_rectChoiceHeuristic, m_rectSplitHeuristic))
		{
			// Insertion réussie dans l'une des couches, on place le glyphe en file d'attente
			layer.queuedGlyphs.resize(layer.queuedGlyphs.size()+1);
			QueuedGlyph& glyph = layer.queuedGlyphs.back();
			glyph.flipped = *flipped;
			glyph.image = image; // Merci le Copy-On-Write
			glyph.rect = *rect;

			*layerIndex = i;
			return true;
		}
		else if (i == m_layers.size() - 1) // Dernière itération ?
		{
			// Dernière couche, et le glyphe ne rentre pas, peut-on agrandir la taille de l'image ?
			unsigned int size = layer.binPack.GetWidth(); // l'image étant carrée, on ne teste qu'une dimension
			if (size < maxAtlasSize)
			{
				// On peut encore agrandir la couche
				size = std::min(size*2, maxAtlasSize);
				layer.binPack.Expand(size, size);

				// On relance la boucle sur la nouvelle dernière couche
				i--;
			}
			else
			{
				// On ne peut plus agrandir la dernière couche, il est temps d'en créer une nouvelle
				m_layers.resize(m_layers.size() + 1);
				Layer& newLayer = m_layers.back();

				newLayer.binPack.Reset(s_atlasStartSize, s_atlasStartSize);

				// On laisse la boucle insérer toute seule le rectangle à la prochaine itération
			}
		}
	}

	return false; // Normalement impossible
}

void NzGuillotineImageAtlas::SetRectChoiceHeuristic(NzGuillotineBinPack::FreeRectChoiceHeuristic heuristic)
{
	m_rectChoiceHeuristic = heuristic;
}

void NzGuillotineImageAtlas::SetRectSplitHeuristic(NzGuillotineBinPack::GuillotineSplitHeuristic heuristic)
{
	m_rectSplitHeuristic = heuristic;
}

unsigned int NzGuillotineImageAtlas::GetMaxAtlasSize() const
{
	return 8192; // Valeur totalement arbitraire
}

void NzGuillotineImageAtlas::ProcessGlyphQueue(Layer& layer) const
{
	std::vector<nzUInt8> pixelBuffer;

	// On s'assure que l'image est de la bonne taille
	NzVector2ui binPackSize(layer.binPack.GetSize());
	NzVector2ui imageSize((layer.image) ? layer.image->GetSize() : NzVector3ui(0U));
	if (binPackSize != imageSize)
		ResizeImage(layer, binPackSize);

	for (QueuedGlyph& glyph : layer.queuedGlyphs)
	{
		unsigned int glyphWidth = glyph.image.GetWidth();
		unsigned int glyphHeight = glyph.image.GetHeight();

		// Calcul de l'éventuel padding (pixels de contour)
		unsigned int paddingX;
		unsigned int paddingY;
		if (glyph.flipped)
		{
			paddingX = (glyph.rect.height - glyphWidth)/2;
			paddingY = (glyph.rect.width - glyphHeight)/2;
		}
		else
		{
			paddingX = (glyph.rect.width - glyphWidth)/2;
			paddingY = (glyph.rect.height - glyphHeight)/2;
		}

		if (paddingX > 0 || paddingY > 0)
		{
			// On remplit les contours
			pixelBuffer.resize(glyph.rect.width * glyph.rect.height);
			std::memset(pixelBuffer.data(), 0, glyph.rect.width*glyph.rect.height*sizeof(nzUInt8));

			layer.image->Update(pixelBuffer.data(), glyph.rect);
		}

		const nzUInt8* pixels;
		// On copie le glyphe dans l'atlas
		if (glyph.flipped)
		{
			pixelBuffer.resize(glyphHeight * glyphWidth);

			// On tourne le glyphe pour qu'il rentre dans le rectangle
			const nzUInt8* src = glyph.image.GetConstPixels();
			nzUInt8* ptr = pixelBuffer.data();

			unsigned int lineStride = glyphWidth*sizeof(nzUInt8); // BPP = 1
			src += lineStride-1; // Départ en haut à droite
			for (unsigned int x = 0; x < glyphWidth; ++x)
			{
				for (unsigned int y = 0; y < glyphHeight; ++y)
				{
					*ptr++ = *src;
					src += lineStride;
				}

				src -= glyphHeight*lineStride + 1;
			}

			pixels = pixelBuffer.data();
			std::swap(glyphWidth, glyphHeight);
		}
		else
			pixels = glyph.image.GetConstPixels();

		layer.image->Update(pixels, NzRectui(glyph.rect.x + paddingX, glyph.rect.y + paddingY, glyphWidth, glyphHeight), 0, glyphWidth, glyphHeight);
		glyph.image.Destroy(); // On libère l'image dès que possible (pour réduire la consommation)
	}

	layer.queuedGlyphs.clear();
}

bool NzGuillotineImageAtlas::ResizeImage(Layer& layer, const NzVector2ui& size) const
{
	NzImage newImage(nzImageType_2D, nzPixelFormat_A8, size.x, size.y);
	if (layer.image)
	{
		NzImage& image = *static_cast<NzImage*>(layer.image.get());
		newImage.Copy(image, NzRectui(size), NzVector2ui(0, 0)); // Copie des anciennes données

		image = std::move(newImage);
	}
	else
		layer.image.reset(new NzImage(std::move(newImage)));

	return true;
}
