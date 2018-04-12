// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RUDMESSAGE_HPP
#define NAZARA_RUDMESSAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/NetPacket.hpp>

namespace Nz
{
	struct RUdpMessage
	{
		IpAddress from;
		NetPacket data;
	};
}

#endif // NAZARA_RUDMESSAGE_HPP