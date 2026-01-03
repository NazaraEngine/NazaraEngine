// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TRANSFERINTERFACE_HPP
#define NAZARA_GRAPHICS_TRANSFERINTERFACE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderResources;

	class NAZARA_GRAPHICS_API TransferInterface
	{
		public:
			TransferInterface() = default;
			TransferInterface(const TransferInterface&) = default;
			TransferInterface(TransferInterface&&) = default;
			virtual ~TransferInterface();

			virtual void OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder) = 0;

			TransferInterface& operator=(const TransferInterface&) = default;
			TransferInterface& operator=(TransferInterface&&) = default;

			NazaraSignal(OnTransferRequired, TransferInterface* /*transfer*/);
	};
}

#include <Nazara/Graphics/TransferInterface.inl>

#endif // NAZARA_GRAPHICS_TRANSFERINTERFACE_HPP
