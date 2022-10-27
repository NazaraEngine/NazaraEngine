// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_SHADERBINDING_HPP
#define NAZARA_RENDERER_SHADERBINDING_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <memory>
#include <variant>

namespace Nz
{
	class RenderBuffer;
	class ShaderBinding;
	class ShaderBindingDeleter;
	class Texture;
	class TextureSampler;

	using ShaderBindingPtr = std::unique_ptr<ShaderBinding, ShaderBindingDeleter>;

	class NAZARA_RENDERER_API ShaderBinding
	{
		friend ShaderBindingDeleter;

		public:
			struct Binding;

			ShaderBinding() = default;
			ShaderBinding(const ShaderBinding&) = delete;
			ShaderBinding(ShaderBinding&&) = delete;
			virtual ~ShaderBinding();

			virtual void Update(const Binding* bindings, std::size_t bindingCount) = 0;
			inline void Update(std::initializer_list<Binding> bindings);

			ShaderBinding& operator=(const ShaderBinding&) = delete;
			ShaderBinding& operator=(ShaderBinding&&) = delete;

			struct StorageBufferBinding
			{
				RenderBuffer* buffer;
				UInt64 offset;
				UInt64 range;
			};

			struct TextureBinding
			{
				const Texture* texture;
				const TextureSampler* sampler;
			};

			struct UniformBufferBinding
			{
				RenderBuffer* buffer;
				UInt64 offset;
				UInt64 range;
			};

			struct Binding
			{
				UInt32 bindingIndex;
				std::variant<StorageBufferBinding, TextureBinding, UniformBufferBinding> content;
			};

		protected:
			virtual void Release() = 0;
	};

	class ShaderBindingDeleter
	{
		public:
			inline void operator()(ShaderBinding* binding);
	};
}

#include <Nazara/Renderer/ShaderBinding.inl>

#endif // NAZARA_RENDERER_SHADERBINDING_HPP
