// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_UTILITY_HPP
#define NAZARA_ENUMS_UTILITY_HPP

enum nzAnimationType
{
	nzAnimationType_Keyframe,
	nzAnimationType_Skeletal,
	nzAnimationType_Static
};

enum nzBufferAccess
{
	nzBufferAccess_DiscardAndWrite,
	nzBufferAccess_ReadOnly,
	nzBufferAccess_ReadWrite,
	nzBufferAccess_WriteOnly
};

enum nzBufferStorage
{
	//nzBufferStorage_Both,
	nzBufferStorage_Hardware,
	nzBufferStorage_Software,

	nzBufferStorage_Max = nzBufferStorage_Software
};

enum nzBufferType
{
	nzBufferType_Index,
	nzBufferType_Vertex
};

enum nzBufferUsage
{
	nzBufferUsage_Dynamic,
	nzBufferUsage_Static
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
	nzCubemapFace_NegativeZ = 5
};

enum nzElementStream
{
	nzElementStream_VertexData,
	nzElementStream_InstancedData,

	nzElementStream_Max = nzElementStream_InstancedData
};

enum nzElementType
{
	nzElementType_Color,
	nzElementType_Double1,
	nzElementType_Double2,
	nzElementType_Double3,
	nzElementType_Double4,
	nzElementType_Float1,
	nzElementType_Float2,
	nzElementType_Float3,
	nzElementType_Float4
};

enum nzElementUsage
{
	nzElementUsage_Diffuse,
	nzElementUsage_Normal,
	nzElementUsage_Position,
	nzElementUsage_Tangent,
	nzElementUsage_TexCoord,

	nzElementUsage_Max = nzElementUsage_TexCoord
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
	nzEventType_TextEntered
};

enum nzExtend
{
	nzExtend_Finite,
	nzExtend_Infinite,
	nzExtend_Null
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

enum nzPixelFormat
{
	nzPixelFormat_Undefined,

	nzPixelFormat_BGR8,    // 3*nzUInt8
	nzPixelFormat_BGRA8,   // 4*nzUInt8
	nzPixelFormat_DXT1,
	nzPixelFormat_DXT3,
	nzPixelFormat_DXT5,
	nzPixelFormat_L8,      // 1*nzUInt8
	nzPixelFormat_LA8,     // 2*nzUInt8
	/*
	nzPixelFormat_RGB16F,
	nzPixelFormat_RGB16I,  // 4*nzUInt16
	nzPixelFormat_RGB32F,
	nzPixelFormat_RGB32I,  // 4*nzUInt32
	nzPixelFormat_RGBA16F,
	nzPixelFormat_RGBA16I, // 4*nzUInt16
	nzPixelFormat_RGBA32F,
	nzPixelFormat_RGBA32I, // 4*nzUInt32
	*/
	nzPixelFormat_RGBA4,   // 1*nzUInt16
	nzPixelFormat_RGB5A1,  // 1*nzUInt16
	nzPixelFormat_RGB8,    // 3*nzUInt8
	nzPixelFormat_RGBA8,   // 4*nzUInt8
	/*
	nzPixelFormat_Depth16,
	nzPixelFormat_Depth24,
	nzPixelFormat_Depth24Stencil8,
	nzPixelFormat_Depth32,
	nzPixelFormat_Stencil1,
	nzPixelFormat_Stencil4,
	nzPixelFormat_Stencil8,
	nzPixelFormat_Stencil16,
	*/

	nzPixelFormat_Max = nzPixelFormat_RGBA8
};

enum nzPixelFlipping
{
	nzPixelFlipping_Horizontally,
	nzPixelFlipping_Vertically,

	nzPixelFlipping_Max = nzPixelFlipping_Vertically
};

enum nzPrimitiveType
{
	nzPrimitiveType_LineList,
	nzPrimitiveType_LineStrip,
	nzPrimitiveType_PointList,
	nzPrimitiveType_TriangleList,
	nzPrimitiveType_TriangleStrip,
	nzPrimitiveType_TriangleFan
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
	nzWindowCursor_Wait
};

enum nzWindowStyle
{
	nzWindowStyle_None       = 0x0,
	nzWindowStyle_Fullscreen = 0x1,

	nzWindowStyle_Closable   = 0x2,
	nzWindowStyle_Resizable  = 0x4,
	nzWindowStyle_Titlebar   = 0x4,

	nzWindowStyle_Default = nzWindowStyle_Closable | nzWindowStyle_Resizable | nzWindowStyle_Titlebar
};

#endif // NAZARA_ENUMS_UTILITY_HPP
