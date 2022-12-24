// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline std::optional<GLTextureFormat> DescribeTextureFormat(PixelFormat pixelFormat)
	{
		// TODO: Fill this switch
		switch (pixelFormat)
		{
			case PixelFormat::A8:               return GLTextureFormat{ GL_R8,                 GL_RED,             GL_UNSIGNED_BYTE,                  GL_ONE,   GL_ONE,   GL_ONE,  GL_RED };
			case PixelFormat::BGR8:             return GLTextureFormat{ GL_RGB8,               GL_RGB,             GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ONE };
			case PixelFormat::BGR8_SRGB:        return GLTextureFormat{ GL_SRGB8,              GL_RGB,             GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ONE };
			case PixelFormat::BGRA8:            return GLTextureFormat{ GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat::BGRA8_SRGB:       return GLTextureFormat{ GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat::Depth16:          return GLTextureFormat{ GL_DEPTH_COMPONENT16,  GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,                 GL_RED,   GL_ZERO,  GL_ZERO, GL_ZERO };
			case PixelFormat::Depth24:          return GLTextureFormat{ GL_DEPTH_COMPONENT24,  GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,                   GL_RED,   GL_ZERO,  GL_ZERO, GL_ZERO };
			case PixelFormat::Depth24Stencil8:  return GLTextureFormat{ GL_DEPTH24_STENCIL8,   GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8,              GL_RED,   GL_GREEN, GL_ZERO, GL_ZERO };
			case PixelFormat::Depth32F:         return GLTextureFormat{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT,                          GL_RED,   GL_ZERO,  GL_ZERO, GL_ZERO };
			case PixelFormat::Depth32FStencil8: return GLTextureFormat{ GL_DEPTH32F_STENCIL8,  GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_RED,   GL_GREEN, GL_ZERO, GL_ZERO };
			case PixelFormat::L8:               return GLTextureFormat{ GL_R8,                 GL_RED,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_RED,   GL_RED,  GL_ONE };
			case PixelFormat::LA8:              return GLTextureFormat{ GL_RG8,                GL_RG,              GL_UNSIGNED_BYTE,                  GL_RED,   GL_RED,   GL_RED,  GL_GREEN };
			case PixelFormat::RGB8:             return GLTextureFormat{ GL_RGB8,               GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ONE };
			case PixelFormat::RGB8_SRGB:        return GLTextureFormat{ GL_SRGB8,              GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ONE };
			case PixelFormat::RGBA8:            return GLTextureFormat{ GL_RGBA8,              GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA8_SRGB:       return GLTextureFormat{ GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA16F:          return GLTextureFormat{ GL_RGBA16F,            GL_RGBA,            GL_FLOAT,                          GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA32F:          return GLTextureFormat{ GL_RGBA32F,            GL_RGBA,            GL_FLOAT,                          GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			default: break;
		}

		NazaraError("Unhandled PixelFormat 0x" + NumberToString(UnderlyingCast(pixelFormat), 16));
		return {};
	}

	inline GLenum ToOpenGL(BlendEquation blendEquation)
	{
		switch (blendEquation)
		{
			case BlendEquation::Add:             return GL_FUNC_ADD;
			case BlendEquation::Max:             return GL_MAX;
			case BlendEquation::Min:             return GL_MIN;
			case BlendEquation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
			case BlendEquation::Subtract:        return GL_FUNC_SUBTRACT;
		}

		NazaraError("Unhandled BlendEquation 0x" + NumberToString(UnderlyingCast(blendEquation), 16));
		return {};
	}

	inline GLenum ToOpenGL(BlendFunc blendFunc)
	{
		switch (blendFunc)
		{
			case BlendFunc::ConstantAlpha:    return GL_CONSTANT_ALPHA;
			case BlendFunc::ConstantColor:    return GL_CONSTANT_COLOR;
			case BlendFunc::DstAlpha:         return GL_DST_ALPHA;
			case BlendFunc::DstColor:         return GL_DST_COLOR;
			case BlendFunc::InvConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
			case BlendFunc::InvConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendFunc::InvDstAlpha:      return GL_ONE_MINUS_DST_ALPHA;
			case BlendFunc::InvDstColor:      return GL_ONE_MINUS_DST_COLOR;
			case BlendFunc::InvSrcAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFunc::InvSrcColor:      return GL_ONE_MINUS_SRC_COLOR;
			case BlendFunc::SrcAlpha:         return GL_SRC_ALPHA;
			case BlendFunc::SrcColor:         return GL_SRC_COLOR;
			case BlendFunc::One:              return GL_ONE;
			case BlendFunc::Zero:             return GL_ZERO;
		}

		NazaraError("Unhandled BlendFunc 0x" + NumberToString(UnderlyingCast(blendFunc), 16));
		return {};
	}
	
	inline GLenum ToOpenGL(FaceFilling side)
	{
		switch (side)
		{
			case FaceFilling::Fill:  return GL_FILL;
			case FaceFilling::Line:  return GL_LINE;
			case FaceFilling::Point: return GL_POINT;
		}

		NazaraError("Unhandled FaceFilling 0x" + NumberToString(UnderlyingCast(side), 16));
		return {};
	}

	inline GLenum ToOpenGL(FaceCulling side)
	{
		switch (side)
		{
			case FaceCulling::None:
				break;

			case FaceCulling::Back:         return GL_BACK;
			case FaceCulling::Front:        return GL_FRONT;
			case FaceCulling::FrontAndBack: return GL_FRONT_AND_BACK;
		}

		NazaraError("Unhandled FaceSide 0x" + NumberToString(UnderlyingCast(side), 16));
		return {};
	}

	inline GLenum ToOpenGL(FrontFace face)
	{
		switch (face)
		{
			case FrontFace::Clockwise:        return GL_CW;
			case FrontFace::CounterClockwise: return GL_CCW;
		}

		NazaraError("Unhandled FrontFace 0x" + NumberToString(UnderlyingCast(face), 16));
		return {};
	}

	inline GLenum ToOpenGL(IndexType indexType)
	{
		switch (indexType)
		{
			case IndexType::U8:  return GL_UNSIGNED_BYTE;
			case IndexType::U16: return GL_UNSIGNED_SHORT;
			case IndexType::U32: return GL_UNSIGNED_INT;
		}

		NazaraError("Unhandled IndexType 0x" + NumberToString(UnderlyingCast(indexType), 16));
		return {};
	}

	inline GLenum ToOpenGL(PrimitiveMode primitiveMode)
	{
		switch (primitiveMode)
		{
			case PrimitiveMode::LineList:      return GL_LINES;
			case PrimitiveMode::LineStrip:     return GL_LINE_STRIP;
			case PrimitiveMode::PointList:     return GL_POINTS;
			case PrimitiveMode::TriangleList:  return GL_TRIANGLES;
			case PrimitiveMode::TriangleStrip: return GL_TRIANGLE_STRIP;
			case PrimitiveMode::TriangleFan:   return GL_TRIANGLE_FAN;
		}

		NazaraError("Unhandled PrimitiveMode 0x" + NumberToString(UnderlyingCast(primitiveMode), 16));
		return {};
	}

	inline GLenum ToOpenGL(RendererComparison comparison)
	{
		switch (comparison)
		{
			case RendererComparison::Always:         return GL_ALWAYS;
			case RendererComparison::Equal:          return GL_EQUAL;
			case RendererComparison::Greater:        return GL_GREATER;
			case RendererComparison::GreaterOrEqual: return GL_GEQUAL;
			case RendererComparison::Less:           return GL_LESS;
			case RendererComparison::LessOrEqual:    return GL_LEQUAL;
			case RendererComparison::Never:          return GL_NEVER;
			case RendererComparison::NotEqual:       return GL_NOTEQUAL;
		}

		NazaraError("Unhandled RendererComparison 0x" + NumberToString(UnderlyingCast(comparison), 16));
		return {};
	}

	inline GLenum ToOpenGL(SamplerFilter filter)
	{
		switch (filter)
		{
			case SamplerFilter::Linear:  return GL_LINEAR;
			case SamplerFilter::Nearest: return GL_NEAREST;
		}

		NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(filter), 16));
		return {};
	}

	inline GLenum ToOpenGL(SamplerFilter minFilter, SamplerMipmapMode mipmapFilter)
	{
		switch (minFilter)
		{
			case SamplerFilter::Linear:
			{
				switch (mipmapFilter)
				{
					case SamplerMipmapMode::Linear: return GL_LINEAR_MIPMAP_LINEAR;
					case SamplerMipmapMode::Nearest: return GL_LINEAR_MIPMAP_NEAREST;
				}

				NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(mipmapFilter), 16));
				return {};
			}

			case SamplerFilter::Nearest:
			{
				switch (mipmapFilter)
				{
					case SamplerMipmapMode::Linear: return GL_NEAREST_MIPMAP_LINEAR;
					case SamplerMipmapMode::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
				}

				NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(mipmapFilter), 16));
				return {};
			}
		}

		NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(minFilter), 16));
		return {};
	}

	inline GLenum ToOpenGL(SamplerWrap wrapMode)
	{
		switch (wrapMode)
		{
			case SamplerWrap::Clamp:          return GL_CLAMP_TO_EDGE;
			case SamplerWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
			case SamplerWrap::Repeat:         return GL_REPEAT;
		}

		NazaraError("Unhandled SamplerWrap 0x" + NumberToString(UnderlyingCast(wrapMode), 16));
		return {};
	}

	inline GLenum ToOpenGL(nzsl::ShaderStageType stageType)
	{
		switch (stageType)
		{
			case nzsl::ShaderStageType::Compute:  return GL_COMPUTE_SHADER;
			case nzsl::ShaderStageType::Fragment: return GL_FRAGMENT_SHADER;
			case nzsl::ShaderStageType::Vertex:   return GL_VERTEX_SHADER;
		}

		NazaraError("Unhandled nzsl::ShaderStageType 0x" + NumberToString(UnderlyingCast(stageType), 16));
		return {};
	}

	inline GLenum ToOpenGL(StencilOperation stencilOp)
	{
		switch (stencilOp)
		{
			case StencilOperation::Decrement:        return GL_DECR;
			case StencilOperation::DecrementNoClamp: return GL_DECR_WRAP;
			case StencilOperation::Increment:        return GL_INCR;
			case StencilOperation::IncrementNoClamp: return GL_INCR_WRAP;
			case StencilOperation::Invert:           return GL_INVERT;
			case StencilOperation::Keep:             return GL_KEEP;
			case StencilOperation::Replace:          return GL_REPLACE;
			case StencilOperation::Zero:             return GL_ZERO;
		}

		NazaraError("Unhandled StencilOperation 0x" + NumberToString(UnderlyingCast(stencilOp), 16));
		return {};
	}

	inline GLenum ToOpenGL(TextureAccess textureAccess)
	{
		switch (textureAccess)
		{
			case TextureAccess::ReadOnly:  return GL_READ_ONLY;
			case TextureAccess::ReadWrite: return GL_READ_WRITE;
			case TextureAccess::WriteOnly: return GL_WRITE_ONLY;
		}

		NazaraError("Unhandled TextureAccess 0x" + NumberToString(UnderlyingCast(textureAccess), 16));
		return {};
	}

	inline GLenum ToOpenGL(GL::BufferTarget bufferTarget)
	{
		switch (bufferTarget)
		{
			case GL::BufferTarget::Array:             return GL_ARRAY_BUFFER;
			case GL::BufferTarget::CopyRead:          return GL_COPY_READ_BUFFER;
			case GL::BufferTarget::CopyWrite:         return GL_COPY_WRITE_BUFFER;
			case GL::BufferTarget::ElementArray:      return GL_ELEMENT_ARRAY_BUFFER;
			case GL::BufferTarget::PixelPack:         return GL_PIXEL_PACK_BUFFER;
			case GL::BufferTarget::PixelUnpack:       return GL_PIXEL_UNPACK_BUFFER;
			case GL::BufferTarget::Storage:           return GL_SHADER_STORAGE_BUFFER;
			case GL::BufferTarget::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
			case GL::BufferTarget::Uniform:           return GL_UNIFORM_BUFFER;
		}

		NazaraError("Unhandled GL::BufferTarget 0x" + NumberToString(UnderlyingCast(bufferTarget), 16));
		return {};
	}

	inline GLenum ToOpenGL(GL::TextureTarget textureTarget)
	{
		switch (textureTarget)
		{
			case GL::TextureTarget::Cubemap:          return GL_TEXTURE_CUBE_MAP;
			case GL::TextureTarget::CubemapNegativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			case GL::TextureTarget::CubemapNegativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			case GL::TextureTarget::CubemapNegativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			case GL::TextureTarget::CubemapPositiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			case GL::TextureTarget::CubemapPositiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			case GL::TextureTarget::CubemapPositiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			case GL::TextureTarget::Target2D:         return GL_TEXTURE_2D;
			case GL::TextureTarget::Target2D_Array:   return GL_TEXTURE_2D_ARRAY;
			case GL::TextureTarget::Target3D:         return GL_TEXTURE_3D;
		}

		NazaraError("Unhandled GL::TextureTarget 0x" + NumberToString(UnderlyingCast(textureTarget), 16));
		return {};
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
