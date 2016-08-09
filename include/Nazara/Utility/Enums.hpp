// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_UTILITY_HPP
#define NAZARA_ENUMS_UTILITY_HPP

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

		BufferType_Max = BufferType_Vertex
	};

	enum BufferUsage
	{
		BufferUsage_Dynamic,
		BufferUsage_Static,

		BufferUsage_Max = BufferUsage_Static
	};

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
		// Cette énumération est prévue pour remplacer l'argument "z" des méthodes de Image contenant un cubemap
		// L'ordre est X, -X, Y, -Y, Z, -Z
		CubemapFace_PositiveX = 0,
		CubemapFace_PositiveY = 2,
		CubemapFace_PositiveZ = 4,
		CubemapFace_NegativeX = 1,
		CubemapFace_NegativeY = 3,
		CubemapFace_NegativeZ = 5,

		CubemapFace_Max = CubemapFace_NegativeZ
	};

	enum DataStorageFlags
	{
		DataStorage_Hardware = 0x1,
		DataStorage_Software = 0x2,

		DataStorage_Both = DataStorage_Hardware | DataStorage_Software,

		DataStorage_Max = DataStorage_Software*2-1
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

	enum PixelFormatType
	{
		PixelFormatType_Undefined = -1,

		PixelFormatType_A8,              // 1*uint8
		PixelFormatType_BGR8,            // 3*uint8
		PixelFormatType_BGRA8,           // 4*uint8
		PixelFormatType_DXT1,
		PixelFormatType_DXT3,
		PixelFormatType_DXT5,
		PixelFormatType_L8,              // 1*uint8
		PixelFormatType_LA8,             // 2*uint8
		PixelFormatType_R8,              // 1*uint8
		PixelFormatType_R8I,             // 1*int8
		PixelFormatType_R8UI,            // 1*uint8
		PixelFormatType_R16,             // 1*uint16
		PixelFormatType_R16F,            // 1*half
		PixelFormatType_R16I,            // 1*int16
		PixelFormatType_R16UI,           // 1*uint16
		PixelFormatType_R32F,            // 1*float
		PixelFormatType_R32I,            // 1*uint16
		PixelFormatType_R32UI,           // 1*uint32
		PixelFormatType_RG8,             // 2*int8
		PixelFormatType_RG8I,            // 2*int8
		PixelFormatType_RG8UI,           // 2*uint8
		PixelFormatType_RG16,            // 2*uint16
		PixelFormatType_RG16F,           // 2*half
		PixelFormatType_RG16I,           // 2*int16
		PixelFormatType_RG16UI,          // 2*uint16
		PixelFormatType_RG32F,           // 2*float
		PixelFormatType_RG32I,           // 2*uint16
		PixelFormatType_RG32UI,          // 2*uint32
		PixelFormatType_RGB5A1,          // 3*uint5 + alpha bit
		PixelFormatType_RGB8,            // 3*uint8
		PixelFormatType_RGB16F,          // 3*half
		PixelFormatType_RGB16I,          // 4*int16
		PixelFormatType_RGB16UI,         // 4*uint16
		PixelFormatType_RGB32F,          // 3*float
		PixelFormatType_RGB32I,          // 4*int32
		PixelFormatType_RGB32UI,         // 4*uint32
		PixelFormatType_RGBA4,           // 4*uint4
		PixelFormatType_RGBA8,           // 4*uint8
		PixelFormatType_RGBA16F,         // 4*half
		PixelFormatType_RGBA16I,         // 4*int16
		PixelFormatType_RGBA16UI,        // 4*uint16
		PixelFormatType_RGBA32F,         // 4*float
		PixelFormatType_RGBA32I,         // 4*int32
		PixelFormatType_RGBA32UI,        // 4*uint32
		PixelFormatType_Depth16,
		PixelFormatType_Depth24,
		PixelFormatType_Depth24Stencil8,
		PixelFormatType_Depth32,
		PixelFormatType_Stencil1,
		PixelFormatType_Stencil4,
		PixelFormatType_Stencil8,
		PixelFormatType_Stencil16,

		PixelFormatType_Max = PixelFormatType_Stencil16
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
		SamplerFilter_Unknown = -1,

		SamplerFilter_Bilinear,
		SamplerFilter_Nearest,
		SamplerFilter_Trilinear,

		SamplerFilter_Default,

		SamplerFilter_Max = SamplerFilter_Default
	};

	enum SamplerWrap
	{
		SamplerWrap_Unknown = -1,

		SamplerWrap_Clamp,
		SamplerWrap_MirroredRepeat,
		SamplerWrap_Repeat,

		SamplerWrap_Default,

		SamplerWrap_Max = SamplerWrap_Repeat
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

	enum TextStyleFlags
	{
		TextStyle_Regular = 0x0,

		TextStyle_Bold          = 0x1,
		TextStyle_Italic        = 0x2,
		TextStyle_StrikeThrough = 0x4,
		TextStyle_Underlined    = 0x8,

		TextStyle_Max = TextStyle_Underlined*2-1
	};

	enum VertexComponent
	{
		VertexComponent_Unused = -1,

		// Nous nous limitons à 16 composants de sommets car c'est le minimum supporté par le GPU
		VertexComponent_InstanceData0,
		VertexComponent_InstanceData1,
		VertexComponent_InstanceData2,
		VertexComponent_InstanceData3,
		VertexComponent_InstanceData4,
		VertexComponent_InstanceData5,
		VertexComponent_Color,
		VertexComponent_Normal,
		VertexComponent_Position,
		VertexComponent_Tangent,
		VertexComponent_TexCoord,
		VertexComponent_Userdata0,
		VertexComponent_Userdata1,
		VertexComponent_Userdata2,
		VertexComponent_Userdata3,
		VertexComponent_Userdata4,

		VertexComponent_FirstInstanceData = VertexComponent_InstanceData0,
		VertexComponent_FirstVertexData = VertexComponent_Color,
		VertexComponent_LastInstanceData = VertexComponent_InstanceData5,
		VertexComponent_LastVertexData = VertexComponent_Userdata4,

		VertexComponent_Max = VertexComponent_Userdata4
	};

	enum VertexLayout
	{
		// Déclarations destinées au rendu
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

		// Déclarations destinées à l'instancing
		VertexLayout_Matrix4,

		VertexLayout_Max = VertexLayout_Matrix4
	};

	enum WindowCursor
	{
		WindowCursor_None,
		WindowCursor_Default,

		WindowCursor_Crosshair,
		WindowCursor_Hand,
		WindowCursor_Help,
		WindowCursor_Move,
		WindowCursor_Pointer,
		WindowCursor_Progress,
		WindowCursor_ResizeE,
		WindowCursor_ResizeN,
		WindowCursor_ResizeNE,
		WindowCursor_ResizeNW,
		WindowCursor_ResizeS,
		WindowCursor_ResizeSE,
		WindowCursor_ResizeSW,
		WindowCursor_ResizeW,
		WindowCursor_Text,
		WindowCursor_Wait,

		WindowCursor_Max = WindowCursor_Wait
	};

	enum WindowEventType
	{
		WindowEventType_GainedFocus,
		WindowEventType_LostFocus,
		WindowEventType_KeyPressed,
		WindowEventType_KeyReleased,
		WindowEventType_MouseButtonDoubleClicked,
		WindowEventType_MouseButtonPressed,
		WindowEventType_MouseButtonReleased,
		WindowEventType_MouseEntered,
		WindowEventType_MouseLeft,
		WindowEventType_MouseMoved,
		WindowEventType_MouseWheelMoved,
		WindowEventType_Moved,
		WindowEventType_Quit,
		WindowEventType_Resized,
		WindowEventType_TextEntered,

		WindowEventType_Max = WindowEventType_TextEntered
	};

	enum WindowStyleFlags
	{
		WindowStyle_None       = 0x0,
		WindowStyle_Fullscreen = 0x1,

		WindowStyle_Closable   = 0x2,
		WindowStyle_Resizable  = 0x4,
		WindowStyle_Titlebar   = 0x8,

		WindowStyle_Default = WindowStyle_Closable | WindowStyle_Resizable | WindowStyle_Titlebar,
		WindowStyle_Max = WindowStyle_Titlebar*2-1
	};
}

#endif // NAZARA_ENUMS_UTILITY_HPP
