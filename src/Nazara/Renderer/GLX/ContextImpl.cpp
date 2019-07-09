// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Code inspiré de NeHe (Lesson1) et de la SFML par Laurent Gomila

#include <Nazara/Renderer/GLX/ContextImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Debug.hpp>

using namespace GLX;

namespace Nz
{
	namespace
	{
		Display* m_display;
		int m_sharedDisplay = 0;

		bool ctxErrorOccurred = false;
		int ctxErrorHandler( Display* /*dpy*/, XErrorEvent* /*ev*/ )
		{
			ctxErrorOccurred = true;
			return 0;
		}
	}

	ContextImpl::ContextImpl() :
	m_colormap(0),
	m_context(0),
	m_window(0),
	m_ownsWindow(false)
	{
		if (m_sharedDisplay == 0)
			m_display = XOpenDisplay(nullptr);

		++m_sharedDisplay;
	}

	ContextImpl::~ContextImpl()
	{
		Destroy();

		if (--m_sharedDisplay == 0)
		{
			XCloseDisplay(m_display);
			m_display = nullptr;
		}
	}

	bool ContextImpl::Activate() const
	{
		return glXMakeCurrent(m_display, m_window, m_context) == true;
	}

	bool ContextImpl::Create(ContextParameters& parameters)
	{
		// En cas d'exception, la ressource sera quand même libérée
		CallOnExit onExit([this] ()
		{
			Destroy();
		});

		// Get a matching FB config
		static int visual_attribs[] =
		{
			GLX_X_RENDERABLE, True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_BUFFER_SIZE, parameters.bitsPerPixel,
			GLX_ALPHA_SIZE, (parameters.bitsPerPixel == 32) ? 8 : 0,
			GLX_DEPTH_SIZE, parameters.depthBits,
			GLX_STENCIL_SIZE, parameters.stencilBits,
			GLX_DOUBLEBUFFER, True,
			GLX_SAMPLE_BUFFERS, (parameters.antialiasingLevel > 0) ? True : False,
			GLX_SAMPLES, parameters.antialiasingLevel,
			None
		};

		int glx_major = 0;
		int glx_minor = 0;
		// FBConfigs were added in GLX version 1.3.
		if (!glXQueryVersion(m_display, &glx_major, &glx_minor) || ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
		{
			NazaraError("Invalid GLX version, version > 1.3 is required.");
			return false;
		}

		int fbcount;
		GLXFBConfig* fbc = glXChooseFBConfig(m_display, XDefaultScreen(m_display), visual_attribs, &fbcount);
		if (!fbc)
		{
			NazaraError("Failed to retrieve a framebuffer config");
			return false;
		}

		// Pick the FB config/visual with the most samples per pixel
		int best_fbc = -1;
		int worst_fbc = -1;
		int best_num_samp = -1;
		int worst_num_samp = 999;

		for (int i = 0; i < fbcount; ++i)
		{
			XVisualInfo* vi = glXGetVisualFromFBConfig(m_display, fbc[i]);

			if (vi)
			{
				int samp_buf = 0, samples = 0;
				glXGetFBConfigAttrib(m_display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
				glXGetFBConfigAttrib(m_display, fbc[i], GLX_SAMPLES       , &samples );

				if ((best_fbc < 0) || (samp_buf && (samples > best_num_samp)))
				{
					best_fbc = i;
					best_num_samp = samples;
				}
				if ((worst_fbc < 0) || !samp_buf || (samples < worst_num_samp))
				{
					worst_fbc = i;
					worst_num_samp = samples;
				}
			}
			XFree(vi);
		}

		GLXFBConfig bestFbc = fbc[best_fbc];

		// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
		XFree(fbc);

		// Get a visual
		XVisualInfo* vi = glXGetVisualFromFBConfig(m_display, bestFbc);
		if (!vi)
		{
			NazaraError("Failed to get best VisualInfo");
			return false;
		}

		// If context is shared by multiple windows
		if (parameters.window)
		{
			m_window = parameters.window;
			m_ownsWindow = false;
		}
		else
		{
			XSetWindowAttributes swa;
			swa.colormap = m_colormap = XCreateColormap(
				m_display,
				XRootWindow(
					m_display,
					vi->screen),
				vi->visual,
				AllocNone
			);

			swa.background_pixmap = None;
			swa.border_pixel      = 0;
			swa.event_mask        = StructureNotifyMask;

			if (!m_colormap)
			{
				NazaraError("Failed to create colormap for context");
				return false;
			}

			m_window = XCreateWindow(
				m_display,
				XRootWindow(
					m_display,
					vi->screen),
				0, 0, // X, Y
				1, 1, // W H
				0,
				vi->depth,
				InputOutput,
				vi->visual,
				CWBorderPixel | CWColormap | CWEventMask,
				&swa
			);

			m_ownsWindow = true;
		}

		if (!m_window)
		{
			NazaraError("Failed to create window");
			return false;
		}

		// Done with the visual info data
		XFree(vi);

		// Install an X error handler so the application won't exit if GL 3.0
		// context allocation fails.
		//
		// Note this error handler is global.  All display connections in all threads
		// of a process use the same error handler, so be sure to guard against other
		// threads issuing X commands while this code is running.
		ctxErrorOccurred = false;
		int (*oldHandler)(Display*, XErrorEvent*) =
		  XSetErrorHandler(&ctxErrorHandler);

		// Check for the GLX_ARB_create_context extension string and the function.
		// If either is not present, use GLX 1.3 context creation method.
		if (!glXCreateContextAttribs)
		{
			NazaraWarning("glXCreateContextAttribs() not found. Using old-style GLX context");
			m_context = glXCreateNewContext(m_display, bestFbc, GLX_RGBA_TYPE, parameters.shared ? parameters.shareContext->m_impl->m_context : 0, True);
		}
		// If it does, try to get a GL 3.0 context!
		else
		{
			int profile = parameters.compatibilityProfile ? GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
			int debug = parameters.debugMode ? GLX_CONTEXT_DEBUG_BIT_ARB : 0;

			int major = 3;//parameters.majorVersion;
			int minor = 3;//parameters.minorVersion;

			int context_attribs[] =
			{
				GLX_CONTEXT_MAJOR_VERSION_ARB, major,
				GLX_CONTEXT_MINOR_VERSION_ARB, minor,
				GLX_CONTEXT_PROFILE_MASK_ARB,  profile,
				GLX_CONTEXT_FLAGS_ARB,         debug,
				None,                          None
			};

			m_context = glXCreateContextAttribs(
				m_display,
				bestFbc,
				parameters.shared ? parameters.shareContext->m_impl->m_context : 0,
				True,
				context_attribs
			);
		}

		// Sync to ensure any errors generated are processed.
		XSync(m_display, False);
		XSetErrorHandler(oldHandler);
		if (ctxErrorOccurred || !m_context)
		{
			NazaraError("Failed to create context, check the version");
			return false;
		}

		onExit.Reset();

		return true;
	}

	void ContextImpl::Destroy()
	{
		// Destroy the context
		if (m_context)
		{
			if (glXGetCurrentContext() == m_context)
				glXMakeCurrent(m_display, None, nullptr);
			glXDestroyContext(m_display, m_context);
			m_context = nullptr;
		}

		// Destroy the window if we own it
		if (m_ownsWindow && m_window)
		{
			XFreeColormap(m_display, m_colormap);
			XDestroyWindow(m_display, m_window);
			m_ownsWindow = false;
			m_window = 0;
			XFlush(m_display);
		}
	}

	void ContextImpl::EnableVerticalSync(bool enabled)
	{
		if (glXSwapIntervalEXT)
			glXSwapIntervalEXT(m_display, glXGetCurrentDrawable(), enabled ? 1 : 0);
		else if (NzglXSwapIntervalMESA)
			NzglXSwapIntervalMESA(enabled ? 1 : 0);
		else if (glXSwapIntervalSGI)
			glXSwapIntervalSGI(enabled ? 1 : 0);
		else
			NazaraError("Vertical sync not supported");
	}

	void ContextImpl::SwapBuffers()
	{
		if (m_window)
			glXSwapBuffers(m_display, m_window);
	}

	bool ContextImpl::Desactivate()
	{
		return glXMakeCurrent(m_display, None, nullptr) == true;
	}
}
