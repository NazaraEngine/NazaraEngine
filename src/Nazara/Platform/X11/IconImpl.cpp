// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/X11/IconImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Platform/X11/Display.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	IconImpl::IconImpl()
	{
		ScopedXCBConnection connection;

		m_iconPixmap.Connect(connection);
		m_maskPixmap.Connect(connection);
	}

	bool IconImpl::Create(const Image& icon)
	{
		Image iconImage(icon); // Vive le COW
		if (!iconImage.Convert(Nz::PixelFormatType_BGRA8))
		{
			NazaraError("Failed to convert icon to BGRA8");
			return false;
		}

		auto width = iconImage.GetWidth();
		auto height = iconImage.GetHeight();

		ScopedXCBConnection connection;

		xcb_screen_t* screen = X11::XCBDefaultScreen(connection);

		if (!m_iconPixmap.Create(
			screen->root_depth,
			screen->root,
			width,
			height))
		{
			NazaraError("Failed to create icon pixmap");
			return false;
		}

		CallOnExit onExit([this](){
			Destroy();
		});

		XCBGContext iconGC(connection);

		if (!iconGC.Create(
			m_iconPixmap,
			0,
			nullptr))
		{
			NazaraError("Failed to create icon gc");
			return false;
		}

		if (!X11::CheckCookie(
			connection,
			xcb_put_image(
				connection,
				XCB_IMAGE_FORMAT_Z_PIXMAP,
				m_iconPixmap,
				iconGC,
				width,
				height,
				0,
				0,
				0,
				screen->root_depth,
				width * height * 4,
				iconImage.GetConstPixels()
			)))
		{
			NazaraError("Failed to put image for icon");
			return false;
		}

		// Create the mask pixmap (must have 1 bit depth)
		std::size_t pitch = (width + 7) / 8;
		static std::vector<UInt8> maskPixels(pitch * height, 0);
		for (std::size_t j = 0; j < height; ++j)
		{
			for (std::size_t i = 0; i < pitch; ++i)
			{
				for (std::size_t k = 0; k < 8; ++k)
				{
					if (i * 8 + k < width)
					{
						UInt8 opacity = (iconImage.GetConstPixels()[(i * 8 + k + j * width) * 4 + 3] > 0) ? 1 : 0;
						maskPixels[i + j * pitch] |= (opacity << k);
					}
				}
			}
		}

		if (!m_maskPixmap.CreatePixmapFromBitmapData(
			X11::XCBDefaultRootWindow(connection),
			reinterpret_cast<uint8_t*>(&maskPixels[0]),
			width,
			height,
			1,
			0,
			1,
			nullptr))
		{
			NazaraError("Failed to create mask pixmap for icon");
			return false;
		}

		onExit.Reset();

		return true;
	}

	void IconImpl::Destroy()
	{
		m_iconPixmap.Destroy();
		m_maskPixmap.Destroy();
	}

	xcb_pixmap_t IconImpl::GetIcon()
	{
		return m_iconPixmap;
	}

	xcb_pixmap_t IconImpl::GetMask()
	{
		return m_maskPixmap;
	}
}
