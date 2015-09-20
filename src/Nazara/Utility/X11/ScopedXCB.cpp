// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/X11/ScopedXCB.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/X11/Display.hpp>
#include <xcb/xcb_image.h>
#include <Nazara/Utility/Debug.hpp>

/***********************************************
          NzScopedXCBConnection
***********************************************/

NzScopedXCBConnection::NzScopedXCBConnection() :
m_connection(nullptr)
{
	m_connection = X11::OpenConnection();
}

NzScopedXCBConnection::~NzScopedXCBConnection()
{
	X11::CloseConnection(m_connection);
}

NzScopedXCBConnection::operator xcb_connection_t*() const
{
	return m_connection;
}

/***********************************************
          NzScopedXCBEWMHConnection
***********************************************/

NzScopedXCBEWMHConnection::NzScopedXCBEWMHConnection(xcb_connection_t* connection) :
m_ewmhConnection(nullptr)
{
	m_ewmhConnection = X11::OpenEWMHConnection(connection);
}

NzScopedXCBEWMHConnection::~NzScopedXCBEWMHConnection()
{
	X11::CloseEWMHConnection(m_ewmhConnection);
}

xcb_ewmh_connection_t* NzScopedXCBEWMHConnection::operator ->() const
{
	return m_ewmhConnection;
}

NzScopedXCBEWMHConnection::operator xcb_ewmh_connection_t*() const
{
	return m_ewmhConnection;
}

/***********************************************
               NzXCBGContext
***********************************************/

NzXCBGContext::NzXCBGContext(xcb_connection_t* connection) :
m_connection(connection),
m_gcontext(XCB_NONE)
{
	NazaraAssert(connection, "Connection must have been established");
}

NzXCBGContext::~NzXCBGContext()
{
	Destroy();
}

bool NzXCBGContext::Create(xcb_drawable_t drawable, uint32_t value_mask, const uint32_t* value_list)
{
	NazaraAssert(m_gcontext == XCB_NONE, "Context must have been destroyed before or just created");

	m_gcontext = xcb_generate_id(m_connection);

	return X11::CheckCookie(
		m_connection,
		xcb_create_gc(
			m_connection,
			m_gcontext,
			drawable,
			value_mask,
			value_list
		));
}

void NzXCBGContext::Destroy()
{
	if (m_gcontext == XCB_NONE)
		return;

	if (!X11::CheckCookie(
		m_connection,
		xcb_free_gc(
			m_connection,
			m_gcontext
		))
	)
		NazaraError("Failed to free gcontext");

	m_gcontext = XCB_NONE;
}

NzXCBGContext::operator xcb_gcontext_t() const
{
	return m_gcontext;
}

/***********************************************
                NzXCBPixmap
***********************************************/

NzXCBPixmap::NzXCBPixmap() :
m_connection(nullptr),
m_pixmap(XCB_NONE)
{
}

NzXCBPixmap::NzXCBPixmap(xcb_connection_t* connection) :
m_connection(connection),
m_pixmap(XCB_NONE)
{
}

NzXCBPixmap::~NzXCBPixmap()
{
	Destroy();
}

void NzXCBPixmap::Connect(xcb_connection_t* connection)
{
	NazaraAssert(connection && !m_connection, "Connection must be established");

	m_connection = connection;
}

bool NzXCBPixmap::Create(uint8_t depth, xcb_drawable_t drawable, uint16_t width, uint16_t height)
{
	NazaraAssert(m_pixmap == XCB_NONE, "Pixmap must have been destroyed before or just created");

	m_pixmap = xcb_generate_id(m_connection);

	return X11::CheckCookie(
		m_connection,
		xcb_create_pixmap(
			m_connection,
			depth,
			m_pixmap,
			drawable,
			width,
			height
		));
}

bool NzXCBPixmap::CreatePixmapFromBitmapData(xcb_drawable_t drawable, uint8_t* data, uint32_t width, uint32_t height, uint32_t depth, uint32_t fg, uint32_t bg, xcb_gcontext_t* gcp)
{
	NazaraAssert(m_pixmap == XCB_NONE, "Pixmap must have been destroyed before or just created");

	m_pixmap = xcb_create_pixmap_from_bitmap_data(
		m_connection,
		drawable,
		data,
		width,
		height,
		depth,
		fg,
		bg,
		gcp
	);

	return m_pixmap != XCB_NONE;
}

void NzXCBPixmap::Destroy()
{
	if (m_pixmap == XCB_NONE)
		return;

	if (!X11::CheckCookie(
		m_connection,
		xcb_free_pixmap(
			m_connection,
			m_pixmap
		))
	)
		NazaraError("Failed to free pixmap");

	m_pixmap = XCB_NONE;
}

NzXCBPixmap::operator xcb_pixmap_t() const
{
	return m_pixmap;
}
