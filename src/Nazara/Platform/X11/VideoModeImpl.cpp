// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/X11/VideoModeImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/X11/Display.hpp>
#include <xcb/randr.h>
#include <algorithm>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	VideoMode VideoModeImpl::GetDesktopMode()
	{
		VideoMode desktopMode;

		ScopedXCBConnection connection;

		// Retrieve the default screen
		xcb_screen_t* screen = X11::XCBDefaultScreen(connection);

		ScopedXCB<xcb_generic_error_t> error(nullptr);

		// Check if the RandR extension is present
		const xcb_query_extension_reply_t* randrExt = xcb_get_extension_data(connection, &xcb_randr_id);

		if (!randrExt || !randrExt->present)
		{
			// Randr extension is not supported: we cannot get the video modes
			NazaraError("Failed to use the RandR extension while trying to get the desktop video mode");
			return desktopMode;
		}

		// Load RandR and check its version
		ScopedXCB<xcb_randr_query_version_reply_t> randrVersion(xcb_randr_query_version_reply(
			connection,
			xcb_randr_query_version(
				connection,
				1,
				1
			),
			&error
		));

		if (error)
		{
			NazaraError("Failed to load the RandR extension while trying to get the desktop video mode");
			return desktopMode;
		}

		// Get the current configuration
		ScopedXCB<xcb_randr_get_screen_info_reply_t> config(xcb_randr_get_screen_info_reply(
			connection,
			xcb_randr_get_screen_info(
				connection,
				screen->root
			),
			&error
		));

		if (error)
		{
			// Failed to get the screen configuration
			NazaraError("Failed to retrieve the screen configuration while trying to get the desktop video mode");
			return desktopMode;
		}

		// Get the current video mode
		xcb_randr_mode_t currentMode = config->sizeID;

		// Get the available screen sizes
		int nbSizes = xcb_randr_get_screen_info_sizes_length(config.get());
		xcb_randr_screen_size_t* sizes = xcb_randr_get_screen_info_sizes(config.get());
		if (sizes && (nbSizes > 0))
		{
			desktopMode = VideoMode(sizes[currentMode].width, sizes[currentMode].height, screen->root_depth);

			if (config->rotation == XCB_RANDR_ROTATION_ROTATE_90 ||
				config->rotation == XCB_RANDR_ROTATION_ROTATE_270)
				std::swap(desktopMode.width, desktopMode.height);
		}
		else
		{
			NazaraError("Failed to retrieve any screen sizes while trying to get the desktop video mode");
		}

		return desktopMode;
	}

	void VideoModeImpl::GetFullscreenModes(std::vector<VideoMode>& modes)
	{
		ScopedXCBConnection connection;

		// Retrieve the default screen
		xcb_screen_t* screen = X11::XCBDefaultScreen(connection);

		ScopedXCB<xcb_generic_error_t> error(nullptr);

		const xcb_query_extension_reply_t* randrExt = xcb_get_extension_data(connection, &xcb_randr_id);

		if (!randrExt || !randrExt->present)
		{
			// Randr extension is not supported: we cannot get the video modes
			NazaraError("Failed to use the RandR extension while trying to get the supported video modes");
			return;
		}

		// Load RandR and check its version
		ScopedXCB<xcb_randr_query_version_reply_t> randrVersion(xcb_randr_query_version_reply(
			connection,
			xcb_randr_query_version(
				connection,
				1,
				1
			),
			&error
		));

		if (error)
		{
			NazaraError("Failed to load the RandR extension while trying to get the supported video modes");
			return;
		}

		// Get the current configuration
		ScopedXCB<xcb_randr_get_screen_info_reply_t> config(xcb_randr_get_screen_info_reply(
			connection,
			xcb_randr_get_screen_info(
				connection,
				screen->root
			),
			&error
		));

		if (error)
		{
			// Failed to get the screen configuration
			NazaraError("Failed to retrieve the screen configuration while trying to get the supported video modes");
			return;
		}

		// Get the available screen sizes
		xcb_randr_screen_size_t* sizes = xcb_randr_get_screen_info_sizes(config.get());
		if (sizes && (config->nSizes > 0))
		{
			// Get the list of supported depths
			xcb_depth_iterator_t iter = xcb_screen_allowed_depths_iterator(screen);
			// Combine depths and sizes to fill the array of supported modes
			for (; iter.rem; xcb_depth_next(&iter))
			{
				for (int j = 0; j < config->nSizes; ++j)
				{
					// Convert to VideoMode
					VideoMode mode(sizes[j].width, sizes[j].height, iter.data->depth);

					if (config->rotation == XCB_RANDR_ROTATION_ROTATE_90 ||
						config->rotation == XCB_RANDR_ROTATION_ROTATE_270)
						std::swap(mode.width, mode.height);

					// Add it only if it is not already in the array
					if (std::find(modes.begin(), modes.end(), mode) == modes.end())
						modes.push_back(mode);
				}
			}
		}
	}
}
