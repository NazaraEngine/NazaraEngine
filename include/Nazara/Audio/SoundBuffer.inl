// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Audio/Debug.hpp>

template<typename... Args>
NzSoundBufferRef NzSoundBuffer::New(Args&&... args)
{
	std::unique_ptr<NzSoundBuffer> object(new NzSoundBuffer(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Audio/DebugOff.hpp>
