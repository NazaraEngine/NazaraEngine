// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>

namespace Nz
{
	class OpenGLRenderPipelineLayout;

	class NAZARA_OPENGLRENDERER_API OpenGLShaderBinding : public ShaderBinding
	{
		public:
			inline OpenGLShaderBinding(OpenGLRenderPipelineLayout& owner, std::size_t poolIndex, std::size_t bindingIndex);
			OpenGLShaderBinding(const OpenGLShaderBinding&) = delete;
			OpenGLShaderBinding(OpenGLShaderBinding&&) = delete;
			~OpenGLShaderBinding() = default;

			void Apply(const GL::Context& context) const;

			inline std::size_t GetBindingIndex() const;
			inline std::size_t GetPoolIndex() const;
			inline const OpenGLRenderPipelineLayout& GetOwner() const;

			void Update(const Binding* bindings, std::size_t bindingCount) override;

			OpenGLShaderBinding& operator=(const OpenGLShaderBinding&) = delete;
			OpenGLShaderBinding& operator=(OpenGLShaderBinding&&) = delete;

		private:
			void Release() override;

			OpenGLRenderPipelineLayout& m_owner;
			std::size_t m_bindingIndex;
			std::size_t m_poolIndex;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP
