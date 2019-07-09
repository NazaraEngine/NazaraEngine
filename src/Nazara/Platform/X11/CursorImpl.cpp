// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/X11/CursorImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Platform/X11/Display.hpp>
#include <Nazara/Platform/X11/ScopedXCB.hpp>
#include <xcb/xcb_image.h>

// Some older versions of xcb/util-renderutil (notably the one available on Travis CI) use `template` as an argument name
// This is a fixed bug (https://cgit.freedesktop.org/xcb/util-renderutil/commit/?id=8d15acc45a47dc4c922eee5b99885db42bc62c17) but until Travis-CI
// has upgraded their Ubuntu version, I'm forced to use this ugly trick.
#define template ptemplate
extern "C"
{
	#include <xcb/xcb_renderutil.h>
}
#undef template

#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	bool CursorImpl::Create(const Image& cursor, int hotSpotX, int hotSpotY)
	{
		Image cursorImage(cursor); // Vive le COW
		if (!cursorImage.Convert(Nz::PixelFormatType_BGRA8))
		{
			NazaraError("Failed to convert cursor to BGRA8");
			return false;
		}

		auto width = cursorImage.GetWidth();
		auto height = cursorImage.GetHeight();

		ScopedXCBConnection connection;

		xcb_screen_t* screen = X11::XCBDefaultScreen(connection);

		ScopedXCB<xcb_generic_error_t> error(nullptr);
		ScopedXCB<xcb_render_query_pict_formats_reply_t> formatsReply = xcb_render_query_pict_formats_reply(
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

		CallOnExit onExit([&](){
			xcb_image_destroy(xi);
			if (pic != XCB_NONE)
				xcb_render_free_picture(connection, pic);
		});

		XCBPixmap pix(connection);
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

		XCBGContext gc(connection);
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

	bool CursorImpl::Create(SystemCursor cursor)
	{
		ScopedXCBConnection connection;
		xcb_screen_t* screen = X11::XCBDefaultScreen(connection);

		const char* cursorName = s_systemCursorIds[cursor];
		if (cursorName)
		{
			if (xcb_cursor_context_new(connection, screen, &m_cursorContext) >= 0)
				m_cursor = xcb_cursor_load_cursor(m_cursorContext, cursorName);
			else
			{
				NazaraError("Failed to create cursor context");
				return false;
			}
		}
		else
			m_cursor = s_hiddenCursor;

		return true;
	}

	void CursorImpl::Destroy()
	{
		ScopedXCBConnection connection;

		xcb_free_cursor(connection, m_cursor);
		if (m_cursorContext)
			xcb_cursor_context_free(m_cursorContext);
	}

	xcb_cursor_t CursorImpl::GetCursor()
	{
		return m_cursor;
	}

	bool CursorImpl::Initialize()
	{
		ScopedXCBConnection connection;
		XCBPixmap cursorPixmap(connection);

		xcb_window_t window = X11::XCBDefaultRootWindow(connection);

		if (!cursorPixmap.Create(1, window, 1, 1))
		{
			NazaraError("Failed to create pixmap for hidden cursor");
			return false;
		}

		s_hiddenCursor = xcb_generate_id(connection);

		// Create the cursor, using the pixmap as both the shape and the mask of the cursor
		if (!X11::CheckCookie(
			connection, xcb_create_cursor(connection,
			                              s_hiddenCursor,
			                              cursorPixmap,
			                              cursorPixmap,
			                              0, 0, 0, // Foreground RGB color
			                              0, 0, 0, // Background RGB color
			                              0,       // X
			                              0        // Y
			                              )))
		{
			NazaraError("Failed to create hidden cursor");
			return false;
		}

		return true;
	}

	void CursorImpl::Uninitialize()
	{
		if (s_hiddenCursor)
		{
			ScopedXCBConnection connection;
			xcb_free_cursor(connection, s_hiddenCursor);
			s_hiddenCursor = 0;
		}
	}

	xcb_cursor_t CursorImpl::s_hiddenCursor = 0;

	std::array<const char*, SystemCursor_Max + 1> CursorImpl::s_systemCursorIds =
	{
		{
			// http://gnome-look.org/content/preview.php?preview=1&id=128170&file1=128170-1.png&file2=&file3=&name=Dummy+X11+cursors&PHPSESSID=6
			"crosshair",           // SystemCursor_Crosshair
			"left_ptr",            // SystemCursor_Default
			"hand",                // SystemCursor_Hand
			"help",                // SystemCursor_Help
			"fleur",               // SystemCursor_Move
			nullptr,               // SystemCursor_None
			"hand",                // SystemCursor_Pointer
			"watch",               // SystemCursor_Progress
			"right_side",          // SystemCursor_ResizeE
			"top_side",            // SystemCursor_ResizeN
			"top_right_corner",    // SystemCursor_ResizeNE
			"top_left_corner",     // SystemCursor_ResizeNW
			"bottom_side",         // SystemCursor_ResizeS
			"bottom_right_corner", // SystemCursor_ResizeSE
			"bottom_left_corner",  // SystemCursor_ResizeSW
			"left_side",           // SystemCursor_ResizeW
			"xterm",               // SystemCursor_Text
			"watch"                // SystemCursor_Wait
		}
	};

	static_assert(SystemCursor_Max + 1 == 18, "System cursor array is incomplete");
}
