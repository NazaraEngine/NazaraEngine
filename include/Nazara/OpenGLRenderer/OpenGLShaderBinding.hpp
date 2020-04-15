// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DescriptorSet.hpp>

namespace Nz
{
	class OpenGLRenderPipelineLayout;

	class NAZARA_OPENGLRENDERER_API OpenGLShaderBinding : public ShaderBinding
	{
		public:
			inline OpenGLShaderBinding(OpenGLRenderPipelineLayout& owner, std::size_t poolIndex, std::size_t bindingIndex, Vk::DescriptorSet descriptorSet);
			OpenGLShaderBinding(const OpenGLShaderBinding&) = default;
			OpenGLShaderBinding(OpenGLShaderBinding&&) noexcept = default;
			~OpenGLShaderBinding() = default;

			inline std::size_t GetBindingIndex() const;
			inline const Vk::DescriptorSet& GetDescriptorSet() const;
			inline std::size_t GetPoolIndex() const;
			inline const OpenGLRenderPipelineLayout& GetOwner() const;

			void Update(std::initializer_list<Binding> bindings) override;

			OpenGLShaderBinding& operator=(const OpenGLShaderBinding&) = delete;
			OpenGLShaderBinding& operator=(OpenGLShaderBinding&&) = delete;

		private:
			void Release() override;

			Vk::AutoDescriptorSet m_descriptorSet;
			OpenGLRenderPipelineLayout& m_owner;
			std::size_t m_bindingIndex;
			std::size_t m_poolIndex;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERBINDING_HPP
