// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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

