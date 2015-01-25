// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

template<typename... Args>
NzUberShaderPreprocessorRef NzUberShaderPreprocessor::New(Args&&... args)
{
	std::unique_ptr<NzUberShaderPreprocessor> object(new NzUberShaderPreprocessor(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Renderer/DebugOff.hpp>
