// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	UberShaderPreprocessorRef UberShaderPreprocessor::New(Args&&... args)
	{
		std::unique_ptr<UberShaderPreprocessor> object(new UberShaderPreprocessor(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
