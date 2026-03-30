// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/DDSConstants.hpp>

namespace Nz
{
	bool Deserialize(SerializationContext& context, DDSHeader* header, TypeTag<DDSHeader>)
	{
		if (!Deserialize(context, &header->size))
			return false;
		if (!Deserialize(context, &header->flags))
			return false;
		if (!Deserialize(context, &header->height))
			return false;
		if (!Deserialize(context, &header->width))
			return false;
		if (!Deserialize(context, &header->pitchOrLinearSize))
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

	bool Deserialize(SerializationContext& context, DDSHeaderDX10Ext* header, TypeTag<DDSHeaderDX10Ext>)
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
		if (!Deserialize(context, &header->miscFlags2))
			return false;

		return true;
	}

	bool Deserialize(SerializationContext& context, DDSPixelFormat* pixelFormat, TypeTag<DDSPixelFormat>)
	{
		if (!Deserialize(context, &pixelFormat->size))
			return false;
		if (!Deserialize(context, &pixelFormat->flags))
			return false;
		if (!Deserialize(context, &pixelFormat->fourCC))
			return false;
		if (!Deserialize(context, &pixelFormat->rgbBitCount))
			return false;
		if (!Deserialize(context, &pixelFormat->rBitMask))
			return false;
		if (!Deserialize(context, &pixelFormat->gBitMask))
			return false;
		if (!Deserialize(context, &pixelFormat->bBitMask))
			return false;
		if (!Deserialize(context, &pixelFormat->aBitMask))
			return false;

		return true;
	}

	bool Serialize(SerializationContext& context, const DDSHeader& header, TypeTag<DDSHeader>)
	{
		if (!Serialize(context, header.size))
			return false;
		if (!Serialize(context, header.flags))
			return false;
		if (!Serialize(context, header.height))
			return false;
		if (!Serialize(context, header.width))
			return false;
		if (!Serialize(context, header.pitchOrLinearSize))
			return false;
		if (!Serialize(context, header.depth))
			return false;
		if (!Serialize(context, header.levelCount))
			return false;

		for (unsigned int i = 0; i < CountOf(header.reserved1); ++i)
		{
			if (!Serialize(context, header.reserved1[i]))
				return false;
		}

		if (!Serialize(context, header.format))
			return false;

		for (unsigned int i = 0; i < CountOf(header.ddsCaps); ++i)
		{
			if (!Serialize(context, header.ddsCaps[i]))
				return false;
		}

		if (!Serialize(context, header.reserved2))
			return false;

		return true;
	}

	bool Serialize(SerializationContext& context, const DDSHeaderDX10Ext& header, TypeTag<DDSHeaderDX10Ext>)
	{
		UInt32 enumValue = static_cast<UInt32>(header.dxgiFormat);
		if (!Serialize(context, enumValue))
			return false;

		enumValue = static_cast<UInt32>(header.resourceDimension);
		if (!Serialize(context, enumValue))
			return false;

		if (!Serialize(context, header.miscFlag))
			return false;
		if (!Serialize(context, header.arraySize))
			return false;
		if (!Serialize(context, header.miscFlags2))
			return false;

		return true;
	}

	bool Serialize(SerializationContext& context, const DDSPixelFormat& pixelFormat, TypeTag<DDSPixelFormat>)
	{
		if (!Serialize(context, pixelFormat.size))
			return false;
		if (!Serialize(context, pixelFormat.flags))
			return false;
		if (!Serialize(context, pixelFormat.fourCC))
			return false;
		if (!Serialize(context, pixelFormat.rgbBitCount))
			return false;
		if (!Serialize(context, pixelFormat.rBitMask))
			return false;
		if (!Serialize(context, pixelFormat.gBitMask))
			return false;
		if (!Serialize(context, pixelFormat.bBitMask))
			return false;
		if (!Serialize(context, pixelFormat.aBitMask))
			return false;

		return true;
	}
}
