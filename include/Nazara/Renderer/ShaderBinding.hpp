// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_SHADERBINDING_HPP
#define NAZARA_RENDERER_SHADERBINDING_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <memory>
#include <string_view>
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

			virtual void UpdateDebugName(std::string_view name) = 0;

			ShaderBinding& operator=(const ShaderBinding&) = delete;
			ShaderBinding& operator=(ShaderBinding&&) = delete;

			struct SampledTextureBinding
			{
				const Texture* texture;
				const TextureSampler* sampler;
				TextureLayout textureLayout = TextureLayout::ColorInput;
			};

			struct SampledTextureBindings
			{
				UInt32 arraySize;
				const SampledTextureBinding* textureBindings;
			};

			struct StorageBufferBinding
			{
				RenderBuffer* buffer;
				UInt64 offset;
				UInt64 range;
				bool dynamic = false;
			};

			struct TextureBinding
			{
				const Texture* texture;
				TextureAccess access;
			};

			struct UniformBufferBinding
			{
				RenderBuffer* buffer;
				UInt64 offset;
				UInt64 range;
				bool dynamic = false;
			};

			struct Binding
			{
				UInt32 bindingIndex;
				std::variant<std::monostate, SampledTextureBinding, SampledTextureBindings, StorageBufferBinding, TextureBinding, UniformBufferBinding> content;
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
