// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_AUDIO_HPP
#define NDK_LUABINDING_AUDIO_HPP

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Audio : public LuaBinding_Base
	{
		public:
			LuaBinding_Audio(LuaBinding& binding);
			~LuaBinding_Audio() = default;

			void Register(Nz::LuaInstance& instance) override;

			Nz::LuaClass<Nz::Music> music;
			Nz::LuaClass<Nz::Sound> sound;
			Nz::LuaClass<Nz::SoundBufferRef> soundBuffer;
			Nz::LuaClass<Nz::SoundEmitter> soundEmitter;
	};
}

#endif // NDK_LUABINDING_CORE_HPP
