// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	ParticleDeclarationRef ParticleDeclaration::New(Args&&... args)
	{
		std::unique_ptr<ParticleDeclaration> object(new ParticleDeclaration(std::forward<Args>(args)...));
		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
