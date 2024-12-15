// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_ENUMS_HPP
#define NAZARA_RENDERER_ENUMS_HPP

#include <NazaraUtils/Flags.hpp>

namespace Nz
{
	enum class AttachmentLoadOp
	{
		Clear,
		Discard,
		Load
	};

	enum class AttachmentStoreOp
	{
		Discard,
		Store
	};

	enum class ColorComponent
	{
		Red,
		Green,
		Blue,
		Alpha
	};

	template<>
	struct EnumAsFlags<ColorComponent>
	{
		static constexpr ColorComponent max = ColorComponent::Alpha;
	};

	using ColorComponentMask = Flags<ColorComponent>;

	constexpr ColorComponentMask ColorComponentAll = ColorComponent::Red | ColorComponent::Green | ColorComponent::Blue | ColorComponent::Alpha;

	enum class FramebufferType
	{
		Texture,
		Window
	};

	enum class MemoryAccess
	{
		ColorRead,
		ColorWrite,
		DepthStencilRead,
		DepthStencilWrite,
		IndexBufferRead,
		IndirectCommandRead,
		HostRead,
		HostWrite,
		MemoryRead,
		MemoryWrite,
		ShaderRead,
		ShaderWrite,
		TransferRead,
		TransferWrite,
		UniformBufferRead,
		VertexBufferRead,

		Max = VertexBufferRead
	};

	template<>
	struct EnumAsFlags<MemoryAccess>
	{
		static constexpr MemoryAccess max = MemoryAccess::Max;
	};

	using MemoryAccessFlags = Flags<MemoryAccess>;

	enum class PipelineStage
	{
		TopOfPipe,

		ColorOutput,
		ComputeShader,
		DrawIndirect,
		FragmentShader,
		FragmentTestsEarly,
		FragmentTestsLate,
		GeometryShader,
		TessellationControlShader,
		TessellationEvaluationShader,
		Transfer,
		TransformFeedback,
		VertexInput,
		VertexShader,

		BottomOfPipe,

		Max = BottomOfPipe
	};

	template<>
	struct EnumAsFlags<PipelineStage>
	{
		static constexpr PipelineStage max = PipelineStage::Max;
	};

	using PipelineStageFlags = Flags<PipelineStage>;

	enum class PresentMode
	{
		Immediate,
		Mailbox,
		RelaxedVerticalSync,
		VerticalSync,

		Max = VerticalSync
	};

	template<>
	struct EnumAsFlags<PresentMode>
	{
		static constexpr PresentMode max = PresentMode::Max;
	};

	using PresentModeFlags = Flags<PresentMode>;

	enum class QueueType
	{
		Compute,
		Graphics,
		Transfer,

		Max = Transfer
	};

	template<>
	struct EnumAsFlags<QueueType>
	{
		static constexpr QueueType max = QueueType::Max;
	};

	using QueueTypeFlags = Flags<QueueType>;

	enum class RenderAPI
	{
		Direct3D,  //< Microsoft Render API, only works on MS platforms
		Mantle,    //< AMD Render API, Vulkan predecessor, only works on AMD GPUs
		Metal,     //< Apple Render API, only works on OS X platforms
		OpenGL,    //< Khronos Render API, works on Desktop and some consoles
		OpenGL_ES, //< Khronos Render API, works on Web, Mobile and some consoles
		Vulkan,    //< New Khronos Render API, made to replace OpenGL, works on desktop (Windows/Linux) and mobile (Android), and Apple platform using MoltenVK

		Unknown,   //< RenderAPI not corresponding to an entry of the enum, or result of a failed query

		Max = Unknown
	};

	constexpr std::size_t RenderAPICount = static_cast<std::size_t>(RenderAPI::Max) + 1;

	enum class RenderAPIValidationLevel
	{
		None = 0,

		Errors   = 1,
		Warnings = 2,
		Verbose  = 3,
		Debug    = 4
	};

	enum class RenderDeviceType
	{
		Integrated, ///< Hardware-accelerated chipset integrated to a CPU (ex: Intel Graphics HD 4000)
		Dedicated,  ///< Hardware-accelerated GPU (ex: AMD R9 390)
		Software,   ///< Software-renderer
		Virtual,    ///< Proxy renderer relaying instructions to another unknown device

		Unknown,    ///< Device type not corresponding to an entry of the enum, or result of a failed query

		Max = Unknown
	};

	enum class ShaderBindingType
	{
		Sampler,
		StorageBuffer,
		Texture,
		UniformBuffer,

		Max = UniformBuffer
	};

	enum class ShaderLanguage
	{
		GLSL,
		HLSL,
		MSL,
		NazaraBinary,
		NazaraShader,
		SpirV
	};

	enum class TextureAccess
	{
		ReadOnly,
		ReadWrite,
		WriteOnly
	};

	enum class TextureLayout
	{
		ColorInput,
		ColorOutput,
		DepthStencilReadOnly,
		DepthStencilReadWrite,
		General,
		Present,
		TransferSource,
		TransferDestination,
		Undefined
	};

	enum class TexturePlane
	{
		Color,
		Depth,
		Stencil,
		Plane0,
		Plane1,
		Plane2,

		Max = Plane2
	};

	template<>
	struct EnumAsFlags<TexturePlane>
	{
		static constexpr TexturePlane max = TexturePlane::Max;
	};

	using TexturePlaneFlags = Flags<TexturePlane>;

	enum class TextureUsage
	{
		ColorAttachment,
		DepthStencilAttachment,
		InputAttachment,
		ShaderReadWrite,
		ShaderSampling,
		TransferSource,
		TransferDestination,

		Max = TransferDestination
	};

	template<>
	struct EnumAsFlags<TextureUsage>
	{
		static constexpr TextureUsage max = TextureUsage::Max;
	};

	using TextureUsageFlags = Flags<TextureUsage>;
}

#endif // NAZARA_RENDERER_ENUMS_HPP
