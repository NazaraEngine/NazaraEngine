// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VkFormat ToVulkan(ComponentType componentType)
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

		NazaraError("Unhandled ComponentType 0x" + String::Number(componentType, 16));
		return VK_FORMAT_UNDEFINED;
	}

	VkCullModeFlagBits ToVulkan(FaceSide faceSide)
	{
		switch (faceSide)
		{
			case FaceSide_None:         return VK_CULL_MODE_NONE;
			case FaceSide_Back:         return VK_CULL_MODE_BACK_BIT;
			case FaceSide_Front:        return VK_CULL_MODE_FRONT_BIT;
			case FaceSide_FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
		}

		NazaraError("Unhandled FaceSide 0x" + String::Number(faceSide, 16));
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

		NazaraError("Unhandled FaceFilling 0x" + String::Number(faceFilling, 16));
		return VK_POLYGON_MODE_FILL;
	}

	VkPrimitiveTopology ToVulkan(PrimitiveMode primitiveMode)
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

		NazaraError("Unhandled FaceFilling 0x" + String::Number(primitiveMode, 16));
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

		NazaraError("Unhandled RendererComparison 0x" + String::Number(comparison, 16));
		return VK_COMPARE_OP_NEVER;
	}

	VkShaderStageFlagBits ToVulkan(ShaderStageType stageType)
	{
		switch (stageType)
		{
			case ShaderStageType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderStageType::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
		}

		NazaraError("Unhandled ShaderStageType 0x" + String::Number(static_cast<std::size_t>(stageType), 16));
		return {};
	}

	VkStencilOp ToVulkan(StencilOperation stencilOp)
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

		NazaraError("Unhandled RendererComparison 0x" + String::Number(stencilOp, 16));
		return VK_STENCIL_OP_KEEP;
	}

	VkVertexInputRate ToVulkan(VertexInputRate inputRate)
	{
		switch (inputRate)
		{
			case VertexInputRate::Instance: return VK_VERTEX_INPUT_RATE_INSTANCE;
			case VertexInputRate::Vertex:   return VK_VERTEX_INPUT_RATE_VERTEX;
		}

		NazaraError("Unhandled VertexInputRate 0x" + String::Number(UnderlyingCast(inputRate), 16));
		return VK_VERTEX_INPUT_RATE_VERTEX;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
