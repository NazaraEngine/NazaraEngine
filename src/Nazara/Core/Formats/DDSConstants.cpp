// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/DDSConstants.hpp>

namespace Nz
{
	bool Deserialize(SerializationContext& context, DDSHeader* header)
	{
		if (!Deserialize(context, &header->size))
			return false;
		if (!Deserialize(context, &header->flags))
			return false;
		if (!Deserialize(context, &header->height))
			return false;
		if (!Deserialize(context, &header->width))
			return false;
		if (!Deserialize(context, &header->pitch))
			return false;
		if (!Deserialize(context, &header->depth))
			return false;
		if (!Deserialize(context, &header->levelCount))
			return false;

		for (unsigned int i = 0; i < CountOf(header->reserved1); ++i)
		{
			if (!Deserialize(context, &header->reserved1[i]))
				return false;
		}

		if (!Deserialize(context, &header->format))
			return false;

		for (unsigned int i = 0; i < CountOf(header->ddsCaps); ++i)
		{
			if (!Deserialize(context, &header->ddsCaps[i]))
				return false;
		}

		if (!Deserialize(context, &header->reserved2))
			return false;

		return true;
	}

	bool Deserialize(SerializationContext& context, DDSHeaderDX10Ext* header)
	{
		UInt32 enumValue;

		if (!Deserialize(context, &enumValue))
			return false;
		header->dxgiFormat = static_cast<DXGI_FORMAT>(enumValue);

		if (!Deserialize(context, &enumValue))
			return false;
		header->resourceDimension = static_cast<D3D10_RESOURCE_DIMENSION>(enumValue);

		if (!Deserialize(context, &header->miscFlag))
			return false;
		if (!Deserialize(context, &header->arraySize))
			return false;
		if (!Deserialize(context, &header->reserved))
			return false;

		return true;
	}

	bool Deserialize(SerializationContext& context, DDSPixelFormat* pixelFormat)
	{
		if (!Deserialize(context, &pixelFormat->size))
			return false;
		if (!Deserialize(context, &pixelFormat->flags))
			return false;
		if (!Deserialize(context, &pixelFormat->fourCC))
			return false;
		if (!Deserialize(context, &pixelFormat->bpp))
			return false;
		if (!Deserialize(context, &pixelFormat->redMask))
			return false;
		if (!Deserialize(context, &pixelFormat->greenMask))
			return false;
		if (!Deserialize(context, &pixelFormat->blueMask))
			return false;
		if (!Deserialize(context, &pixelFormat->alphaMask))
			return false;

		return true;
	}
}
