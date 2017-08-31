// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/DDSConstants.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool Unserialize(SerializationContext& context, DDSHeader* header)
	{
		if (!Unserialize(context, &header->size))
			return false;
		if (!Unserialize(context, &header->flags))
			return false;
		if (!Unserialize(context, &header->height))
			return false;
		if (!Unserialize(context, &header->width))
			return false;
		if (!Unserialize(context, &header->pitch))
			return false;
		if (!Unserialize(context, &header->depth))
			return false;
		if (!Unserialize(context, &header->levelCount))
			return false;

		for (unsigned int i = 0; i < CountOf(header->reserved1); ++i)
		{
			if (!Unserialize(context, &header->reserved1[i]))
				return false;
		}

		if (!Unserialize(context, &header->format))
			return false;

		for (unsigned int i = 0; i < CountOf(header->ddsCaps); ++i)
		{
			if (!Unserialize(context, &header->ddsCaps[i]))
				return false;
		}

		if (!Unserialize(context, &header->reserved2))
			return false;

		return true;
	}

	bool Unserialize(SerializationContext& context, DDSHeaderDX10Ext* header)
	{
		UInt32 enumValue;

		if (!Unserialize(context, &enumValue))
			return false;
		header->dxgiFormat = static_cast<DXGI_FORMAT>(enumValue);

		if (!Unserialize(context, &enumValue))
			return false;
		header->resourceDimension = static_cast<D3D10_RESOURCE_DIMENSION>(enumValue);

		if (!Unserialize(context, &header->miscFlag))
			return false;
		if (!Unserialize(context, &header->arraySize))
			return false;
		if (!Unserialize(context, &header->reserved))
			return false;

		return true;
	}

	bool Unserialize(SerializationContext& context, DDSPixelFormat* pixelFormat)
	{
		if (!Unserialize(context, &pixelFormat->size))
			return false;
		if (!Unserialize(context, &pixelFormat->flags))
			return false;
		if (!Unserialize(context, &pixelFormat->fourCC))
			return false;
		if (!Unserialize(context, &pixelFormat->bpp))
			return false;
		if (!Unserialize(context, &pixelFormat->redMask))
			return false;
		if (!Unserialize(context, &pixelFormat->greenMask))
			return false;
		if (!Unserialize(context, &pixelFormat->blueMask))
			return false;
		if (!Unserialize(context, &pixelFormat->alphaMask))
			return false;

		return true;
	}
}
