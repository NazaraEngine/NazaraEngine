// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPASS_HPP
#define NAZARA_RENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API RenderPass
	{
		public:
			struct Attachment;

			RenderPass() = default;
			RenderPass(const RenderPass&) = delete;
			RenderPass(RenderPass&&) noexcept = default;
			virtual ~RenderPass();

			RenderPass& operator=(const RenderPass&) = delete;
			RenderPass& operator=(RenderPass&&) noexcept = default;

			struct Attachment
			{
				PixelFormat format;
				// TODO
			};
	};
}

#include <Nazara/Renderer/RenderPass.inl>

#endif // NAZARA_RENDERPASS_HPP
