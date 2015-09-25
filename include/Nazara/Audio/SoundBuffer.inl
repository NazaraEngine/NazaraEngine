// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	SoundBufferRef SoundBuffer::New(Args&&... args)
	{
		std::unique_ptr<SoundBuffer> object(new SoundBuffer(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Audio/DebugOff.hpp>
