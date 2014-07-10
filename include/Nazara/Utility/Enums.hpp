// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_UTILITY_HPP
#define NAZARA_ENUMS_UTILITY_HPP

enum nzAnimationType
{
	nzAnimationType_Skeletal,
	nzAnimationType_Static,

	nzAnimationType_Max = nzAnimationType_Static
};

enum nzBufferAccess
{
	nzBufferAccess_DiscardAndWrite,
	nzBufferAccess_ReadOnly,
	nzBufferAccess_ReadWrite,
	nzBufferAccess_WriteOnly,

	nzBufferAccess_Max = nzBufferAccess_WriteOnly
};

enum nzBufferStorage
{
	//nzBufferStorage_Both, ///TODO
	nzBufferStorage_Hardware,
	nzBufferStorage_Software,

	nzBufferStorage_Max = nzBufferStorage_Software
};

enum nzBufferType
{
	nzBufferType_Index,
	nzBufferType_Vertex,

	nzBufferType_Max = nzBufferType_Vertex
};

enum nzBufferUsage
{
	nzBufferUsage_Dynamic,
	nzBufferUsage_Static,

	nzBufferUsage_Max = nzBufferUsage_Static
};

enum nzComponentType
{
	nzComponentType_Color,
	nzComponentType_Double1,
	nzComponentType_Double2,
	nzComponentType_Double3,
	nzComponentType_Double4,
	nzComponentType_Float1,
	nzComponentType_Float2,
	nzComponentType_Float3,
	nzComponentType_Float4,
	nzComponentType_Int1,
	nzComponentType_Int2,
	nzComponentType_Int3,
	nzComponentType_Int4,

	nzComponentType_Max = nzComponentType_Int4
};

enum nzCubemapFace
{
	// Cette énumération est prévue pour remplacer l'argument "z" des méthodes de NzImage contenant un cubemap
	// L'ordre est X, -X, Y, -Y, Z, -Z
	nzCubemapFace_PositiveX = 0,
	nzCubemapFace_PositiveY = 2,
	nzCubemapFace_PositiveZ = 4,
	nzCubemapFace_NegativeX = 1,
	nzCubemapFace_NegativeY = 3,
	nzCubemapFace_NegativeZ = 5,

	nzCubemapFace_Max = nzCubemapFace_NegativeZ
};

enum nzEventType
{
	nzEventType_GainedFocus,
	nzEventType_LostFocus,
	nzEventType_KeyPressed,
	nzEventType_KeyReleased,
	nzEventType_MouseButtonDoubleClicked,
	nzEventType_MouseButtonPressed,
	nzEventType_MouseButtonReleased,
	nzEventType_MouseEntered,
	nzEventType_MouseLeft,
	nzEventType_MouseMoved,
	nzEventType_MouseWheelMoved,
	nzEventType_Moved,
	nzEventType_Quit,
	nzEventType_Resized,
	nzEventType_TextEntered,

	nzEventType_Max = nzEventType_TextEntered
};

enum nzImageType
{
	nzImageType_1D,
	nzImageType_1D_Array,
	nzImageType_2D,
	nzImageType_2D_Array,
	nzImageType_3D,
	nzImageType_Cubemap,

	nzImageType_Max = nzImageType_Cubemap
};

enum nzNodeType
{
	nzNodeType_Default,  // NzNode
	nzNodeType_Scene,    // NzSceneNode (Graphics)
	nzNodeType_Skeletal, ///TODO

	nzNodeType_Max = nzNodeType_Skeletal
};

enum nzPixelFormat
{
	nzPixelFormat_Undefined = -1,

	nzPixelFormat_BGR8,            // 3*uint8
	nzPixelFormat_BGRA8,           // 4*uint8
	nzPixelFormat_DXT1,
	nzPixelFormat_DXT3,
	nzPixelFormat_DXT5,
	nzPixelFormat_L8,              // 1*uint8
	nzPixelFormat_LA8,             // 2*uint8
	nzPixelFormat_R8,              // 1*uint8
	nzPixelFormat_R8I,             // 1*int8
	nzPixelFormat_R8UI,            // 1*uint8
	nzPixelFormat_R16,             // 1*uint16
	nzPixelFormat_R16F,            // 1*half
	nzPixelFormat_R16I,            // 1*int16
	nzPixelFormat_R16UI,           // 1*uint16
	nzPixelFormat_R32F,            // 1*float
	nzPixelFormat_R32I,            // 1*uint16
	nzPixelFormat_R32UI,           // 1*uint32
	nzPixelFormat_RG8,             // 2*int8
	nzPixelFormat_RG8I,            // 2*int8
	nzPixelFormat_RG8UI,           // 2*uint8
	nzPixelFormat_RG16,            // 2*uint16
	nzPixelFormat_RG16F,           // 2*half
	nzPixelFormat_RG16I,           // 2*int16
	nzPixelFormat_RG16UI,          // 2*uint16
	nzPixelFormat_RG32F,           // 2*float
	nzPixelFormat_RG32I,           // 2*uint16
	nzPixelFormat_RG32UI,          // 2*uint32
	nzPixelFormat_RGB5A1,          // 3*uint5 + alpha bit
	nzPixelFormat_RGB8,            // 3*uint8
	nzPixelFormat_RGB16F,          // 3*half
	nzPixelFormat_RGB16I,          // 4*int16
	nzPixelFormat_RGB16UI,         // 4*uint16
	nzPixelFormat_RGB32F,          // 3*float
	nzPixelFormat_RGB32I,          // 4*int32
	nzPixelFormat_RGB32UI,         // 4*uint32
	nzPixelFormat_RGBA4,           // 4*uint4
	nzPixelFormat_RGBA8,           // 4*uint8
	nzPixelFormat_RGBA16F,         // 4*half
	nzPixelFormat_RGBA16I,         // 4*int16
	nzPixelFormat_RGBA16UI,        // 4*uint16
	nzPixelFormat_RGBA32F,         // 4*float
	nzPixelFormat_RGBA32I,         // 4*int32
	nzPixelFormat_RGBA32UI,        // 4*uint32
	nzPixelFormat_Depth16,
	nzPixelFormat_Depth24,
	nzPixelFormat_Depth24Stencil8,
	nzPixelFormat_Depth32,
	nzPixelFormat_Stencil1,
	nzPixelFormat_Stencil4,
	nzPixelFormat_Stencil8,
	nzPixelFormat_Stencil16,

