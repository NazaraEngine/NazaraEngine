/*
	Copyright(c) 2002 - 2016 Lee Salzman

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENETCOMPRESSOR_HPP
#define NAZARA_ENETCOMPRESSOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/NetBuffer.hpp>

namespace Nz
{
	class ENetPeer;

	class NAZARA_NETWORK_API ENetCompressor
	{
		public:
			ENetCompressor() = default;
			virtual ~ENetCompressor();

			virtual std::size_t Compress(const ENetPeer* peer, const NetBuffer* buffers, std::size_t bufferCount, std::size_t totalInputSize, UInt8* output, std::size_t maxOutputSize) = 0;
			virtual std::size_t Decompress(const ENetPeer* peer, const UInt8* input, std::size_t inputSize, UInt8* output, std::size_t maxOutputSize) = 0;
	};
}

#endif // NAZARA_ENETCOMPRESSOR_HPP
