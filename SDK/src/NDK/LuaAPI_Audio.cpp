// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <Nazara/Audio.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Audio(Nz::LuaInstance& instance)
	{
		/*********************************** Nz::SoundBuffer **********************************/
		Nz::LuaClass<Nz::SoundBufferRef> soundBuffer("SoundBuffer");

		soundBuffer.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::SoundBufferRef*
		{
			return new Nz::SoundBufferRef(new Nz::SoundBuffer);
		});

		soundBuffer.SetMethod("Destroy", &Nz::SoundBuffer::Destroy);
		soundBuffer.SetMethod("GetDuration", &Nz::SoundBuffer::GetDuration);
		soundBuffer.SetMethod("GetFormat", &Nz::SoundBuffer::GetFormat);
		soundBuffer.SetMethod("GetSampleCount", &Nz::SoundBuffer::GetSampleCount);
		soundBuffer.SetMethod("GetSampleRate", &Nz::SoundBuffer::GetSampleRate);
		soundBuffer.SetMethod("IsValid", &Nz::SoundBuffer::IsValid);
		soundBuffer.SetMethod("LoadFromFile", &Nz::SoundBuffer::LoadFromFile, Nz::SoundBufferParams());

		soundBuffer.SetStaticMethod("IsFormatSupported", &Nz::SoundBuffer::IsFormatSupported);

		// Manual
		soundBuffer.SetMethod("Create", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance) -> int
		{
			int index = 1;
			Nz::AudioFormat format = lua.Check<Nz::AudioFormat>(&index);
			unsigned int sampleCount = lua.Check<unsigned int>(&index);
			unsigned int sampleRate = lua.Check<unsigned int>(&index);

			std::size_t bufferSize = 0;
			const char* buffer = lua.CheckString(index, &bufferSize);
			lua.ArgCheck(buffer && bufferSize >= sampleCount * sizeof(Nz::Int16), index, "Invalid buffer");

			lua.PushBoolean(instance->Create(format, sampleCount, sampleRate, reinterpret_cast<const Nz::Int16*>(buffer)));
			return 1;
		});

		soundBuffer.SetMethod("GetSamples", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance) -> int
		{
			lua.PushString(reinterpret_cast<const char*>(instance->GetSamples()), instance->GetSampleCount() * sizeof(Nz::Int16));
			return 1;
		});

		soundBuffer.SetMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& soundBuffer) -> int
		{
			Nz::StringStream stream("SoundBuffer(");
			if (soundBuffer->IsValid())
			{
				Nz::String filePath = soundBuffer->GetFilePath();
				if (!filePath.IsEmpty())
					stream << "File: " << filePath << ", ";
				
				stream << "Duration: " << soundBuffer->GetDuration() / 1000.f << "s";
			}
			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		soundBuffer.Register(instance);

		/*********************************** Nz::SoundEmitter **********************************/
		Nz::LuaClass<Nz::SoundEmitter> soundEmitter("SoundEmitter");

		soundEmitter.SetMethod("EnableLooping", &Nz::SoundEmitter::EnableLooping);
		soundEmitter.SetMethod("EnableSpatialization", &Nz::SoundEmitter::EnableSpatialization);
		soundEmitter.SetMethod("GetAttenuation", &Nz::SoundEmitter::GetAttenuation);
		soundEmitter.SetMethod("GetDuration", &Nz::SoundEmitter::GetDuration);
		soundEmitter.SetMethod("GetMinDistance", &Nz::SoundEmitter::GetMinDistance);
		soundEmitter.SetMethod("GetPitch", &Nz::SoundEmitter::GetPitch);
		soundEmitter.SetMethod("GetPlayingOffset", &Nz::SoundEmitter::GetPlayingOffset);
		soundEmitter.SetMethod("GetPosition", &Nz::Sound::GetPosition);
		soundEmitter.SetMethod("GetStatus", &Nz::SoundEmitter::GetStatus);
		soundEmitter.SetMethod("GetVelocity", &Nz::Sound::GetVelocity);
		soundEmitter.SetMethod("GetVolume", &Nz::SoundEmitter::GetVolume);
		soundEmitter.SetMethod("IsLooping", &Nz::SoundEmitter::IsLooping);
		soundEmitter.SetMethod("IsSpatialized", &Nz::SoundEmitter::IsSpatialized);
		soundEmitter.SetMethod("Pause", &Nz::SoundEmitter::Pause);
		soundEmitter.SetMethod("Play", &Nz::SoundEmitter::Play);
		soundEmitter.SetMethod("SetAttenuation", &Nz::SoundEmitter::SetAttenuation);
		soundEmitter.SetMethod("SetMinDistance", &Nz::SoundEmitter::SetMinDistance);
		soundEmitter.SetMethod("SetPitch", &Nz::SoundEmitter::SetPitch);
		soundEmitter.SetMethod("SetPosition", (void(Nz::SoundEmitter::*)(const Nz::Vector3f&)) &Nz::SoundEmitter::SetPosition);
		soundEmitter.SetMethod("SetVelocity", (void(Nz::SoundEmitter::*)(const Nz::Vector3f&)) &Nz::SoundEmitter::SetVelocity);
		soundEmitter.SetMethod("SetVolume", &Nz::SoundEmitter::SetVolume);
		soundEmitter.SetMethod("Stop", &Nz::SoundEmitter::Stop);

		soundEmitter.Register(instance);

		/*********************************** Nz::Sound **********************************/
		Nz::LuaClass<Nz::Sound> soundClass("Sound");
		soundClass.Inherit(soundEmitter);

		soundClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::Sound*
		{
			return new Nz::Sound;
		});

		soundClass.SetMethod("GetBuffer", &Nz::Sound::GetBuffer);
		soundClass.SetMethod("IsPlayable", &Nz::Sound::IsPlayable);
		soundClass.SetMethod("IsPlaying", &Nz::Sound::IsPlaying);
		soundClass.SetMethod("LoadFromFile", &Nz::Sound::LoadFromFile, Nz::SoundBufferParams());
		soundClass.SetMethod("SetPlayingOffset", &Nz::Sound::SetPlayingOffset);

		// Manual
		soundClass.SetMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Sound& sound) -> int
		{
			Nz::StringStream stream("Sound(");
			if (const Nz::SoundBuffer* buffer = sound.GetBuffer())
				stream << buffer;

			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		soundClass.Register(instance);
	}
}
