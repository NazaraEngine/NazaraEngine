// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_X11DISPLAY_HPP
#define NAZARA_X11DISPLAY_HPP

#include <Nazara/Utility/WindowHandle.hpp>
#include <Nazara/Utility/X11/ScopedXCB.hpp>
#include <xcb/xcb_ewmh.h>
#include <string>

typedef struct _XCBKeySymbols xcb_key_symbols_t;

namespace X11
{
	bool CheckCookie(xcb_connection_t* connection, xcb_void_cookie_t cookie);
	void CloseConnection(xcb_connection_t* connection);
	void CloseEWMHConnection(xcb_ewmh_connection_t* ewmh_connection);

	xcb_atom_t GetAtom(const std::string& name, bool onlyIfExists = false);

	void Initialize();

	xcb_key_symbols_t* XCBKeySymbolsAlloc(xcb_connection_t* connection);
	void XCBKeySymbolsFree(xcb_key_symbols_t* keySymbols);

	xcb_connection_t* OpenConnection();
	xcb_ewmh_connection_t* OpenEWMHConnection(xcb_connection_t* connection);

	void Uninitialize();

	xcb_screen_t* XCBDefaultScreen(xcb_connection_t* connection);
	xcb_window_t XCBDefaultRootWindow(xcb_connection_t* connection);
	int XCBScreen(xcb_connection_t* connection);
	xcb_screen_t* XCBScreenOfDisplay(xcb_connection_t* connection, int screen_nbr);
}

#endif // NAZARA_X11DISPLAY_HPP
