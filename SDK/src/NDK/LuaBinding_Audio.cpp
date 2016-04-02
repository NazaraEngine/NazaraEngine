// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	void LuaBinding::BindAudio()
	{
		/*********************************** Nz::Music **********************************/
		musicClass.Inherit(soundEmitter);

		musicClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::Music*
		{
			return new Nz::Music;
		});

		//musicClass.SetMethod("Create", &Nz::Music::Create);
		//musicClass.SetMethod("Destroy", &Nz::Music::Destroy);

		musicClass.SetMethod("EnableLooping", &Nz::Music::EnableLooping);

		musicClass.SetMethod("GetDuration", &Nz::Music::GetDuration);
		musicClass.SetMethod("GetFormat", &Nz::Music::GetFormat);
		musicClass.SetMethod("GetPlayingOffset", &Nz::Music::GetPlayingOffset);
		musicClass.SetMethod("GetSampleCount", &Nz::Music::GetSampleCount);
		musicClass.SetMethod("GetSampleRate", &Nz::Music::GetSampleRate);
		musicClass.SetMethod("GetStatus", &Nz::Music::GetStatus);

		musicClass.SetMethod("IsLooping", &Nz::Music::IsLooping);

		musicClass.SetMethod("OpenFromFile", &Nz::Music::OpenFromFile, Nz::MusicParams());

		musicClass.SetMethod("Pause", &Nz::Music::Pause);
		musicClass.SetMethod("Play", &Nz::Music::Play);

		musicClass.SetMethod("SetPlayingOffset", &Nz::Music::SetPlayingOffset);

		musicClass.SetMethod("Stop", &Nz::Music::Stop);

		// Manual
		musicClass.SetMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Music& music) -> int
		{
			Nz::StringStream stream("Music(");
			stream << music.GetFilePath() << ')';

			lua.PushString(stream);
			return 1;
		});

		/*********************************** Nz::Sound **********************************/
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

		/*********************************** Nz::SoundBuffer **********************************/
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

		/*********************************** Nz::SoundEmitter **********************************/
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
	}

	void LuaBinding::RegisterAudio(Nz::LuaInstance& instance)
	{
		musicClass.Register(instance);
		soundClass.Register(instance);
		soundBuffer.Register(instance);
		soundEmitter.Register(instance);
	}
}
