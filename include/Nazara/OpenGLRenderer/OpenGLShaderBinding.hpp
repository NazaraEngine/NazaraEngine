// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>

namespace Nz
{
	class OpenGLRenderPipelineLayout;

	class NAZARA_OPENGLRENDERER_API OpenGLShaderBinding final : public ShaderBinding
	{
		public:
			inline OpenGLShaderBinding(OpenGLRenderPipelineLayout& owner, std::size_t poolIndex, std::size_t bindingIndex);
			OpenGLShaderBinding(const OpenGLShaderBinding&) = delete;
			OpenGLShaderBinding(OpenGLShaderBinding&&) = delete;
			~OpenGLShaderBinding() = default;

			void Apply(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 setIndex, const GL::Context& context) const;

			inline std::size_t GetBindingIndex() const;
			inline std::size_t GetPoolIndex() const;
			inline const OpenGLRenderPipelineLayout& GetOwner() const;

			void Update(const Binding* bindings, std::size_t bindingCount) override;

			void UpdateDebugName(std::string_view name) override;

			OpenGLShaderBinding& operator=(const OpenGLShaderBinding&) = delete;
			OpenGLShaderBinding& operator=(OpenGLShaderBinding&&) = delete;

		private:
			void HandleTextureBinding(UInt32 bindingIndex, const SampledTextureBinding& textureBinding);
			void Release() override;

			OpenGLRenderPipelineLayout& m_owner;
			std::size_t m_bindingIndex;
			std::size_t m_poolIndex;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP
