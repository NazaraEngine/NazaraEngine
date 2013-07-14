// Copyright (C) 2013 Jérôme Leclercq
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

enum nzAttributeType
{
	nzAttributeType_Color,
	nzAttributeType_Double1,
	nzAttributeType_Double2,
	nzAttributeType_Double3,
	nzAttributeType_Double4,
	nzAttributeType_Float1,
	nzAttributeType_Float2,
	nzAttributeType_Float3,
	nzAttributeType_Float4,

	nzAttributeType_Max = nzAttributeType_Float4
};

enum nzAttributeUsage
{
	nzAttributeUsage_InstanceData0,
	nzAttributeUsage_InstanceData1,
	nzAttributeUsage_InstanceData2,
	nzAttributeUsage_InstanceData3,
	nzAttributeUsage_InstanceData4,
	nzAttributeUsage_InstanceData5,
	nzAttributeUsage_Normal,
	nzAttributeUsage_Position,
	nzAttributeUsage_Tangent,
	nzAttributeUsage_TexCoord,
	nzAttributeUsage_Userdata0,
	nzAttributeUsage_Userdata1,
	nzAttributeUsage_Userdata2,
	nzAttributeUsage_Userdata3,
	nzAttributeUsage_Userdata4,
	nzAttributeUsage_Userdata5,

	nzAttributeUsage_FirstInstanceData = nzAttributeUsage_InstanceData0,
	nzAttributeUsage_FirstVertexData = nzAttributeUsage_Normal,
	nzAttributeUsage_LastInstanceData = nzAttributeUsage_InstanceData5,
	nzAttributeUsage_LastVertexData = nzAttributeUsage_Userdata5,

	nzAttributeUsage_Max = nzAttributeUsage_Userdata5
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

	nzPixelFormat_BGR8,            // 3*nzUInt8
	nzPixelFormat_BGRA8,           // 4*nzUInt8
	nzPixelFormat_DXT1,
	nzPixelFormat_DXT3,
	nzPixelFormat_DXT5,
	nzPixelFormat_L8,              // 1*nzUInt8
	nzPixelFormat_LA8,             // 2*nzUInt8
	/*
	nzPixelFormat_RGB16F,
	nzPixelFormat_RGB16I,          // 4*nzUInt16
	nzPixelFormat_RGB32F,
	nzPixelFormat_RGB32I,          // 4*nzUInt32
	nzPixelFormat_RGBA16F,
	nzPixelFormat_RGBA16I,         // 4*nzUInt16
	nzPixelFormat_RGBA32F,
	nzPixelFormat_RGBA32I,         // 4*nzUInt32
	*/
	nzPixelFormat_RGB5A1,          // 1*nzUInt16
	nzPixelFormat_RGB8,            // 3*nzUInt8
	nzPixelFormat_RGBA4,           // 1*nzUInt16
	nzPixelFormat_RGBA8,           // 4*nzUInt8
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

enum nzVertexLayout
{
	// Déclarations destinées au rendu
	nzVertexLayout_XY,
	nzVertexLayout_XY_UV,
	nzVertexLayout_XYZ,
	nzVertexLayout_XYZ_Normal,
	nzVertexLayout_XYZ_Normal_UV,
	nzVertexLayout_XYZ_Normal_UV_Tangent,
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
