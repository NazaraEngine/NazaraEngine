// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINELAYOUT_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINELAYOUT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <memory>
#include <type_traits>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderPipelineLayout : public RenderPipelineLayout
	{
		friend OpenGLShaderBinding;

		public:
			OpenGLRenderPipelineLayout(RenderPipelineLayoutInfo layoutInfo);
			OpenGLRenderPipelineLayout(const OpenGLRenderPipelineLayout&) = delete;
			OpenGLRenderPipelineLayout(OpenGLRenderPipelineLayout&&) = delete;
			~OpenGLRenderPipelineLayout();

			ShaderBindingPtr AllocateShaderBinding() override;

			inline const RenderPipelineLayoutInfo& GetLayoutInfo() const;

			inline std::size_t GetTextureDescriptorCount() const;
			inline std::size_t GetUniformBufferDescriptorCount() const;

			OpenGLRenderPipelineLayout& operator=(const OpenGLRenderPipelineLayout&) = delete;
			OpenGLRenderPipelineLayout& operator=(OpenGLRenderPipelineLayout&&) = delete;

		private:
			struct DescriptorPool;
			struct TextureDescriptor;
			struct UniformBufferDescriptor;

			DescriptorPool& AllocatePool();
			ShaderBindingPtr AllocateFromPool(std::size_t poolIndex);
			TextureDescriptor& GetTextureDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t textureIndex);
			UniformBufferDescriptor& GetUniformBufferDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t uniformBufferIndex);
			void Release(ShaderBinding& binding);
			inline void TryToShrink();

			static constexpr UInt32 InvalidIndex = 0xFFFFFFFF;

			struct TextureDescriptor
			{
				UInt32 bindingIndex = InvalidIndex;
				GLuint texture;
				GLuint sampler;
				GL::TextureTarget textureTarget;
			};

			struct UniformBufferDescriptor
			{
				UInt32 bindingIndex = InvalidIndex;
				GLuint buffer;
				GLintptr offset;
				GLsizeiptr size;
			};

			struct DescriptorPool
			{
				using BindingStorage = std::aligned_storage_t<sizeof(OpenGLShaderBinding), alignof(OpenGLShaderBinding)>;

				Bitset<UInt64> freeBindings;
				std::vector<TextureDescriptor> textureDescriptor;
				std::vector<UniformBufferDescriptor> uniformBufferDescriptor;
				std::unique_ptr<BindingStorage[]> storage;
			};

			std::size_t m_textureDescriptorCount;
			std::size_t m_uniformBufferDescriptorCount;
			std::vector<DescriptorPool> m_descriptorPools;
			RenderPipelineLayoutInfo m_layoutInfo;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP
