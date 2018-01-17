// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_X11DISPLAY_HPP
#define NAZARA_X11DISPLAY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/Config.hpp>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

typedef struct _XCBKeySymbols xcb_key_symbols_t;

namespace Nz
{
	class String;

	class NAZARA_PLATFORM_API X11
	{
		public:
			X11() = delete;
			~X11() = delete;

			static bool CheckCookie(xcb_connection_t* connection, xcb_void_cookie_t cookie);
			static void CloseConnection(xcb_connection_t* connection);
			static void CloseEWMHConnection(xcb_ewmh_connection_t* ewmh_connection);

			static xcb_atom_t GetAtom(const String& name, bool onlyIfExists = false);

			static bool Initialize();
			static bool IsInitialized();

			static xcb_key_symbols_t* XCBKeySymbolsAlloc(xcb_connection_t* connection);
			static void XCBKeySymbolsFree(xcb_key_symbols_t* keySymbols);

			static xcb_connection_t* OpenConnection();
			static xcb_ewmh_connection_t* OpenEWMHConnection(xcb_connection_t* connection);

			static void Uninitialize();

			static xcb_screen_t* XCBDefaultScreen(xcb_connection_t* connection);
			static xcb_window_t XCBDefaultRootWindow(xcb_connection_t* connection);
			static int XCBScreen(xcb_connection_t* connection);
			static xcb_screen_t* XCBScreenOfDisplay(xcb_connection_t* connection, int screen_nbr);

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_X11DISPLAY_HPP
