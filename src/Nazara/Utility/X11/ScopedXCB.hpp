// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCOPEDXCB_HPP
#define NAZARA_SCOPEDXCB_HPP

#include <xcb/xcb_ewmh.h>

class NzScopedXCBConnection
{
	public:
		NzScopedXCBConnection();
		~NzScopedXCBConnection();

		operator xcb_connection_t*() const;

	private:
		xcb_connection_t* m_connection;
};

class NzScopedXCBEWMHConnection
{
	public:
		NzScopedXCBEWMHConnection(xcb_connection_t* connection);
		~NzScopedXCBEWMHConnection();

		xcb_ewmh_connection_t* operator ->() const;

		operator xcb_ewmh_connection_t*() const;

	private:
		xcb_ewmh_connection_t* m_ewmhConnection;
};

template <typename T>
class NzScopedXCB
{
	public:
		NzScopedXCB(T* pointer);
		~NzScopedXCB();

		T* operator ->() const;
		T** operator &();

		operator bool() const;

		T* get() const;

	private:
		T* m_pointer;
};

class NzXCBGContext
{
	public:
		NzXCBGContext(xcb_connection_t* connection);
		~NzXCBGContext();

		bool Create(xcb_drawable_t drawable, uint32_t value_mask, const uint32_t* value_list);

		void Destroy();

		operator xcb_gcontext_t() const;

	private:
		xcb_connection_t* m_connection;
		xcb_gcontext_t m_gcontext;
};

class NzXCBPixmap
{
	public:
		NzXCBPixmap();
		NzXCBPixmap(xcb_connection_t* connection);
		~NzXCBPixmap();

		void Connect(xcb_connection_t* connection);
		bool Create(uint8_t depth, xcb_drawable_t drawable, uint16_t width, uint16_t height);
		bool CreatePixmapFromBitmapData(xcb_drawable_t drawable, uint8_t* data, uint32_t width, uint32_t height, uint32_t depth, uint32_t fg, uint32_t bg, xcb_gcontext_t* gcp);

		void Destroy();

		operator xcb_pixmap_t() const;

	private:
		xcb_connection_t* m_connection;
		xcb_pixmap_t m_pixmap;
};

#include <Nazara/Utility/X11/ScopedXCB.inl>

#endif // NAZARA_SCOPEDXCB_HPP
