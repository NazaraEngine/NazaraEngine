// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_UTILITY_HPP
#define NAZARA_ENUMS_UTILITY_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum AnimationType
	{
		AnimationType_Skeletal,
		AnimationType_Static,

		AnimationType_Max = AnimationType_Static
	};

	enum BlendFunc
	{
		BlendFunc_DestAlpha,
		BlendFunc_DestColor,
		BlendFunc_SrcAlpha,
		BlendFunc_SrcColor,
		BlendFunc_InvDestAlpha,
		BlendFunc_InvDestColor,
		BlendFunc_InvSrcAlpha,
		BlendFunc_InvSrcColor,
		BlendFunc_One,
		BlendFunc_Zero,

		BlendFunc_Max = BlendFunc_Zero
	};

	enum BufferAccess
	{
		BufferAccess_DiscardAndWrite,
		BufferAccess_ReadOnly,
		BufferAccess_ReadWrite,
		BufferAccess_WriteOnly,

		BufferAccess_Max = BufferAccess_WriteOnly
	};

	enum BufferType
	{
		BufferType_Index,
		BufferType_Vertex,
		BufferType_Uniform,

		BufferType_Max = BufferType_Uniform
	};

	enum BufferUsage
	{
		BufferUsage_DeviceLocal,
		BufferUsage_DirectMapping,
		BufferUsage_Dynamic,
		BufferUsage_PersistentMapping,

		BufferUsage_Max = BufferUsage_DirectMapping
	};

	template<>
	struct EnumAsFlags<BufferUsage>
	{
		static constexpr BufferUsage max = BufferUsage_Max;
	};

	using BufferUsageFlags = Flags<BufferUsage>;

	enum ComponentType
	{
		ComponentType_Color,
		ComponentType_Double1,
		ComponentType_Double2,
		ComponentType_Double3,
		ComponentType_Double4,
		ComponentType_Float1,
		ComponentType_Float2,
		ComponentType_Float3,
		ComponentType_Float4,
		ComponentType_Int1,
		ComponentType_Int2,
		ComponentType_Int3,
		ComponentType_Int4,
		ComponentType_Quaternion,

		ComponentType_Max = ComponentType_Quaternion
	};

	enum CubemapFace
	{
		// This enumeration is intended to replace the "z" argument of Image's methods containing cubemap
		// The order is X, -X, Y, -Y, Z, -Z
		CubemapFace_PositiveX = 0,
		CubemapFace_PositiveY = 2,
		CubemapFace_PositiveZ = 4,
		CubemapFace_NegativeX = 1,
		CubemapFace_NegativeY = 3,
		CubemapFace_NegativeZ = 5,

		CubemapFace_Max = CubemapFace_NegativeZ
	};

	enum DataStorage
	{
		DataStorage_Hardware,
		DataStorage_Software,

		DataStorage_Max = DataStorage_Software
	};

	enum FaceFilling
	{
		FaceFilling_Fill,
		FaceFilling_Line,
		FaceFilling_Point,

		FaceFilling_Max = FaceFilling_Point
	};

	enum FaceSide
	{
		FaceSide_None,

		FaceSide_Back,
		FaceSide_Front,
		FaceSide_FrontAndBack,

		FaceSide_Max = FaceSide_FrontAndBack
	};

	enum ImageType
	{
		ImageType_1D,
		ImageType_1D_Array,
		ImageType_2D,
		ImageType_2D_Array,
		ImageType_3D,
		ImageType_Cubemap,

		ImageType_Max = ImageType_Cubemap
	};

	enum NodeType
	{
		NodeType_Default,  // Node
		NodeType_Scene,    // SceneNode (Graphics)
		NodeType_Skeletal, ///TODO

		NodeType_Max = NodeType_Skeletal
	};

	enum PixelFormatContent
	{
		PixelFormatContent_Undefined = -1,

		PixelFormatContent_ColorRGBA,
		PixelFormatContent_DepthStencil,
		PixelFormatContent_Stencil,

		PixelFormatContent_Max = PixelFormatContent_Stencil
	};

	enum PixelFormat
	{
		PixelFormat_Undefined = -1,

		PixelFormat_A8,              // 1*uint8
		PixelFormat_BGR8,            // 3*uint8
		PixelFormat_BGRA8,           // 4*uint8
		PixelFormat_DXT1,
		PixelFormat_DXT3,
		PixelFormat_DXT5,
		PixelFormat_L8,              // 1*uint8
		PixelFormat_LA8,             // 2*uint8
		PixelFormat_R8,              // 1*uint8
		PixelFormat_R8I,             // 1*int8
		PixelFormat_R8UI,            // 1*uint8
		PixelFormat_R16,             // 1*uint16
		PixelFormat_R16F,            // 1*half
		PixelFormat_R16I,            // 1*int16
		PixelFormat_R16UI,           // 1*uint16
		PixelFormat_R32F,            // 1*float
		PixelFormat_R32I,            // 1*uint16
		PixelFormat_R32UI,           // 1*uint32
		PixelFormat_RG8,             // 2*int8
		PixelFormat_RG8I,            // 2*int8
		PixelFormat_RG8UI,           // 2*uint8
		PixelFormat_RG16,            // 2*uint16
		PixelFormat_RG16F,           // 2*half
		PixelFormat_RG16I,           // 2*int16
		PixelFormat_RG16UI,          // 2*uint16
		PixelFormat_RG32F,           // 2*float
		PixelFormat_RG32I,           // 2*uint16
		PixelFormat_RG32UI,          // 2*uint32
		PixelFormat_RGB5A1,          // 3*uint5 + alpha bit
		PixelFormat_RGB8,            // 3*uint8
		PixelFormat_RGB16F,          // 3*half
		PixelFormat_RGB16I,          // 4*int16
		PixelFormat_RGB16UI,         // 4*uint16
		PixelFormat_RGB32F,          // 3*float
		PixelFormat_RGB32I,          // 4*int32
		PixelFormat_RGB32UI,         // 4*uint32
		PixelFormat_RGBA4,           // 4*uint4
		PixelFormat_RGBA8,           // 4*uint8
		PixelFormat_RGBA16F,         // 4*half
		PixelFormat_RGBA16I,         // 4*int16
		PixelFormat_RGBA16UI,        // 4*uint16
		PixelFormat_RGBA32F,         // 4*float
		PixelFormat_RGBA32I,         // 4*int32
		PixelFormat_RGBA32UI,        // 4*uint32
		PixelFormat_Depth16,
		PixelFormat_Depth24,
		PixelFormat_Depth24Stencil8,
		PixelFormat_Depth32,
		PixelFormat_Stencil1,
		PixelFormat_Stencil4,
		PixelFormat_Stencil8,
		PixelFormat_Stencil16,

		PixelFormat_Max = PixelFormat_Stencil16
	};

	enum PixelFormatSubType
	{
		PixelFormatSubType_Compressed, // Opaque
		PixelFormatSubType_Double,     // F64
		PixelFormatSubType_Float,      // F32
		PixelFormatSubType_Half,       // F16
		PixelFormatSubType_Int,        //   Signed integer
		PixelFormatSubType_Unsigned,   // Unsigned integer

		PixelFormatSubType_Max = PixelFormatSubType_Unsigned
	};

	enum PixelFlipping
	{
		PixelFlipping_Horizontally,
		PixelFlipping_Vertically,

		PixelFlipping_Max = PixelFlipping_Vertically
	};

	enum PrimitiveMode
	{
		PrimitiveMode_LineList,
		PrimitiveMode_LineStrip,
		PrimitiveMode_PointList,
		PrimitiveMode_TriangleList,
		PrimitiveMode_TriangleStrip,
		PrimitiveMode_TriangleFan,

		PrimitiveMode_Max = PrimitiveMode_TriangleFan
	};

	enum RendererComparison
	{
		RendererComparison_Always,
		RendererComparison_Equal,
		RendererComparison_Greater,
		RendererComparison_GreaterOrEqual,
		RendererComparison_Less,
		RendererComparison_LessOrEqual,
		RendererComparison_Never,
		RendererComparison_NotEqual,

		RendererComparison_Max = RendererComparison_NotEqual
	};

	enum RendererParameter
	{
		RendererParameter_Blend,
		RendererParameter_ColorWrite,
		RendererParameter_DepthBuffer,
		RendererParameter_DepthWrite,
		RendererParameter_FaceCulling,
		RendererParameter_ScissorTest,
		RendererParameter_StencilTest,

		RendererParameter_Max = RendererParameter_StencilTest
	};

	enum SamplerFilter
	{
		SamplerFilter_Linear,
		SamplerFilter_Nearest,

		SamplerFilter_Max = SamplerFilter_Nearest
	};

	enum SamplerMipmapMode
	{
		SamplerMipmapMode_Linear,
		SamplerMipmapMode_Nearest,

		SamplerMipmapMode_Max = SamplerMipmapMode_Nearest
	};

	enum SamplerWrap
	{
		SamplerWrap_Clamp,
		SamplerWrap_MirroredRepeat,
		SamplerWrap_Repeat,

		SamplerWrap_Max = SamplerWrap_Repeat
	};

	enum class ShaderStageType
	{
		Fragment,
		Vertex,

		Max = Vertex
	};

	constexpr std::size_t ShaderStageTypeCount = static_cast<std::size_t>(ShaderStageType::Max) + 1;

	template<>
	struct EnumAsFlags<ShaderStageType>
	{
		static constexpr ShaderStageType max = ShaderStageType::Max;
	};

	using ShaderStageTypeFlags = Flags<ShaderStageType>;

	constexpr ShaderStageTypeFlags ShaderStageType_All = ShaderStageType::Fragment | ShaderStageType::Vertex;

	enum StructFieldType
	{
		StructFieldType_Bool1,
		StructFieldType_Bool2,
		StructFieldType_Bool3,
		StructFieldType_Bool4,
		StructFieldType_Float1,
		StructFieldType_Float2,
		StructFieldType_Float3,
		StructFieldType_Float4,
		StructFieldType_Double1,
		StructFieldType_Double2,
		StructFieldType_Double3,
		StructFieldType_Double4,
		StructFieldType_Int1,
		StructFieldType_Int2,
		StructFieldType_Int3,
		StructFieldType_Int4,
		StructFieldType_UInt1,
		StructFieldType_UInt2,
		StructFieldType_UInt3,
		StructFieldType_UInt4,

		StructFieldType_Max = StructFieldType_UInt4
	};

	enum StructLayout
	{
		StructLayout_Packed,
		StructLayout_Std140,

		StructLayout_Max = StructLayout_Std140
	};

	enum StencilOperation
	{
		StencilOperation_Decrement,
		StencilOperation_DecrementNoClamp,
		StencilOperation_Increment,
		StencilOperation_IncrementNoClamp,
		StencilOperation_Invert,
		StencilOperation_Keep,
		StencilOperation_Replace,
		StencilOperation_Zero,

		StencilOperation_Max = StencilOperation_Zero
	};

	enum TextAlign
	{
		TextAlign_Left,
		TextAlign_Middle,
		TextAlign_Right,

		TextAlign_Max = TextAlign_Right
	};

	enum TextStyle
	{
		TextStyle_Bold,
		TextStyle_Italic,
		TextStyle_StrikeThrough,
		TextStyle_Underlined,

		TextStyle_Max = TextStyle_Underlined
	};

	template<>
	struct EnumAsFlags<TextStyle>
	{
		static constexpr TextStyle max = TextStyle_Max;
	};

	using TextStyleFlags = Flags<TextStyle>;

	constexpr TextStyleFlags TextStyle_Regular = 0;

	enum VertexComponent
	{
		VertexComponent_Unused = -1,

		VertexComponent_Color,
		VertexComponent_Normal,
		VertexComponent_Position,
		VertexComponent_Tangent,
		VertexComponent_TexCoord,
		VertexComponent_Userdata,

		VertexComponent_Max = VertexComponent_Userdata
	};

	enum class VertexInputRate
	{
		Instance,
		Vertex
	};

	enum VertexLayout
	{
		// Predefined declarations for rendering
		VertexLayout_XY,
		VertexLayout_XY_Color,
		VertexLayout_XY_UV,
		VertexLayout_XYZ,
		VertexLayout_XYZ_Color,
		VertexLayout_XYZ_Color_UV,
		VertexLayout_XYZ_Normal,
		VertexLayout_XYZ_Normal_UV,
		VertexLayout_XYZ_Normal_UV_Tangent,
		VertexLayout_XYZ_Normal_UV_Tangent_Skinning,
		VertexLayout_XYZ_UV,

		// Predefined declarations for instancing
		VertexLayout_Matrix4,

		VertexLayout_Max = VertexLayout_Matrix4
	};
}

#endif // NAZARA_ENUMS_UTILITY_HPP