	nzPixelFormat_Max = nzPixelFormat_Stencil16
};

enum nzPixelFormatType
{
	nzPixelFormatType_Undefined = -1,

	nzPixelFormatType_Color,
	nzPixelFormatType_Depth,
	nzPixelFormatType_DepthStencil,
	nzPixelFormatType_Stencil,

	nzPixelFormatType_Max = nzPixelFormatType_Stencil
};

enum nzPixelFlipping
{
	nzPixelFlipping_Horizontally,
	nzPixelFlipping_Vertically,

	nzPixelFlipping_Max = nzPixelFlipping_Vertically
};

enum nzPrimitiveMode
{
	nzPrimitiveMode_LineList,
	nzPrimitiveMode_LineStrip,
	nzPrimitiveMode_PointList,
	nzPrimitiveMode_TriangleList,
	nzPrimitiveMode_TriangleStrip,
	nzPrimitiveMode_TriangleFan,

	nzPrimitiveMode_Max = nzPrimitiveMode_TriangleFan
};

enum nzVertexComponent
{
	nzVertexComponent_Unused = -1,

	nzVertexComponent_InstanceData0,
	nzVertexComponent_InstanceData1,
	nzVertexComponent_InstanceData2,
	nzVertexComponent_InstanceData3,
	nzVertexComponent_InstanceData4,
	nzVertexComponent_InstanceData5,
	nzVertexComponent_Normal,
	nzVertexComponent_Position,
	nzVertexComponent_Tangent,
	nzVertexComponent_TexCoord,
	nzVertexComponent_Userdata0,
	nzVertexComponent_Userdata1,
	nzVertexComponent_Userdata2,
	nzVertexComponent_Userdata3,
	nzVertexComponent_Userdata4,
	nzVertexComponent_Userdata5,

	nzVertexComponent_FirstInstanceData = nzVertexComponent_InstanceData0,
	nzVertexComponent_FirstVertexData = nzVertexComponent_Normal,
	nzVertexComponent_LastInstanceData = nzVertexComponent_InstanceData5,
	nzVertexComponent_LastVertexData = nzVertexComponent_Userdata5,

	nzVertexComponent_Max = nzVertexComponent_Userdata5
};

enum nzVertexLayout
{
	// Déclarations destinées au rendu
	nzVertexLayout_XY,
	nzVertexLayout_XY_UV,
	nzVertexLayout_XYZ,
	nzVertexLayout_XYZ_Normal,
	nzVertexLayout_XYZ_Normal_UV,
	nzVertexLayout_XYZ_Normal_UV_Tangent,
	nzVertexLayout_XYZ_Normal_UV_Tangent_Skinning,
	nzVertexLayout_XYZ_UV,

	// Déclarations destinées à l'instancing
	nzVertexLayout_Matrix4,

	nzVertexLayout_Max = nzVertexLayout_Matrix4
};

enum nzWindowCursor
{
	nzWindowCursor_None,
	nzWindowCursor_Default,

	nzWindowCursor_Crosshair,
	nzWindowCursor_Hand,
	nzWindowCursor_Help,
	nzWindowCursor_Move,
	nzWindowCursor_Pointer,
	nzWindowCursor_Progress,
	nzWindowCursor_ResizeE,
	nzWindowCursor_ResizeN,
	nzWindowCursor_ResizeNE,
	nzWindowCursor_ResizeNW,
	nzWindowCursor_ResizeS,
	nzWindowCursor_ResizeSE,
	nzWindowCursor_ResizeSW,
	nzWindowCursor_ResizeW,
	nzWindowCursor_Text,
	nzWindowCursor_Wait,

	nzWindowCursor_Max = nzWindowCursor_Wait
};

enum nzWindowStyleFlags
{
	nzWindowStyle_None       = 0x0,
	nzWindowStyle_Fullscreen = 0x1,

	nzWindowStyle_Closable   = 0x2,
	nzWindowStyle_Resizable  = 0x4,
	nzWindowStyle_Titlebar   = 0x8,

	nzWindowStyle_Default = nzWindowStyle_Closable | nzWindowStyle_Resizable | nzWindowStyle_Titlebar,
	nzWindowStyle_Max = nzWindowStyle_Titlebar*2-1
};

#endif // NAZARA_ENUMS_UTILITY_HPP
