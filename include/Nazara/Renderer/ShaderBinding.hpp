// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERBINDING_HPP
#define NAZARA_SHADERBINDING_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <variant>

namespace Nz
{
	class AbstractBuffer;
	class Texture;
	class TextureSampler;

	class NAZARA_RENDERER_API ShaderBinding
	{
		public:
			struct Binding;

			ShaderBinding() = default;
			virtual ~ShaderBinding();

			virtual void Update(std::initializer_list<Binding> bindings) = 0;

			struct TextureBinding
			{
				Texture* texture;
				TextureSampler* sampler;
			};

			struct UniformBufferBinding
			{
				AbstractBuffer* buffer;
				UInt64 offset;
				UInt64 range;
			};

			struct Binding
			{
				std::size_t bindingIndex;
				std::variant<TextureBinding, UniformBufferBinding> content;
			};

		protected:
			ShaderBinding(const ShaderBinding&) = delete;
			ShaderBinding(ShaderBinding&&) = default;
	};
}

#include <Nazara/Renderer/ShaderBinding.inl>

#endif // NAZARA_SHADERBINDING_HPP
