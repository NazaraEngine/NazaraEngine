// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline void ShaderBinding::Update(std::initializer_list<Binding> bindings)
	{
		Update(bindings.begin(), bindings.size());
	}

	inline void ShaderBindingDeleter::operator()(ShaderBinding* binding)
	{
		binding->Release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
