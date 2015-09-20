// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/X11/CursorImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/X11/Display.hpp>
#include <xcb/xcb_image.h>
#include <xcb/xcb_renderutil.h>
#include <Nazara/Utility/Debug.hpp>

bool NzCursorImpl::Create(const NzImage& cursor, int hotSpotX, int hotSpotY)
{
	NzImage cursorImage(cursor); // Vive le COW
	if (!cursorImage.Convert(nzPixelFormat_BGRA8))
	{
		NazaraError("Failed to convert cursor to BGRA8");
		return false;
	}

	auto width = cursorImage.GetWidth();
	auto height = cursorImage.GetHeight();

	NzScopedXCBConnection connection;

	xcb_screen_t* screen = X11::XCBDefaultScreen(connection);

	NzScopedXCB<xcb_generic_error_t> error(nullptr);
	NzScopedXCB<xcb_render_query_pict_formats_reply_t> formatsReply = xcb_render_query_pict_formats_reply(
		connection,
		xcb_render_query_pict_formats(connection),
		&error);

	if (!formatsReply || error)
	{
		NazaraError("Failed to get pict formats");
		return false;
	}

	xcb_render_pictforminfo_t* fmt = xcb_render_util_find_standard_format(
		formatsReply.get(),
		XCB_PICT_STANDARD_ARGB_32);

	if (!fmt)
	{
		NazaraError("Failed to find format PICT_STANDARD_ARGB_32");
		return false;
	}

	xcb_image_t* xi = xcb_image_create(
		width, height,
		XCB_IMAGE_FORMAT_Z_PIXMAP,
		32, 32, 32, 32,
		XCB_IMAGE_ORDER_LSB_FIRST,
		XCB_IMAGE_ORDER_MSB_FIRST,
		0, 0, 0);

	if (!xi)
	{
		NazaraError("Failed to create image for cursor");
		return false;
	}

	std::unique_ptr<uint8_t[]> data(new uint8_t[xi->stride * height]);

	if (!data)
	{
		xcb_image_destroy(xi);
		NazaraError("Failed to allocate memory for cursor image");
		return false;
	}

	xi->data = data.get();

	std::copy(cursorImage.GetConstPixels(), cursorImage.GetConstPixels() + cursorImage.GetBytesPerPixel() * width * height, xi->data);

	xcb_render_picture_t pic = XCB_NONE;

	NzCallOnExit onExit([&](){
		xcb_image_destroy(xi);
		if (pic != XCB_NONE)
			xcb_render_free_picture(connection, pic);
	});

	NzXCBPixmap pix(connection);
	if (!pix.Create(32, screen->root, width, height))
	{
		NazaraError("Failed to create pixmap for cursor");
		return false;
	}

	pic = xcb_generate_id(connection);
	if (!X11::CheckCookie(
		connection,
		xcb_render_create_picture(
			connection,
			pic,
			pix,
			fmt->id,
			0,
			nullptr
		)))
	{
		NazaraError("Failed to create render picture for cursor");
		return false;
	}

	NzXCBGContext gc(connection);
	if (!gc.Create(pix, 0, nullptr))
	{
		NazaraError("Failed to create gcontext for cursor");
		return false;
	}

	if (!X11::CheckCookie(
		connection,
		xcb_image_put(
			connection,
			pix,
			gc,
			xi,
			0, 0,
			0
		)))
	{
		NazaraError("Failed to put image for cursor");
		return false;
	}

	m_cursor = xcb_generate_id(connection);
	if (!X11::CheckCookie(
		connection,
		xcb_render_create_cursor(
			connection,
			m_cursor,
			pic,
			hotSpotX, hotSpotY
		)))
	{
		NazaraError("Failed to create cursor");
		return false;
	}

	return true;
}

void NzCursorImpl::Destroy()
{
	NzScopedXCBConnection connection;

	xcb_free_cursor(connection, m_cursor);
	m_cursor = 0;
}

xcb_cursor_t NzCursorImpl::GetCursor()
{
	return m_cursor;
}
