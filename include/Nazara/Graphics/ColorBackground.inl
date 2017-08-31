// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Creates a new color background from the arguments
	* \return A reference to the newly created color background
	*
	* \param args Arguments for the color background
	*/

	template<typename... Args>
	ColorBackgroundRef ColorBackground::New(Args&&... args)
	{
		std::unique_ptr<ColorBackground> object(new ColorBackground(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
