// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GLX/ScopedXCB.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/GLX/Display.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	/***********************************************
	              ScopedXCBConnection
	***********************************************/

	ScopedXCBConnection::ScopedXCBConnection() :
	m_connection(nullptr)
	{
		m_connection = X11::OpenConnection();
	}

	ScopedXCBConnection::~ScopedXCBConnection()
	{
		X11::CloseConnection(m_connection);
	}

	ScopedXCBConnection::operator xcb_connection_t*() const
	{
		return m_connection;
	}

	/***********************************************
	             ScopedXCBEWMHConnection
	***********************************************/

	ScopedXCBEWMHConnection::ScopedXCBEWMHConnection(xcb_connection_t* connection) :
	m_ewmhConnection(nullptr)
	{
		m_ewmhConnection = X11::OpenEWMHConnection(connection);
	}

	ScopedXCBEWMHConnection::~ScopedXCBEWMHConnection()
	{
		X11::CloseEWMHConnection(m_ewmhConnection);
	}

	xcb_ewmh_connection_t* ScopedXCBEWMHConnection::operator ->() const
	{
		return m_ewmhConnection;
	}

	ScopedXCBEWMHConnection::operator xcb_ewmh_connection_t*() const
	{
		return m_ewmhConnection;
	}

	/***********************************************
	                  XCBGContext
	***********************************************/

	XCBGContext::XCBGContext(xcb_connection_t* connection) :
	m_connection(connection),
	m_gcontext(XCB_NONE)
	{
		NazaraAssert(connection, "Connection must have been established");
	}

	XCBGContext::~XCBGContext()
	{
		Destroy();
	}

	bool XCBGContext::Create(xcb_drawable_t drawable, uint32_t value_mask, const uint32_t* value_list)
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

	void XCBGContext::Destroy()
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

	XCBGContext::operator xcb_gcontext_t() const
	{
		return m_gcontext;
	}
}
