// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINELAYOUT_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINELAYOUT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NZSL/GlslWriter.hpp>
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

			ShaderBindingPtr AllocateShaderBinding(UInt32 setIndex) override;

			inline const nzsl::GlslWriter::BindingMapping& GetBindingMapping() const;
			inline const RenderPipelineLayoutInfo& GetLayoutInfo() const;

			void UpdateDebugName(std::string_view name) override;

			OpenGLRenderPipelineLayout& operator=(const OpenGLRenderPipelineLayout&) = delete;
			OpenGLRenderPipelineLayout& operator=(OpenGLRenderPipelineLayout&&) = delete;

		private:
			struct DescriptorPool;
			struct SampledTextureDescriptor;
			struct StorageBufferDescriptor;
			struct TextureDescriptor;
			struct UniformBufferDescriptor;

			DescriptorPool& AllocatePool();
			ShaderBindingPtr AllocateFromPool(std::size_t poolIndex, UInt32 setIndex);
			template<typename F> void ForEachDescriptor(std::size_t poolIndex, std::size_t bindingIndex, F&& functor);
			SampledTextureDescriptor& GetSampledTextureDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex);
			StorageBufferDescriptor& GetStorageBufferDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex);
			TextureDescriptor& GetTextureDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex);
			UniformBufferDescriptor& GetUniformBufferDescriptor(std::size_t poolIndex, std::size_t bindingIndex, std::size_t descriptorIndex);
			void Release(ShaderBinding& binding);
			inline void TryToShrink();

			struct SampledTextureDescriptor
			{
				GLuint texture;
				GLuint sampler;
				GL::TextureTarget textureTarget;
			};

			struct StorageBufferDescriptor
			{
				GLuint buffer;
				GLintptr offset;
				GLsizeiptr size;
			};

			struct TextureDescriptor
			{
				GLboolean layered;
				GLenum access;
				GLenum format;
				GLint layer;
				GLint level;
				GLuint texture;
			};

			struct UniformBufferDescriptor
			{
				GLuint buffer;
				GLintptr offset;
				GLsizeiptr size;
			};

			using Descriptor = std::variant<std::monostate, SampledTextureDescriptor, StorageBufferDescriptor, TextureDescriptor, UniformBufferDescriptor>;

			struct DescriptorPool
			{
				using BindingStorage = std::aligned_storage_t<sizeof(OpenGLShaderBinding), alignof(OpenGLShaderBinding)>;

				Bitset<UInt64> freeBindings;
				std::vector<Descriptor> descriptors;
				std::unique_ptr<BindingStorage[]> storage;
			};

			std::size_t m_maxDescriptorCount;
			std::vector<DescriptorPool> m_descriptorPools;
			nzsl::GlslWriter::BindingMapping m_bindingMapping;
			RenderPipelineLayoutInfo m_layoutInfo;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINELAYOUT_HPP
