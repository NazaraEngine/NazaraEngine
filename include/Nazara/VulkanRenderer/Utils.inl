// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	std::optional<PixelFormat> FromVulkan(VkFormat format)
	{
		switch (format)
		{
			case VK_FORMAT_B8G8R8A8_UNORM: return PixelFormat::PixelFormat_BGRA8;
			case VK_FORMAT_B8G8R8A8_SRGB: return PixelFormat::PixelFormat_BGRA8_SRGB;
			case VK_FORMAT_D24_UNORM_S8_UINT: return PixelFormat::PixelFormat_Depth24Stencil8;
			case VK_FORMAT_D32_SFLOAT: return PixelFormat::PixelFormat_Depth32;
			case VK_FORMAT_R8G8B8A8_UNORM: return PixelFormat::PixelFormat_RGBA8;
			case VK_FORMAT_R8G8B8A8_SRGB: return PixelFormat::PixelFormat_RGBA8_SRGB;
			default: break;
		}

		return std::nullopt;
	}

	VkAttachmentLoadOp ToVulkan(AttachmentLoadOp loadOp)
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

	VkAttachmentStoreOp ToVulkan(AttachmentStoreOp storeOp)
	{
		switch (storeOp)
		{
			case AttachmentStoreOp::Discard: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case AttachmentStoreOp::Store:   return VK_ATTACHMENT_STORE_OP_STORE;
		}

		NazaraError("Unhandled AttachmentStoreOp 0x" + NumberToString(UnderlyingCast(storeOp), 16));
		return {};
	}

	inline VkBufferUsageFlags ToVulkan(BufferType bufferType)
	{
		switch (bufferType)
		{
			case BufferType_Index: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case BufferType_Vertex: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case BufferType_Uniform: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}

		NazaraError("Unhandled BufferType 0x" + NumberToString(bufferType, 16));
		return 0;
	}

	inline VkFormat ToVulkan(ComponentType componentType)
	{
		switch (componentType)
		{
			case ComponentType_Color:      return VK_FORMAT_R8G8B8A8_UINT;
			case ComponentType_Double1:    return VK_FORMAT_R64_SFLOAT;
			case ComponentType_Double2:    return VK_FORMAT_R64G64_SFLOAT;
			case ComponentType_Double3:    return VK_FORMAT_R64G64B64_SFLOAT;
			case ComponentType_Double4:    return VK_FORMAT_R64G64B64A64_SFLOAT;
			case ComponentType_Float1:     return VK_FORMAT_R32_SFLOAT;
			case ComponentType_Float2:     return VK_FORMAT_R32G32_SFLOAT;
			case ComponentType_Float3:     return VK_FORMAT_R32G32B32_SFLOAT;
			case ComponentType_Float4:     return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ComponentType_Int1:       return VK_FORMAT_R32_SINT;
			case ComponentType_Int2:       return VK_FORMAT_R32G32_SINT;
			case ComponentType_Int3:       return VK_FORMAT_R32G32B32_SINT;
			case ComponentType_Int4:       return VK_FORMAT_R32G32B32A32_SINT;
			case ComponentType_Quaternion: return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		NazaraError("Unhandled ComponentType 0x" + NumberToString(componentType, 16));
		return VK_FORMAT_UNDEFINED;
	}

	inline VkCullModeFlagBits ToVulkan(FaceSide faceSide)
	{
		switch (faceSide)
		{
			case FaceSide_None:         return VK_CULL_MODE_NONE;
			case FaceSide_Back:         return VK_CULL_MODE_BACK_BIT;
			case FaceSide_Front:        return VK_CULL_MODE_FRONT_BIT;
			case FaceSide_FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
		}

		NazaraError("Unhandled FaceSide 0x" + NumberToString(faceSide, 16));
		return VK_CULL_MODE_BACK_BIT;
	}

	inline VkPolygonMode ToVulkan(FaceFilling faceFilling)
	{
		switch (faceFilling)
		{
			case FaceFilling_Fill:  return VK_POLYGON_MODE_FILL;
			case FaceFilling_Line:  return VK_POLYGON_MODE_LINE;
			case FaceFilling_Point: return VK_POLYGON_MODE_POINT;
		}

		NazaraError("Unhandled FaceFilling 0x" + NumberToString(faceFilling, 16));
		return VK_POLYGON_MODE_FILL;
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

	VkPipelineStageFlagBits ToVulkan(PipelineStage pipelineStage)
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

	VkPipelineStageFlags ToVulkan(PipelineStageFlags pipelineStages)
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

	VkFormat ToVulkan(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
			case PixelFormat::PixelFormat_BGRA8: return VK_FORMAT_B8G8R8A8_UNORM;
			case PixelFormat::PixelFormat_BGRA8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
			case PixelFormat::PixelFormat_Depth24Stencil8: return VK_FORMAT_D24_UNORM_S8_UINT;
			case PixelFormat::PixelFormat_Depth32: return VK_FORMAT_D32_SFLOAT;
			case PixelFormat::PixelFormat_RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
			case PixelFormat::PixelFormat_RGBA8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
			default: break;
		}

		NazaraError("Unhandled PixelFormat 0x" + NumberToString(pixelFormat, 16));
		return {};
	}

	inline VkPrimitiveTopology ToVulkan(PrimitiveMode primitiveMode)
	{
		switch (primitiveMode)
		{
			case PrimitiveMode_LineList:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case PrimitiveMode_LineStrip:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case PrimitiveMode_PointList:     return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case PrimitiveMode_TriangleList:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case PrimitiveMode_TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case PrimitiveMode_TriangleFan:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		}

		NazaraError("Unhandled FaceFilling 0x" + NumberToString(primitiveMode, 16));
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	inline VkCompareOp ToVulkan(RendererComparison comparison)
	{
		switch (comparison)
		{
			case RendererComparison_Never:          return VK_COMPARE_OP_NEVER; 
			case RendererComparison_Less:           return VK_COMPARE_OP_LESS; 
			case RendererComparison_Equal:          return VK_COMPARE_OP_EQUAL; 
			case RendererComparison_LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL; 
			case RendererComparison_Greater:        return VK_COMPARE_OP_GREATER; 
			case RendererComparison_NotEqual:       return VK_COMPARE_OP_NOT_EQUAL; 
			case RendererComparison_GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL; 
			case RendererComparison_Always:         return VK_COMPARE_OP_ALWAYS; 
		}

		NazaraError("Unhandled RendererComparison 0x" + NumberToString(comparison, 16));
		return VK_COMPARE_OP_NEVER;
	}

	inline VkFilter ToVulkan(SamplerFilter samplerFilter)
	{
		switch (samplerFilter)
		{
			case SamplerFilter_Linear:  return VK_FILTER_LINEAR;
			case SamplerFilter_Nearest: return VK_FILTER_NEAREST;
		}

		NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(samplerFilter), 16));
		return VK_FILTER_NEAREST;
	}

	inline VkSamplerMipmapMode ToVulkan(SamplerMipmapMode samplerMipmap)
	{
		switch (samplerMipmap)
		{
			case SamplerMipmapMode_Linear:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			case SamplerMipmapMode_Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		}

		NazaraError("Unhandled SamplerMipmapMode 0x" + NumberToString(UnderlyingCast(samplerMipmap), 16));
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}

	inline VkSamplerAddressMode ToVulkan(SamplerWrap samplerWrap)
	{
		switch (samplerWrap)
		{
			case SamplerWrap_Clamp:          return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case SamplerWrap_MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case SamplerWrap_Repeat:         return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}

		NazaraError("Unhandled SamplerWrap 0x" + NumberToString(UnderlyingCast(samplerWrap), 16));
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}

	inline VkDescriptorType ToVulkan(ShaderBindingType bindingType)
	{
		switch (bindingType)
		{
			case ShaderBindingType::Texture:       return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case ShaderBindingType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}

		NazaraError("Unhandled ShaderBindingType 0x" + NumberToString(UnderlyingCast(bindingType), 16));
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	}

	inline VkShaderStageFlagBits ToVulkan(ShaderStageType stageType)
	{
		switch (stageType)
		{
			case ShaderStageType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderStageType::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
		}

		NazaraError("Unhandled ShaderStageType 0x" + NumberToString(UnderlyingCast(stageType), 16));
		return {};
	}

	inline VkShaderStageFlags ToVulkan(ShaderStageTypeFlags stageType)
	{
		VkShaderStageFlags shaderStageBits = 0;
		for (int i = 0; i <= UnderlyingCast(ShaderStageType::Max); ++i)
		{
			ShaderStageType shaderStage = static_cast<ShaderStageType>(i);
			if (stageType.Test(shaderStage))
				shaderStageBits |= ToVulkan(shaderStage);
		}

		return shaderStageBits;
	}

	inline VkStencilOp ToVulkan(StencilOperation stencilOp)
	{
		switch (stencilOp)
		{
			case StencilOperation_Decrement:        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case StencilOperation_DecrementNoClamp: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
			case StencilOperation_Increment:        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case StencilOperation_IncrementNoClamp: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case StencilOperation_Invert:           return VK_STENCIL_OP_INVERT;
			case StencilOperation_Keep:             return VK_STENCIL_OP_KEEP;
			case StencilOperation_Replace:          return VK_STENCIL_OP_REPLACE;
			case StencilOperation_Zero:             return VK_STENCIL_OP_ZERO;
		}

		NazaraError("Unhandled StencilOperation 0x" + NumberToString(stencilOp, 16));
		return {};
	}

	VkImageLayout ToVulkan(TextureLayout textureLayout)
	{
		switch (textureLayout)
		{
			case TextureLayout::ColorInput:          return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case TextureLayout::ColorOutput:         return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case TextureLayout::DepthStencilInput:   return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			case TextureLayout::DepthStencilOutput:  return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			case TextureLayout::Present:             return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case TextureLayout::TransferSource:      return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case TextureLayout::TransferDestination: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			case TextureLayout::Undefined:           return VK_IMAGE_LAYOUT_UNDEFINED;
		}

		NazaraError("Unhandled TextureLayout 0x" + NumberToString(UnderlyingCast(textureLayout), 16));
		return {};
	}

	VkImageUsageFlagBits ToVulkan(TextureUsage textureLayout)
	{
		switch (textureLayout)
		{
			case TextureUsage::ColorOutput:         return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			case TextureUsage::DepthStencilOutput:  return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			case TextureUsage::InputAttachment:     return VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
			case TextureUsage::ShaderSampling:      return VK_IMAGE_USAGE_SAMPLED_BIT;
			case TextureUsage::TransferSource:      return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			case TextureUsage::TransferDestination: return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		NazaraError("Unhandled TextureUsage 0x" + NumberToString(UnderlyingCast(textureLayout), 16));
		return {};
	}

	VkImageUsageFlags ToVulkan(TextureUsageFlags textureLayout)
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
