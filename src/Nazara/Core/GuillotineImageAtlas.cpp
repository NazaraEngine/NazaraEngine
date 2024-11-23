// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/GuillotineImageAtlas.hpp>
#include <Nazara/Core/PixelFormat.hpp>

namespace Nz
{
	void GuillotineImageAtlas::Clear()
	{
		m_layers.clear();
		OnAtlasCleared(this);
	}

	void GuillotineImageAtlas::Free(SparsePtr<const Rectui> rects, SparsePtr<std::size_t> layers, std::size_t count)
	{
		for (std::size_t i = 0; i < count; ++i)
		{
			NazaraAssertMsg(layers[i] < m_layers.size(), "Rectangle #%zu belongs to an out-of-bounds layer (%zu >= %zu)", i, layers[i], m_layers.size());

			m_layers[layers[i]].binPack.FreeRectangle(rects[i]);
			m_layers[layers[i]].freedRectangles++;
		}
	}

	AbstractImage* GuillotineImageAtlas::GetLayer(std::size_t layerIndex) const
	{
		NazaraAssertMsg(layerIndex < m_layers.size(), "layer index out of range (%zu >= %zu)", layerIndex, m_layers.size());

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
		NazaraAssertMsg(layerIndex || m_layers.size() <= 1, "invalid layerIndex on a multi-layer atlas");

		// Ensure there's at least one layer before inserting
		if (m_layers.empty())
		{
			Vector2ui32 layerSize(m_initialLayerSize);

			Layer layer;
			if (!ResizeLayer(layer, layerSize))
			{
				NazaraError("failed to allocate initial layer");
				return false;
			}

			layer.binPack.Expand(layerSize);
			m_layers.emplace_back(std::move(layer));
		}

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
				glyph.flipped = (flipped) ? *flipped : false; //< if flipped is nullptr, flipping is forbidden
				glyph.rect = *rect;

				// Image implements Copy-On-Write, so if no conversion is required this will not copy
				glyph.image = image;
				if (!glyph.image.Convert(m_pixelFormat))
				{
					layer.binPack.FreeRectangle(glyph.rect);
					layer.freedRectangles++;

					NazaraError("failed to convert image to {0}", PixelFormatInfo::GetName(m_pixelFormat));
					return false;
				}

				if (layerIndex)
					*layerIndex = i;

				return true;
			}
			else if (i == m_layers.size() - 1)
			{
				// Last layer and glyph can't be inserted, try to double the layer size
				Vector2ui newSize = layer.binPack.GetSize() * 2;

				// Limit image atlas size to prevent allocating too much contiguous memory blocks
				if (newSize.x <= m_maxLayerSize && newSize.y <= m_maxLayerSize && ResizeLayer(layer, newSize))
				{
					// Atlas has been enlarged successfully, re-run iteration
					layer.binPack.Expand(newSize);

					i--;
				}
				else
				{
					if (!layerIndex)
					{
						NazaraError("atlas cannot allocate a new layer since layerIndex output pointer is null");
						return false;
					}

					// Atlas cannot be enlarged, make a new layer
					newSize = { m_initialLayerSize, m_initialLayerSize };

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

		NAZARA_UNREACHABLE();
	}

	std::shared_ptr<AbstractImage> GuillotineImageAtlas::ResizeImage(const std::shared_ptr<AbstractImage>& oldImage, const Vector2ui& size) const
	{
		std::shared_ptr<Image> newImage = std::make_shared<Image>(ImageType::E2D, m_pixelFormat, size.x, size.y);
		if (oldImage)
		{
			const Image& srcImage = SafeCast<const Image&>(*oldImage);
			if (srcImage.IsLevelAllocated(0))
				newImage->Copy(srcImage, Boxui(Vector3ui::Zero(), oldImage->GetSize()), Vector2ui(0, 0));
		}

		return newImage;
	}

	bool GuillotineImageAtlas::ResizeLayer(Layer& layer, const Vector2ui& size)
	{
		std::shared_ptr<AbstractImage> newImage = ResizeImage(layer.image, size);
		if (!newImage)
			return false; // Allocation failed

		if (newImage == layer.image)
			return true; // The image object hasn't changed

		// Image object did change, notify and store the new one before signaling
		std::shared_ptr<AbstractImage> previousImage = std::move(layer.image);
		layer.image = std::move(newImage);

		OnAtlasLayerChange(this, previousImage.get(), layer.image.get());
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
				pixelBuffer.clear();
				pixelBuffer.resize(PixelFormatInfo::ComputeSize(m_pixelFormat, glyph.rect.width, glyph.rect.height, 1));

				layer.image->Update(pixelBuffer.data(), glyph.rect);
			}

			const UInt8* pixels;
			// Copy the glyph to the atlas
			if (glyph.flipped)
			{
				pixelBuffer.resize(glyphHeight * glyphWidth);

				// Rotate the glyph
				const UInt8* src = glyph.image.GetConstPixels();
				NazaraAssertMsg(src, "glyph image has no pixels");
				UInt8* ptr = pixelBuffer.data();

				unsigned int lineStride = glyphWidth * PixelFormatInfo::GetBytesPerPixel(m_pixelFormat);
				src += lineStride - 1; // Top-right
				for (unsigned int x = 0; x < glyphWidth; ++x)
				{
					for (unsigned int y = 0; y < glyphHeight; ++y)
					{
						*ptr++ = *src;
						src += lineStride;
					}

					src -= glyphHeight * lineStride + 1;
				}

				pixels = pixelBuffer.data();
				std::swap(glyphWidth, glyphHeight);
			}
			else
			{
				pixels = glyph.image.GetConstPixels();
				NazaraAssertMsg(pixels, "glyph image has no pixels");
			}

			layer.image->Update(pixels, Rectui(glyph.rect.x + paddingX, glyph.rect.y + paddingY, glyphWidth, glyphHeight), 0, glyphWidth, glyphHeight);
			glyph.image.Destroy(); // Free the image as soon as possible (to reduce memory usage)
		}

		layer.queuedGlyphs.clear();
	}
}
