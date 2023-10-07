// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_ENUMS_HPP
#define NAZARA_UTILITY_ENUMS_HPP

#include <NazaraUtils/Flags.hpp>

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
		Storage,
		Uniform,
		Upload,

		Max = Upload
	};

	enum class BufferUsage
	{
		DeviceLocal,
		DirectMapping,
		Dynamic,
		Read,
		PersistentMapping,
		Write,

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

		Max = Int4
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

	template<>
	struct EnumAsFlags<DataStorage>
	{
		static constexpr DataStorage max = DataStorage::Max;
	};

	using DataStoreFlags = Flags<DataStorage>;
	constexpr std::size_t DataStorageCount = static_cast<std::size_t>(DataStorage::Max) + 1;

	enum class FaceFilling
	{
		Fill,
		Line,
		Point,

		Max = Point
	};

	enum class FaceCulling
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

	enum class IndexType
	{
		U8,
		U16,
		U32,

		Max = U32
	};

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

		/***** Regular 8bits formats *****/

		// A
		A8Sint,
		A8Snorm,
		A8Uint,
		A8Unorm,

		// L
		L8Sint,
		L8Snorm,
		L8Uint,
		L8Unorm,
		L8Unorm_sRGB,

		// R
		R8Sint,
		R8Snorm,
		R8Uint,
		R8Unorm,
		R8Unorm_sRGB,

		/***** Regular 16bits formats *****/

		// R
		R16Float,
		R16Sint,
		R16Snorm,
		R16Uint,
		R16Unorm,
		R16Unorm_sRGB,

		// RG
		RG8Sint,
		RG8Snorm,
		RG8Uint,
		RG8Unorm,
		RG8Unorm_sRGB,

		// LA
		LA8Unorm,

		/***** Regular 24bits formats *****/

		// BGR
		BGR8Sint,
		BGR8Snorm,
		BGR8Uint,
		BGR8Unorm,
		BGR8Unorm_sRGB,

		// RGB
		RGB8Sint,
		RGB8Snorm,
		RGB8Uint,
		RGB8Unorm,
		RGB8Unorm_sRGB,

		/***** Regular 32bits formats *****/ 

		// R
		R32Sint,
		R32Snorm,
		R32Float,
		R32Uint,
		R32Unorm,
		R32Unorm_sRGB,

		// RG
		RG16Sint,
		RG16Snorm,
		RG16Float,
		RG16Uint,
		RG16Unorm,
		RG16Unorm_sRGB,

		// RGBA
		RGBA8Sint,
		RGBA8Snorm,
		RGBA8Uint,
		RGBA8Unorm,
		RGBA8Unorm_sRGB,

		// BGRA
		BGRA8Sint,
		BGRA8Snorm,
		BGRA8Uint,
		BGRA8Unorm,
		BGRA8Unorm_sRGB,

		/***** Regular 48bits formats *****/
		RGB16Float,
		RGB16Sint,
		RGB16Snorm,
		RGB16Uint,
		RGB16Unorm,

		/***** Regular 64bits formats *****/

		// RG
		RG32Float,
		RG32Sint,
		RG32Uint,

		// RGBA
		RGBA16Float,
		RGBA16Sint,
		RGBA16Snorm,
		RGBA16Uint,
		RGBA16Unorm,

		/***** Regular 96bits formats *****/
		RGB32Float,
		RGB32Sint,
		RGB32Snorm,
		RGB32Uint,
		RGB32Unorm,

		/***** Regular 128bits formats *****/
		RGBA32Float,
		RGBA32Sint,
		RGBA32Snorm,
		RGBA32Uint,
		RGBA32Unorm,

		/***** Packed 16bits formats *****/
		A1BGR5Unorm,
		ABGR4Unorm,
		BGRA4Unorm,
		B5G6R5Unorm,
		BGR5A1Unorm,
		RGBA4Unorm,
		RGB5A1Unorm,

		/***** Packed 32bits formats *****/
		BGR10A2Unorm,
		RGB10A2Uint,
		RGB10A2Unorm,
		RG11B10Float,
		RGB9E5Float,

		/***** BC formats (block, previously called DXT) *****/
		BC1,
		BC1_sRGB,
		BC2,
		BC2_sRGB,
		BC3,
		BC3_sRGB,
		BC4_RSnorm,
		BC4_RUnorm,
		BC5_RGSnorm,
		BC5_RGUnorm,
		BC6H_RGBFloat,
		BC7,
		BC7_sRGB,

		/***** EAC formats (block) *****/
		EAC_R11Snorm,
		EAC_R11Unorm,
		EAC_RG11Snorm,
		EAC_RG11Unorm,

		/***** ETC2 formats (block) *****/
		ETC2_RGB8,
		ETC2_RGB8_sRGB,
		ETC2_RGBA1,
		ETC2_RGBA1_sRGB,
		ETC2_RGBA8,
		ETC2_RGBA8_sRGB,

		/***** Depth and stencil formats *****/
		Depth16Float,
		Depth16Unorm,
		Depth16Unorm_Stencil8,
		Depth24Unorm,
		Depth24Unorm_Stencil8,
		Depth32Float,
		Depth32Float_Stencil8,
		Stencil8,

		Max = Stencil8
	};

	constexpr std::size_t PixelFormatCount = static_cast<std::size_t>(PixelFormat::Max) + 1;

	enum class PixelFormatSubType
	{
		Compressed, // Opaque
		Double,     // f64
		Float,      // f32
		Half,       // f16
		Int,        // int
		Unsigned,   // uint

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

	constexpr TextStyleFlags TextStyle_Regular = TextStyleFlags{};

	enum class VertexComponent
	{
		Unused = -1,

		Color,
		JointIndices,
		JointWeights,
		Normal,
		Position,
		SizeSinCos,
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
		UV_SizeSinCos,
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
		UV_SizeSinCos_Color,
		XYZ_UV,

		// Predefined declarations for instancing
		Matrix4,

		Max = Matrix4
	};

	constexpr std::size_t VertexLayoutCount = static_cast<std::size_t>(VertexLayout::Max) + 1;
}

#endif // NAZARA_UTILITY_ENUMS_HPP
