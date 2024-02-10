// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline bool GLTextureFormat::HasSwizzle() const
	{
		return swizzleR != GL_RED || swizzleG != GL_GREEN || swizzleB != GL_BLUE || swizzleA != GL_ALPHA;
	}

	inline std::optional<GLTextureFormat> DescribeTextureFormat(PixelFormat pixelFormat)
	{
		// TODO: Fill this switch
		switch (pixelFormat)
		{
			case PixelFormat::A8:               return GLTextureFormat{ GL_R8,                 GL_RED,             GL_UNSIGNED_BYTE,                  GL_ONE,   GL_ONE,   GL_ONE,  GL_RED };
			case PixelFormat::BGR8:             return GLTextureFormat{ GL_RGB8,               GL_RGB,             GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat::BGR8_SRGB:        return GLTextureFormat{ GL_SRGB8,              GL_RGB,             GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat::BGRA8:            return GLTextureFormat{ GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat::BGRA8_SRGB:       return GLTextureFormat{ GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat::Depth16:          return GLTextureFormat{ GL_DEPTH_COMPONENT16,  GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,                 GL_RED,   GL_ZERO,  GL_ZERO, GL_ZERO };
			case PixelFormat::Depth24:          return GLTextureFormat{ GL_DEPTH_COMPONENT24,  GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,                   GL_RED,   GL_ZERO,  GL_ZERO, GL_ZERO };
			case PixelFormat::Depth24Stencil8:  return GLTextureFormat{ GL_DEPTH24_STENCIL8,   GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8,              GL_RED,   GL_GREEN, GL_ZERO, GL_ZERO };
			case PixelFormat::Depth32F:         return GLTextureFormat{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT,                          GL_RED,   GL_ZERO,  GL_ZERO, GL_ZERO };
			case PixelFormat::Depth32FStencil8: return GLTextureFormat{ GL_DEPTH32F_STENCIL8,  GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_RED,   GL_GREEN, GL_ZERO, GL_ZERO };
			case PixelFormat::L8:               return GLTextureFormat{ GL_R8,                 GL_RED,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_RED,   GL_RED,  GL_ONE };
			case PixelFormat::LA8:              return GLTextureFormat{ GL_RG8,                GL_RG,              GL_UNSIGNED_BYTE,                  GL_RED,   GL_RED,   GL_RED,  GL_GREEN };
			case PixelFormat::R8:               return GLTextureFormat{ GL_R8,                 GL_RED,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RG8:              return GLTextureFormat{ GL_RG8,                GL_RG,              GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGB8:             return GLTextureFormat{ GL_RGB8,               GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGB8_SRGB:        return GLTextureFormat{ GL_SRGB8,              GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA8:            return GLTextureFormat{ GL_RGBA8,              GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA8_SRGB:       return GLTextureFormat{ GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA16F:          return GLTextureFormat{ GL_RGBA16F,            GL_RGBA,            GL_FLOAT,                          GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat::RGBA32F:          return GLTextureFormat{ GL_RGBA32F,            GL_RGBA,            GL_FLOAT,                          GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			default: break;
		}

		NazaraErrorFmt("unhandled PixelFormat {0:#x})", UnderlyingCast(pixelFormat));
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

		NazaraErrorFmt("unhandled BlendEquation {0:#x})", UnderlyingCast(blendEquation));
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

		NazaraErrorFmt("unhandled BlendFunc {0:#x})", UnderlyingCast(blendFunc));
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

		NazaraErrorFmt("unhandled FaceFilling {0:#x})", UnderlyingCast(side));
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

		NazaraErrorFmt("unhandled FaceSide {0:#x})", UnderlyingCast(side));
		return {};
	}

	inline GLenum ToOpenGL(FrontFace face)
	{
		switch (face)
		{
			case FrontFace::Clockwise:        return GL_CW;
			case FrontFace::CounterClockwise: return GL_CCW;
		}

		NazaraErrorFmt("unhandled FrontFace {0:#x})", UnderlyingCast(face));
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

		NazaraErrorFmt("unhandled IndexType {0:#x})", UnderlyingCast(indexType));
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

		NazaraErrorFmt("unhandled PrimitiveMode {0:#x})", UnderlyingCast(primitiveMode));
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

		NazaraErrorFmt("unhandled RendererComparison {0:#x})", UnderlyingCast(comparison));
		return {};
	}

	inline GLenum ToOpenGL(SamplerFilter filter)
	{
		switch (filter)
		{
			case SamplerFilter::Linear:  return GL_LINEAR;
			case SamplerFilter::Nearest: return GL_NEAREST;
		}

		NazaraErrorFmt("unhandled SamplerFilter {0:#x})", UnderlyingCast(filter));
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

				NazaraErrorFmt("unhandled SamplerFilter {0:#x})", UnderlyingCast(mipmapFilter));
				return {};
			}

			case SamplerFilter::Nearest:
			{
				switch (mipmapFilter)
				{
					case SamplerMipmapMode::Linear: return GL_NEAREST_MIPMAP_LINEAR;
					case SamplerMipmapMode::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
				}

				NazaraErrorFmt("unhandled SamplerFilter {0:#x})", UnderlyingCast(mipmapFilter));
				return {};
			}
		}

		NazaraErrorFmt("unhandled SamplerFilter {0:#x})", UnderlyingCast(minFilter));
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

		NazaraErrorFmt("unhandled SamplerWrap {0:#x})", UnderlyingCast(wrapMode));
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

		NazaraErrorFmt("unhandled nzsl::ShaderStageType {0:#x})", UnderlyingCast(stageType));
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

		NazaraErrorFmt("unhandled StencilOperation {0:#x})", UnderlyingCast(stencilOp));
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

		NazaraErrorFmt("unhandled TextureAccess {0:#x})", UnderlyingCast(textureAccess));
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

		NazaraErrorFmt("unhandled GL::BufferTarget {0:#x})", UnderlyingCast(bufferTarget));
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

		NazaraErrorFmt("unhandled GL::TextureTarget {0:#x})", UnderlyingCast(textureTarget));
		return {};
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
