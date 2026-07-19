// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void ShaderBinding::Update(std::initializer_list<Binding> bindings)
	{
		Update(bindings.begin(), bindings.size());
	}

	inline auto ShaderBinding::StorageBufferBinding::FromView(const GpuBufferView& view, bool dynamic) -> StorageBufferBinding
	{
		return StorageBufferBinding{
			.buffer = view.GetBuffer(),
			.offset = view.GetOffset(),
			.range = view.GetSize(),
			.dynamic = dynamic
		};
	}

	inline auto ShaderBinding::StorageBufferBinding::WholeBuffer(GpuBuffer& buffer, bool dynamic) -> StorageBufferBinding
	{
		return StorageBufferBinding{
			.buffer = &buffer,
			.offset = 0,
			.range = buffer.GetSize(),
			.dynamic = dynamic
		};
	}

	inline auto ShaderBinding::UniformBufferBinding::FromView(const GpuBufferView& view, bool dynamic) -> UniformBufferBinding
	{
		return UniformBufferBinding{
			.buffer = view.GetBuffer(),
			.offset = view.GetOffset(),
			.range = view.GetSize(),
			.dynamic = dynamic
		};
	}

	inline auto ShaderBinding::UniformBufferBinding::WholeBuffer(GpuBuffer& buffer, bool dynamic) -> UniformBufferBinding
	{
		return UniformBufferBinding{
			.buffer = &buffer,
			.offset = 0,
			.range = buffer.GetSize(),
			.dynamic = dynamic
		};
	}

	inline void ShaderBindingDeleter::operator()(ShaderBinding* binding)
	{
		binding->Release();
	}
}
