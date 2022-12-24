// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline std::optional<PixelFormat> FromVulkan(VkFormat format)
	{
		switch (format)
		{
			case VK_FORMAT_B8G8R8A8_UNORM:     return PixelFormat::BGRA8;
			case VK_FORMAT_B8G8R8A8_SRGB:      return PixelFormat::BGRA8_SRGB;
			case VK_FORMAT_D16_UNORM:          return PixelFormat::Depth16;
			case VK_FORMAT_D16_UNORM_S8_UINT:  return PixelFormat::Depth16Stencil8;
			case VK_FORMAT_D24_UNORM_S8_UINT:  return PixelFormat::Depth24Stencil8;
			case VK_FORMAT_D32_SFLOAT:         return PixelFormat::Depth32F;
			case VK_FORMAT_D32_SFLOAT_S8_UINT: return PixelFormat::Depth32FStencil8;
			case VK_FORMAT_R8G8B8A8_UNORM:     return PixelFormat::RGBA8;
			case VK_FORMAT_R8G8B8A8_SRGB:      return PixelFormat::RGBA8_SRGB;
			default: break;
		}

		return std::nullopt;
	}

	inline VkAttachmentLoadOp ToVulkan(AttachmentLoadOp loadOp)
	{
		switch (loadOp)
		{
			case AttachmentLoadOp::Clear:   return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case AttachmentLoadOp::Discard: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			case AttachmentLoadOp::Load:    return VK_ATTACHMENT_LOAD_OP_LOAD;
		}

		NazaraError("Unhandled AttachmentLoadOp 0x" + NumberToString(UnderlyingCast(loadOp), 16));
		return {};
	}

	inline VkAttachmentStoreOp ToVulkan(AttachmentStoreOp storeOp)
	{
		switch (storeOp)
		{
			case AttachmentStoreOp::Discard: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case AttachmentStoreOp::Store:   return VK_ATTACHMENT_STORE_OP_STORE;
		}

		NazaraError("Unhandled AttachmentStoreOp 0x" + NumberToString(UnderlyingCast(storeOp), 16));
		return {};
	}

	inline VkBlendOp ToVulkan(BlendEquation blendEquation)
	{
		switch (blendEquation)
		{
			case BlendEquation::Add:             return VK_BLEND_OP_ADD;
			case BlendEquation::Max:             return VK_BLEND_OP_MAX;
			case BlendEquation::Min:             return VK_BLEND_OP_MIN;
			case BlendEquation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
			case BlendEquation::Subtract:        return VK_BLEND_OP_SUBTRACT;
		}

		NazaraError("Unhandled BlendEquation 0x" + NumberToString(UnderlyingCast(blendEquation), 16));
		return {};
	}

	inline VkBlendFactor ToVulkan(BlendFunc blendFunc)
	{
		switch (blendFunc)
		{
			case BlendFunc::ConstantAlpha:    return VK_BLEND_FACTOR_CONSTANT_ALPHA;
			case BlendFunc::ConstantColor:    return VK_BLEND_FACTOR_CONSTANT_COLOR;
			case BlendFunc::DstAlpha:         return VK_BLEND_FACTOR_DST_ALPHA;
			case BlendFunc::DstColor:         return VK_BLEND_FACTOR_DST_COLOR;
			case BlendFunc::InvConstantAlpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
			case BlendFunc::InvConstantColor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			case BlendFunc::InvDstAlpha:      return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			case BlendFunc::InvDstColor:      return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case BlendFunc::InvSrcAlpha:      return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case BlendFunc::InvSrcColor:      return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case BlendFunc::SrcAlpha:         return VK_BLEND_FACTOR_SRC_ALPHA;
			case BlendFunc::SrcColor:         return VK_BLEND_FACTOR_SRC_COLOR;
			case BlendFunc::One:              return VK_BLEND_FACTOR_ONE;
			case BlendFunc::Zero:             return VK_BLEND_FACTOR_ZERO;
		}

		NazaraError("Unhandled BlendFunc 0x" + NumberToString(UnderlyingCast(blendFunc), 16));
		return {};
	}

	inline VkBufferUsageFlags ToVulkan(BufferType bufferType)
	{
		switch (bufferType)
		{
			case BufferType::Index: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case BufferType::Storage: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			case BufferType::Vertex: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case BufferType::Uniform: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}

		NazaraError("Unhandled BufferType 0x" + NumberToString(UnderlyingCast(bufferType), 16));
		return 0;
	}

	inline VkFormat ToVulkan(ComponentType componentType)
	{
		switch (componentType)
		{
			case ComponentType::Color:      return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ComponentType::Double1:    return VK_FORMAT_R64_SFLOAT;
			case ComponentType::Double2:    return VK_FORMAT_R64G64_SFLOAT;
			case ComponentType::Double3:    return VK_FORMAT_R64G64B64_SFLOAT;
			case ComponentType::Double4:    return VK_FORMAT_R64G64B64A64_SFLOAT;
			case ComponentType::Float1:     return VK_FORMAT_R32_SFLOAT;
			case ComponentType::Float2:     return VK_FORMAT_R32G32_SFLOAT;
			case ComponentType::Float3:     return VK_FORMAT_R32G32B32_SFLOAT;
			case ComponentType::Float4:     return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ComponentType::Int1:       return VK_FORMAT_R32_SINT;
			case ComponentType::Int2:       return VK_FORMAT_R32G32_SINT;
			case ComponentType::Int3:       return VK_FORMAT_R32G32B32_SINT;
			case ComponentType::Int4:       return VK_FORMAT_R32G32B32A32_SINT;
		}

		NazaraError("Unhandled ComponentType 0x" + NumberToString(UnderlyingCast(componentType), 16));
		return VK_FORMAT_UNDEFINED;
	}

	inline VkCullModeFlagBits ToVulkan(FaceCulling faceSide)
	{
		switch (faceSide)
		{
			case FaceCulling::None:         return VK_CULL_MODE_NONE;
			case FaceCulling::Back:         return VK_CULL_MODE_BACK_BIT;
			case FaceCulling::Front:        return VK_CULL_MODE_FRONT_BIT;
			case FaceCulling::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
		}

		NazaraError("Unhandled FaceSide 0x" + NumberToString(UnderlyingCast(faceSide), 16));
		return VK_CULL_MODE_BACK_BIT;
	}

	inline VkPolygonMode ToVulkan(FaceFilling faceFilling)
	{
		switch (faceFilling)
		{
			case FaceFilling::Fill:  return VK_POLYGON_MODE_FILL;
			case FaceFilling::Line:  return VK_POLYGON_MODE_LINE;
			case FaceFilling::Point: return VK_POLYGON_MODE_POINT;
		}

		NazaraError("Unhandled FaceFilling 0x" + NumberToString(UnderlyingCast(faceFilling), 16));
		return VK_POLYGON_MODE_FILL;
	}

	inline VkFrontFace ToVulkan(FrontFace frontFace)
	{
		switch (frontFace)
		{
			case FrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
			case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}

		NazaraError("Unhandled FrontFace 0x" + NumberToString(UnderlyingCast(frontFace), 16));
		return {};
	}

	inline VkIndexType ToVulkan(IndexType indexType)
	{
		switch (indexType)
		{
			case IndexType::U8:  return VK_INDEX_TYPE_UINT8_EXT;
			case IndexType::U16: return VK_INDEX_TYPE_UINT16;
			case IndexType::U32: return VK_INDEX_TYPE_UINT32;
		}

		NazaraError("Unhandled IndexType 0x" + NumberToString(UnderlyingCast(indexType), 16));
		return {};
	}

	inline VkAccessFlagBits ToVulkan(MemoryAccess memoryAccess)
	{
		switch (memoryAccess)
		{
			case MemoryAccess::ColorRead:           return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			case MemoryAccess::ColorWrite:          return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			case MemoryAccess::DepthStencilRead:    return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			case MemoryAccess::DepthStencilWrite:   return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			case MemoryAccess::IndexBufferRead:     return VK_ACCESS_INDEX_READ_BIT;
			case MemoryAccess::IndirectCommandRead: return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
			case MemoryAccess::HostRead:            return VK_ACCESS_HOST_READ_BIT;
			case MemoryAccess::HostWrite:           return VK_ACCESS_HOST_WRITE_BIT;
			case MemoryAccess::MemoryRead:          return VK_ACCESS_MEMORY_READ_BIT;
			case MemoryAccess::MemoryWrite:         return VK_ACCESS_MEMORY_WRITE_BIT;
			case MemoryAccess::ShaderRead:          return VK_ACCESS_SHADER_READ_BIT;
			case MemoryAccess::ShaderWrite:         return VK_ACCESS_SHADER_WRITE_BIT;
			case MemoryAccess::TransferRead:        return VK_ACCESS_TRANSFER_READ_BIT;
			case MemoryAccess::TransferWrite:       return VK_ACCESS_TRANSFER_WRITE_BIT;
			case MemoryAccess::UniformBufferRead:   return VK_ACCESS_UNIFORM_READ_BIT;
			case MemoryAccess::VertexBufferRead:    return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		}

		NazaraError("Unhandled MemoryAccess 0x" + NumberToString(UnderlyingCast(memoryAccess), 16));
		return {};
	}

	inline VkAccessFlags ToVulkan(MemoryAccessFlags memoryAccessFlags)
	{
		VkShaderStageFlags accessBits = 0;
		for (int i = 0; i <= UnderlyingCast(MemoryAccess::Max); ++i)
		{
			MemoryAccess memoryAccess = static_cast<MemoryAccess>(i);
			if (memoryAccessFlags.Test(memoryAccess))
				accessBits |= ToVulkan(memoryAccess);
		}

		return accessBits;
	}

	inline VkPipelineStageFlagBits ToVulkan(PipelineStage pipelineStage)
	{
		switch (pipelineStage)
		{
			case PipelineStage::TopOfPipe:                    return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			case PipelineStage::ColorOutput:                  return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			case PipelineStage::DrawIndirect:                 return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
			case PipelineStage::FragmentShader:               return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			case PipelineStage::FragmentTestsEarly:           return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			case PipelineStage::FragmentTestsLate:            return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			case PipelineStage::GeometryShader:               return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			case PipelineStage::TessellationControlShader:    return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
			case PipelineStage::TessellationEvaluationShader: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			case PipelineStage::Transfer:                     return VK_PIPELINE_STAGE_TRANSFER_BIT;
			case PipelineStage::TransformFeedback:            return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
			case PipelineStage::VertexInput:                  return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
			case PipelineStage::VertexShader:                 return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			case PipelineStage::BottomOfPipe:                 return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}

		NazaraError("Unhandled PipelineStage 0x" + NumberToString(UnderlyingCast(pipelineStage), 16));
		return {};
	}

	inline VkPipelineStageFlags ToVulkan(PipelineStageFlags pipelineStages)
	{
		VkShaderStageFlags pipelineStageBits = 0;
		for (int i = 0; i <= UnderlyingCast(PipelineStage::Max); ++i)
		{
			PipelineStage pipelineStage = static_cast<PipelineStage>(i);
			if (pipelineStages.Test(pipelineStage))
				pipelineStageBits |= ToVulkan(pipelineStage);
		}

		return pipelineStageBits;
	}

	inline VkFormat ToVulkan(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
			// TODO: Fill this switch
			case PixelFormat::BGR8:             return VK_FORMAT_B8G8R8_UNORM;
			case PixelFormat::BGR8_SRGB:        return VK_FORMAT_B8G8R8_SRGB;
			case PixelFormat::BGRA8:            return VK_FORMAT_B8G8R8A8_UNORM;
			case PixelFormat::BGRA8_SRGB:       return VK_FORMAT_B8G8R8A8_SRGB;
			case PixelFormat::Depth16:          return VK_FORMAT_D16_UNORM;
			case PixelFormat::Depth16Stencil8:  return VK_FORMAT_D16_UNORM_S8_UINT;
			case PixelFormat::Depth24:          return VK_FORMAT_UNDEFINED;
			case PixelFormat::Depth24Stencil8:  return VK_FORMAT_D24_UNORM_S8_UINT;
			case PixelFormat::Depth32F:         return VK_FORMAT_D32_SFLOAT;
			case PixelFormat::Depth32FStencil8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
			case PixelFormat::RGB8:             return VK_FORMAT_R8G8B8_UNORM;
			case PixelFormat::RGB8_SRGB:        return VK_FORMAT_R8G8B8_SRGB;
			case PixelFormat::RGBA8:            return VK_FORMAT_R8G8B8A8_UNORM;
			case PixelFormat::RGBA8_SRGB:       return VK_FORMAT_R8G8B8A8_SRGB;
			case PixelFormat::RGBA16F:          return VK_FORMAT_R16G16B16A16_SFLOAT;
			case PixelFormat::RGBA32F:          return VK_FORMAT_R32G32B32A32_SFLOAT;
			case PixelFormat::Stencil1:         return VK_FORMAT_UNDEFINED;
			case PixelFormat::Stencil4:         return VK_FORMAT_UNDEFINED;
			case PixelFormat::Stencil8:         return VK_FORMAT_S8_UINT;
			case PixelFormat::Stencil16:        return VK_FORMAT_UNDEFINED;
			default: break;
		}

		NazaraError("Unhandled PixelFormat 0x" + NumberToString(UnderlyingCast(pixelFormat), 16));
		return VK_FORMAT_UNDEFINED;
	}

	VkImageAspectFlags ToVulkan(PixelFormatContent pixelFormatContent)
	{
		switch (pixelFormatContent)
		{
			case PixelFormatContent::Undefined:
				break;

			case PixelFormatContent::ColorRGBA:    return VK_IMAGE_ASPECT_COLOR_BIT;
			case PixelFormatContent::Depth:        return VK_IMAGE_ASPECT_DEPTH_BIT;
			case PixelFormatContent::DepthStencil: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			case PixelFormatContent::Stencil:      return VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		NazaraError("Unhandled PixelFormatContent 0x" + NumberToString(UnderlyingCast(pixelFormatContent), 16));
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}

	inline VkPrimitiveTopology ToVulkan(PrimitiveMode primitiveMode)
	{
		switch (primitiveMode)
		{
			case PrimitiveMode::LineList:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case PrimitiveMode::LineStrip:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case PrimitiveMode::PointList:     return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case PrimitiveMode::TriangleList:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case PrimitiveMode::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case PrimitiveMode::TriangleFan:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		}

		NazaraError("Unhandled FaceFilling 0x" + NumberToString(UnderlyingCast(primitiveMode), 16));
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	inline VkCompareOp ToVulkan(RendererComparison comparison)
	{
		switch (comparison)
		{
			case RendererComparison::Never:          return VK_COMPARE_OP_NEVER; 
			case RendererComparison::Less:           return VK_COMPARE_OP_LESS; 
			case RendererComparison::Equal:          return VK_COMPARE_OP_EQUAL; 
			case RendererComparison::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL; 
			case RendererComparison::Greater:        return VK_COMPARE_OP_GREATER; 
			case RendererComparison::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL; 
			case RendererComparison::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL; 
			case RendererComparison::Always:         return VK_COMPARE_OP_ALWAYS; 
		}

		NazaraError("Unhandled RendererComparison 0x" + NumberToString(UnderlyingCast(comparison), 16));
		return VK_COMPARE_OP_NEVER;
	}

	inline VkFilter ToVulkan(SamplerFilter samplerFilter)
	{
		switch (samplerFilter)
		{
			case SamplerFilter::Linear:  return VK_FILTER_LINEAR;
			case SamplerFilter::Nearest: return VK_FILTER_NEAREST;
		}

		NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(samplerFilter), 16));
		return VK_FILTER_NEAREST;
	}

	inline VkSamplerMipmapMode ToVulkan(SamplerMipmapMode samplerMipmap)
	{
		switch (samplerMipmap)
		{
			case SamplerMipmapMode::Linear:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			case SamplerMipmapMode::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		}

		NazaraError("Unhandled SamplerMipmapMode 0x" + NumberToString(UnderlyingCast(samplerMipmap), 16));
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}

	inline VkSamplerAddressMode ToVulkan(SamplerWrap samplerWrap)
	{
		switch (samplerWrap)
		{
			case SamplerWrap::Clamp:          return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case SamplerWrap::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case SamplerWrap::Repeat:         return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}

		NazaraError("Unhandled SamplerWrap 0x" + NumberToString(UnderlyingCast(samplerWrap), 16));
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}

	inline VkDescriptorType ToVulkan(ShaderBindingType bindingType)
	{
		switch (bindingType)
		{
			case ShaderBindingType::Sampler:       return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case ShaderBindingType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case ShaderBindingType::Texture:       return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case ShaderBindingType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}

		NazaraError("Unhandled ShaderBindingType 0x" + NumberToString(UnderlyingCast(bindingType), 16));
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	}

	inline VkShaderStageFlagBits ToVulkan(nzsl::ShaderStageType stageType)
	{
		switch (stageType)
		{
			case nzsl::ShaderStageType::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
			case nzsl::ShaderStageType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case nzsl::ShaderStageType::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
		}

		NazaraError("Unhandled nzsl::ShaderStageType 0x" + NumberToString(UnderlyingCast(stageType), 16));
		return {};
	}

	inline VkShaderStageFlags ToVulkan(nzsl::ShaderStageTypeFlags stageType)
	{
		VkShaderStageFlags shaderStageBits = 0;
		for (int i = 0; i <= UnderlyingCast(nzsl::ShaderStageType::Max); ++i)
		{
			nzsl::ShaderStageType shaderStage = static_cast<nzsl::ShaderStageType>(i);
			if (stageType.Test(shaderStage))
				shaderStageBits |= ToVulkan(shaderStage);
		}

		return shaderStageBits;
	}

	inline VkStencilOp ToVulkan(StencilOperation stencilOp)
	{
		switch (stencilOp)
		{
			case StencilOperation::Decrement:        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case StencilOperation::DecrementNoClamp: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
			case StencilOperation::Increment:        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case StencilOperation::IncrementNoClamp: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case StencilOperation::Invert:           return VK_STENCIL_OP_INVERT;
			case StencilOperation::Keep:             return VK_STENCIL_OP_KEEP;
			case StencilOperation::Replace:          return VK_STENCIL_OP_REPLACE;
			case StencilOperation::Zero:             return VK_STENCIL_OP_ZERO;
		}

		NazaraError("Unhandled StencilOperation 0x" + NumberToString(UnderlyingCast(stencilOp), 16));
		return {};
	}

	inline VkImageLayout ToVulkan(TextureLayout textureLayout)
	{
		switch (textureLayout)
		{
			case TextureLayout::ColorInput:            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case TextureLayout::ColorOutput:           return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case TextureLayout::DepthStencilReadOnly:  return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			case TextureLayout::DepthStencilReadWrite: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			case TextureLayout::Present:               return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case TextureLayout::TransferSource:        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case TextureLayout::TransferDestination:   return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			case TextureLayout::Undefined:             return VK_IMAGE_LAYOUT_UNDEFINED;
		}

		NazaraError("Unhandled TextureLayout 0x" + NumberToString(UnderlyingCast(textureLayout), 16));
		return {};
	}

	inline VkImageUsageFlagBits ToVulkan(TextureUsage textureLayout)
	{
		switch (textureLayout)
		{
			case TextureUsage::ColorAttachment:        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			case TextureUsage::DepthStencilAttachment: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			case TextureUsage::InputAttachment:        return VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
			case TextureUsage::ShaderReadWrite:        return VK_IMAGE_USAGE_STORAGE_BIT;
			case TextureUsage::ShaderSampling:         return VK_IMAGE_USAGE_SAMPLED_BIT;
			case TextureUsage::TransferSource:         return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			case TextureUsage::TransferDestination:    return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		NazaraError("Unhandled TextureUsage 0x" + NumberToString(UnderlyingCast(textureLayout), 16));
		return {};
	}

	inline VkImageUsageFlags ToVulkan(TextureUsageFlags textureLayout)
	{
		VkImageUsageFlags imageUsageBits = 0;
		for (int i = 0; i <= UnderlyingCast(TextureUsage::Max); ++i)
		{
			TextureUsage textureUsage = static_cast<TextureUsage>(i);
			if (textureLayout.Test(textureUsage))
				imageUsageBits |= ToVulkan(textureUsage);
		}

		return imageUsageBits;
	}

	inline VkVertexInputRate ToVulkan(VertexInputRate inputRate)
	{
		switch (inputRate)
		{
			case VertexInputRate::Instance: return VK_VERTEX_INPUT_RATE_INSTANCE;
			case VertexInputRate::Vertex:   return VK_VERTEX_INPUT_RATE_VERTEX;
		}

		NazaraError("Unhandled VertexInputRate 0x" + NumberToString(UnderlyingCast(inputRate), 16));
		return {};
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
