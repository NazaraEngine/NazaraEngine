// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/GuillotineImageAtlas.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr Vector2ui s_guillotineAtlasStartSize(512);
	}

	GuillotineImageAtlas::GuillotineImageAtlas() :
	m_rectChoiceHeuristic(GuillotineBinPack::RectBestAreaFit),
	m_rectSplitHeuristic(GuillotineBinPack::SplitMinimizeArea),
	m_maxLayerSize(16384)
	{
	}

	void GuillotineImageAtlas::Clear()
	{
		m_layers.clear();
		OnAtlasCleared(this);
	}

	void GuillotineImageAtlas::Free(SparsePtr<const Rectui> rects, SparsePtr<std::size_t> layers, std::size_t count)
	{
		for (std::size_t i = 0; i < count; ++i)
		{
			NazaraAssertFmt(layers[i] < m_layers.size(), "Rectangle #{0} belongs to an out-of-bounds layer ({1} >= {2})", i, layers[i], m_layers.size());

			m_layers[layers[i]].binPack.FreeRectangle(rects[i]);
			m_layers[layers[i]].freedRectangles++;
		}
	}

	unsigned int GuillotineImageAtlas::GetMaxLayerSize() const
	{
		return m_maxLayerSize;
	}

	GuillotineBinPack::FreeRectChoiceHeuristic GuillotineImageAtlas::GetRectChoiceHeuristic() const
	{
		return m_rectChoiceHeuristic;
	}

	GuillotineBinPack::GuillotineSplitHeuristic GuillotineImageAtlas::GetRectSplitHeuristic() const
	{
		return m_rectSplitHeuristic;
	}

	AbstractImage* GuillotineImageAtlas::GetLayer(std::size_t layerIndex) const
	{
		NazaraAssertFmt(layerIndex < m_layers.size(), "layer index out of range ({0} >= {1})", layerIndex, m_layers.size());

		Layer& layer = m_layers[layerIndex];
		ProcessGlyphQueue(layer);

		return layer.image.get();
	}

	std::size_t GuillotineImageAtlas::GetLayerCount() const
	{
		return m_layers.size();
	}

	DataStoreFlags GuillotineImageAtlas::GetStorage() const
	{
		return DataStorage::Software;
	}

	bool GuillotineImageAtlas::Insert(const Image& image, Rectui* rect, bool* flipped, std::size_t* layerIndex)
	{
		// Ensure there's at least one layer before inserting
		if (m_layers.empty())
			m_layers.emplace_back();

		// Reserve some space for that rectangle (pixel copy only happens in ProcessGlyphQueue)
		for (std::size_t i = 0; i < m_layers.size(); ++i)
		{
			Layer& layer = m_layers[i];

			// Try to reduce fragmentation by merging free rectangles if at least X rectangles were freed before inserting
			if (layer.freedRectangles > 10)
			{
				while (layer.binPack.MergeFreeRectangles());
				layer.freedRectangles = 0;
			}

			if (layer.binPack.Insert(rect, flipped, 1, false, m_rectChoiceHeuristic, m_rectSplitHeuristic))
			{
				// Found some space, queue glyph copy
				layer.queuedGlyphs.resize(layer.queuedGlyphs.size()+1);
				QueuedGlyph& glyph = layer.queuedGlyphs.back();
				glyph.flipped = *flipped;
				glyph.image = image; // Copy-On-Write
				glyph.rect = *rect;

				*layerIndex = i;
				return true;
			}
			else if (i == m_layers.size() - 1)
			{
				// Last layer and glyph can't be inserted, try to double the layer size
				Vector2ui newSize = layer.binPack.GetSize() * 2;
				if (newSize == Vector2ui::Zero())
					newSize = s_guillotineAtlasStartSize;

				// Limit image atlas size to prevent allocating too much contiguous memory blocks
				if (newSize.x <= m_maxLayerSize && newSize.y <= m_maxLayerSize && ResizeLayer(layer, newSize))
				{
					// Atlas has been enlarged successfully, re-run iteration
					layer.binPack.Expand(newSize);

					i--;
				}
				else
				{
					// Atlas cannot be enlarged, make a new layer
					newSize = s_guillotineAtlasStartSize;

					Layer newLayer;
					if (!ResizeLayer(newLayer, newSize))
					{
						NazaraError("failed to allocate new layer, we are probably out of memory");
						return false;
					}

					newLayer.binPack.Reset(newSize);

					m_layers.emplace_back(std::move(newLayer));

					// This new layer will be processed on next iteration
				}
			}
		}

		// It shouldn't be possible to get here (TODO: Add NAZARA_UNREACHEABLE)
		NazaraInternalError("unknown error");
		return false;
	}

	void GuillotineImageAtlas::SetMaxLayerSize(unsigned int maxLayerSize)
	{
		m_maxLayerSize = maxLayerSize;
	}

	void GuillotineImageAtlas::SetRectChoiceHeuristic(GuillotineBinPack::FreeRectChoiceHeuristic heuristic)
	{
		m_rectChoiceHeuristic = heuristic;
	}

	void GuillotineImageAtlas::SetRectSplitHeuristic(GuillotineBinPack::GuillotineSplitHeuristic heuristic)
	{
		m_rectSplitHeuristic = heuristic;
	}

	std::shared_ptr<AbstractImage> GuillotineImageAtlas::ResizeImage(const std::shared_ptr<AbstractImage>& oldImage, const Vector2ui& size) const
	{
		std::shared_ptr<Image> newImage = std::make_shared<Image>(ImageType::E2D, PixelFormat::A8, size.x, size.y);
		if (oldImage)
			newImage->Copy(static_cast<Image&>(*oldImage), Boxui(Vector3ui::Zero(), oldImage->GetSize()), Vector2ui(0, 0)); // Copie des anciennes données

		return newImage;
	}

	bool GuillotineImageAtlas::ResizeLayer(Layer& layer, const Vector2ui& size)
	{
		std::shared_ptr<AbstractImage> newImage = ResizeImage(layer.image, size);
		if (!newImage)
			return false; // Allocation failed

		if (newImage == layer.image)
			return true; // The image object hasn't changed

		// Image object did change, notify and store the new one
		OnAtlasLayerChange(this, layer.image.get(), newImage.get());

		layer.image = std::move(newImage);
		return true;
	}

	void GuillotineImageAtlas::ProcessGlyphQueue(Layer& layer) const
	{
		std::vector<UInt8> pixelBuffer;

		for (QueuedGlyph& glyph : layer.queuedGlyphs)
		{
			unsigned int glyphWidth = glyph.image.GetWidth();
			unsigned int glyphHeight = glyph.image.GetHeight();

			// Compute padding
			unsigned int paddingX;
			unsigned int paddingY;
			if (glyph.flipped)
			{
				paddingX = (glyph.rect.height - glyphWidth) / 2;
				paddingY = (glyph.rect.width - glyphHeight) / 2;
			}
			else
			{
				paddingX = (glyph.rect.width - glyphWidth) / 2;
				paddingY = (glyph.rect.height - glyphHeight) / 2;
			}

			if (paddingX > 0 || paddingY > 0)
			{
				// Prefill the rectangle if we have some padding
				pixelBuffer.resize(glyph.rect.width * glyph.rect.height);
				std::memset(pixelBuffer.data(), 0, glyph.rect.width * glyph.rect.height * sizeof(UInt8));

				layer.image->Update(pixelBuffer.data(), glyph.rect);
			}

			const UInt8* pixels;
			// Copy the glyph to the atlas
			if (glyph.flipped)
			{
				pixelBuffer.resize(glyphHeight * glyphWidth);

				// Rotate the glyph
				const UInt8* src = glyph.image.GetConstPixels();
				UInt8* ptr = pixelBuffer.data();

				unsigned int lineStride = glyphWidth * sizeof(UInt8); // BPP = 1
				src += lineStride-1; // Top-right
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

			layer.image->Update(pixels, Rectui(glyph.rect.x + paddingX, glyph.rect.y + paddingY, glyphWidth, glyphHeight), 0, glyphWidth, glyphHeight);
			glyph.image.Destroy(); // Free the image as soon as possible (to reduce memory usage)
		}

		layer.queuedGlyphs.clear();
	}
}
