// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ENUMS_HPP
#define NAZARA_CORE_ENUMS_HPP

#include <NazaraUtils/Flags.hpp>

namespace Nz
{
	enum class ApplicationDirectory
	{
		Cache,
		Config,
		Data,
		SavedGames
	};

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
		// TODO: Rework to handle normalized
		Byte1,
		Byte2,
		Byte3,
		Byte4,
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
		UInt1,
		UInt2,
		UInt3,
		UInt4,

		Max = UInt4
	};

	constexpr std::size_t ComponentTypeCount = static_cast<std::size_t>(ComponentType::Max) + 1;

	enum class CoordSys
	{
		Global,
		Local,

		Max = Local
	};

	enum class CursorPosition
	{
		AtBegin,   // beginning of the file
		AtCurrent, // Position of the cursor
		AtEnd,     // End of the file

		Max = AtEnd
	};

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

	enum class ErrorMode
	{
		Silent,
		ThrowException,

		Max = ThrowException
	};

	template<>
	struct EnumAsFlags<ErrorMode>
	{
		static constexpr ErrorMode max = ErrorMode::Max;
	};

	using ErrorModeFlags = Flags<ErrorMode>;

	constexpr ErrorModeFlags ErrorMode_Default = {};

	enum class ErrorType
	{
		CheckFailed,
		Internal,
		Normal,
		Warning,

		Max = Warning
	};

	constexpr std::size_t ErrorTypeCount = static_cast<std::size_t>(ErrorType::Max) + 1;

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

	enum class IndexType
	{
		U8,
		U16,
		U32,

		Max = U32
	};

	enum class HashType
	{
		CRC32,
		CRC64,
		Fletcher16,
		MD5,
		SHA1,
		SHA224,
		SHA256,
		SHA384,
		SHA512,
		Whirlpool,

		Max = Whirlpool
	};

	constexpr std::size_t HashTypeCount = static_cast<std::size_t>(HashType::Max) + 1;

	enum class OpenMode
	{
		NotOpen,    //< File is not open

		Append,     //< Disables writing to existing content, all write operations are performed at the end
		Defer,      //< Defers file opening until a read/write operation is performed on it
		Lock,       //< Prevents file modification by other handles while it's open
		MustExist,  //< Fails if the file doesn't exists, even if opened in write mode
		Read,       //< Allows read operations
		Text,       //< Opens in text mode (converts system line endings from/to \n)
		Truncate,   //< Creates the file if it doesn't exist and empties it otherwise
		Unbuffered, //< Each read/write operations are performed directly using system calls (very slow)
		Write,      //< Allows write operations, creates the file if it doesn't exist

		Max = Write
	};

	template<>
	struct EnumAsFlags<OpenMode>
	{
		static constexpr OpenMode max = OpenMode::Max;
	};

	using OpenModeFlags = Flags<OpenMode>;

	constexpr OpenModeFlags OpenMode_ReadWrite = OpenMode::Read | OpenMode::Write;

	enum class ParameterType
	{
		Boolean,
		Color,
		Double,
		Integer,
		None,
		Pointer,
		String,
		Userdata,

		Max = Userdata
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

	enum class PrimitiveType
	{
		Box,
		Cone,
		Plane,
		Sphere,

		Max = Sphere
	};

	constexpr std::size_t PrimitiveTypeCount = static_cast<std::size_t>(PrimitiveType::Max) + 1;

	enum class ProcessorCap
	{
		x64,
		AES,
		AVX,
		FMA3,
		FMA4,
		MMX,
		Popcnt,
		RDRAND,
		XOP,
		SSE,
		SSE2,
		SSE3,
		SSSE3,
		SSE41,
		SSE42,
		SSE4a,

		Max = SSE4a
	};

	constexpr std::size_t ProcessorCapCount = static_cast<std::size_t>(ProcessorCap::Max) + 1;

	enum class ProcessorVendor
	{
		Unknown = -1,

		ACRN,
		AMD,
		Ao486,
		AppleRosetta2,
		Bhyve,
		Centaur,
		Cyrix,
		Elbrus,
		Hygon,
		HyperV,
		Intel,
		KVM,
		MicrosoftXTA,
		NSC,
		NexGen,
		Parallels,
		QEMU,
		QNX,
		Rise,
		SIS,
		Transmeta,
		UMC,
		VIA,
		VMware,
		Vortex,
		XenHVM,
		Zhaoxin,

		Max = Zhaoxin
	};

	constexpr std::size_t ProcessorVendorCount = static_cast<std::size_t>(ProcessorVendor::Max) + 1;

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

	enum class ResourceLoadingError
	{
		DecodingError,
		FailedToOpenFile,
		Internal,
		Unsupported,
		Unrecognized
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

	enum class SphereType
	{
		Cubic,
		Ico,
		UV,

		Max = UV
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

	enum class StreamOption
	{
		None,

		MemoryMapped,
		Sequential,
		Text,
		Unbuffered,

		Max = Unbuffered
	};

	template<>
	struct EnumAsFlags<StreamOption>
	{
		static constexpr StreamOption max = StreamOption::Max;
	};

	using StreamOptionFlags = Flags<StreamOption>;

	enum class SystemDirectory
	{
		// System-specific
		Cache,
		Fonts,
		Temporary,

		// User-specific
		UserConfig,
		UserDesktop,
		UserDocuments,
		UserDownloads,
		UserHome,
		UserMusic,
		UserPictures,
		UserVideos,
	};

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
		XY_Color_UV,
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

#endif // NAZARA_CORE_ENUMS_HPP
