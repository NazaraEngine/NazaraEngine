// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TRANSFERINTERFACE_HPP
#define NAZARA_GRAPHICS_TRANSFERINTERFACE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderFrame;

	class NAZARA_GRAPHICS_API TransferInterface
	{
		public:
			TransferInterface() = default;
			TransferInterface(const TransferInterface&) = delete;
			TransferInterface(TransferInterface&&) = delete;
			virtual ~TransferInterface();

			virtual void OnTransfer(RenderFrame& renderFrame, CommandBufferBuilder& builder) = 0;

			TransferInterface& operator=(const TransferInterface&) = delete;
			TransferInterface& operator=(TransferInterface&&) = delete;
	};
}

#include <Nazara/Graphics/TransferInterface.inl>

#endif // NAZARA_GRAPHICS_TRANSFERINTERFACE_HPP
