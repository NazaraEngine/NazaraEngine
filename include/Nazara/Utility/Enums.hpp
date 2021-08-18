// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_UTILITY_HPP
#define NAZARA_ENUMS_UTILITY_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum class AnimationType
	{
		Skeletal,
		Static,

		Max = Static
	};

	enum class BlendEquation
	{
		Add,
		Max,
		Min,
		ReverseSubtract,
		Subtract,
	};

	enum class BlendFunc
	{
		ConstantColor,
		ConstantAlpha,
		DstAlpha,
		DstColor,
		SrcAlpha,
		SrcColor,
		InvConstantColor,
		InvConstantAlpha,
		InvDstAlpha,
		InvDstColor,
		InvSrcAlpha,
		InvSrcColor,
		One,
		Zero
	};

	enum class BufferAccess
	{
		DiscardAndWrite,
		ReadOnly,
		ReadWrite,
		WriteOnly,

		Max = WriteOnly
	};

	enum class BufferType
	{
		Index,
		Vertex,
		Uniform,

		Max = Uniform
	};

	enum class BufferUsage
	{
		DeviceLocal,
		DirectMapping,
		Dynamic,
		PersistentMapping,

		Max = DirectMapping
	};

	template<>
	struct EnumAsFlags<BufferUsage>
	{
		static constexpr BufferUsage max = BufferUsage::Max;
	};

	using BufferUsageFlags = Flags<BufferUsage>;

	enum class ComponentType
	{
		Color,
		Double1,
		Double2,
		Double3,
		Double4,
		Float1,
		Float2,
		Float3,
		Float4,
		Int1,
		Int2,
		Int3,
		Int4,
		Quaternion,

		Max = Quaternion
	};

	constexpr std::size_t ComponentTypeCount = static_cast<std::size_t>(ComponentType::Max) + 1;

	enum class CubemapFace
	{
		// This enumeration is intended to replace the "z" argument of Image's methods containing cubemap
		// The order is X, -X, Y, -Y, Z, -Z
		PositiveX = 0,
		PositiveY = 2,
		PositiveZ = 4,
		NegativeX = 1,
		NegativeY = 3,
		NegativeZ = 5,

		Max = NegativeZ
	};

	enum class DataStorage
	{
		Hardware,
		Software,

		Max = Software
	};

	constexpr std::size_t DataStorageCount = static_cast<std::size_t>(DataStorage::Max) + 1;

	enum class FaceFilling
	{
		Fill,
		Line,
		Point,

		Max = Point
	};

	enum class FaceSide
	{
		None,

		Back,
		Front,
		FrontAndBack,

		Max = FrontAndBack
	};

	enum class FrontFace
	{
		Clockwise,
		CounterClockwise,

		Max = CounterClockwise
	};

	enum class ImageType
	{
		E1D,
		E1D_Array,
		E2D,
		E2D_Array,
		E3D,
		Cubemap,

		Max = Cubemap
	};

	constexpr std::size_t ImageTypeCount = static_cast<std::size_t>(ImageType::Max) + 1;

	enum class NodeType
	{
		Default,  // Node
		Scene,    // SceneNode (Graphics)
		Skeletal, ///TODO

		Max = Skeletal
	};

	enum class PixelFormatContent
	{
		Undefined = -1,

		ColorRGBA,
		Depth,
		DepthStencil,
		Stencil,

		Max = Stencil
	};

	enum class PixelFormat
	{
		Undefined = -1,

		A8,              // 1*uint8
		BGR8,            // 3*uint8
		BGR8_SRGB,       // 3*uint8
		BGRA8,           // 4*uint8
		BGRA8_SRGB,      // 4*uint8
		DXT1,
		DXT3,
		DXT5,
		L8,              // 1*uint8
		LA8,             // 2*uint8
		R8,              // 1*uint8
		R8I,             // 1*int8
		R8UI,            // 1*uint8
		R16,             // 1*uint16
		R16F,            // 1*half
		R16I,            // 1*int16
		R16UI,           // 1*uint16
		R32F,            // 1*float
		R32I,            // 1*uint16
		R32UI,           // 1*uint32
		RG8,             // 2*int8
		RG8I,            // 2*int8
		RG8UI,           // 2*uint8
		RG16,            // 2*uint16
		RG16F,           // 2*half
		RG16I,           // 2*int16
		RG16UI,          // 2*uint16
		RG32F,           // 2*float
		RG32I,           // 2*uint16
		RG32UI,          // 2*uint32
		RGB5A1,          // 3*uint5 + alpha bit
		RGB8,            // 3*uint8
		RGB8_SRGB,       // 3*uint8
		RGB16F,          // 3*half
		RGB16I,          // 4*int16
		RGB16UI,         // 4*uint16
		RGB32F,          // 3*float
		RGB32I,          // 4*int32
		RGB32UI,         // 4*uint32
		RGBA4,           // 4*uint4
		RGBA8,           // 4*uint8
		RGBA8_SRGB,      // 4*uint8
		RGBA16F,         // 4*half
		RGBA16I,         // 4*int16
		RGBA16UI,        // 4*uint16
		RGBA32F,         // 4*float
		RGBA32I,         // 4*int32
		RGBA32UI,        // 4*uint32
		Depth16,
		Depth16Stencil8,
		Depth24,
		Depth24Stencil8,
		Depth32F,
		Depth32FStencil8,
		Stencil1,
		Stencil4,
		Stencil8,
		Stencil16,

		Max = Stencil16
	};

	constexpr std::size_t PixelFormatCount = static_cast<std::size_t>(PixelFormat::Max) + 1;

	enum class PixelFormatSubType
	{
		Compressed, // Opaque
		Double,     // F64
		Float,      // F32
		Half,       // F16
		Int,        //   Signed integer
		Unsigned,   // Unsigned integer

		Max = Unsigned
	};

	enum class PixelFlipping
	{
		Horizontally,
		Vertically,

		Max = Vertically
	};

	constexpr std::size_t PixelFlippingCount = static_cast<std::size_t>(PixelFlipping::Max) + 1;

	enum class PrimitiveMode
	{
		LineList,
		LineStrip,
		PointList,
		TriangleList,
		TriangleStrip,
		TriangleFan,

		Max = TriangleFan
	};

	enum class RendererComparison
	{
		Always,
		Equal,
		Greater,
		GreaterOrEqual,
		Less,
		LessOrEqual,
		Never,
		NotEqual,

		Max = NotEqual
	};

	enum class SamplerFilter
	{
		Linear,
		Nearest,

		Max = Nearest
	};

	enum class SamplerMipmapMode
	{
		Linear,
		Nearest,

		Max = Nearest
	};

	enum class SamplerWrap
	{
		Clamp,
		MirroredRepeat,
		Repeat,

		Max = Repeat
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

	enum class StructFieldType
	{
		Bool1,
		Bool2,
		Bool3,
		Bool4,
		Float1,
		Float2,
		Float3,
		Float4,
		Double1,
		Double2,
		Double3,
		Double4,
		Int1,
		Int2,
		Int3,
		Int4,
		UInt1,
		UInt2,
		UInt3,
		UInt4,

		Max = UInt4
	};

	enum class StructLayout
	{
		Packed,
		Std140,

		Max = Std140
	};

	enum class StencilOperation
	{
		Decrement,
		DecrementNoClamp,
		Increment,
		IncrementNoClamp,
		Invert,
		Keep,
		Replace,
		Zero,

		Max = Zero
	};

	enum class TextAlign
	{
		Left,
		Middle,
		Right,

		Max = Right
	};

	enum class TextStyle
	{
		Bold,
		Italic,
		StrikeThrough,
		Underlined,

		Max = Underlined
	};

	template<>
	struct EnumAsFlags<TextStyle>
	{
		static constexpr TextStyle max = TextStyle::Max;
	};

	using TextStyleFlags = Flags<TextStyle>;

	constexpr TextStyleFlags TextStyle_Regular = 0;

	enum class VertexComponent
	{
		Unused = -1,

		Color,
		Normal,
		Position,
		Tangent,
		TexCoord,
		Userdata,

		Max = Userdata
	};

	enum class VertexInputRate
	{
		Instance,
		Vertex
	};

	enum class VertexLayout
	{
		// Predefined declarations for rendering
		XY,
		XY_Color,
		XY_UV,
		XYZ,
		XYZ_Color,
		XYZ_Color_UV,
		XYZ_Normal,
		XYZ_Normal_UV,
		XYZ_Normal_UV_Tangent,
		XYZ_Normal_UV_Tangent_Skinning,
		XYZ_UV,

		// Predefined declarations for instancing
		Matrix4,

		Max = Matrix4
	};

	constexpr std::size_t VertexLayoutCount = static_cast<std::size_t>(VertexLayout::Max) + 1;
}

#endif // NAZARA_ENUMS_UTILITY_HPP
