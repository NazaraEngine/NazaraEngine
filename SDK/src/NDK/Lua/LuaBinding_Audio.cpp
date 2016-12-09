// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Lua/LuaBinding_Audio.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindAudio(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Audio>(binding);
	}

	LuaBinding_Audio::LuaBinding_Audio(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::SoundEmitter **********************************/
		soundEmitter.Reset("SoundEmitter");
		{
			soundEmitter.BindMethod("EnableLooping", &Nz::SoundEmitter::EnableLooping);
			soundEmitter.BindMethod("EnableSpatialization", &Nz::SoundEmitter::EnableSpatialization);

			soundEmitter.BindMethod("GetAttenuation", &Nz::SoundEmitter::GetAttenuation);
			soundEmitter.BindMethod("GetDuration", &Nz::SoundEmitter::GetDuration);
			soundEmitter.BindMethod("GetMinDistance", &Nz::SoundEmitter::GetMinDistance);
			soundEmitter.BindMethod("GetPitch", &Nz::SoundEmitter::GetPitch);
			soundEmitter.BindMethod("GetPlayingOffset", &Nz::SoundEmitter::GetPlayingOffset);
			soundEmitter.BindMethod("GetPosition", &Nz::Sound::GetPosition);
			soundEmitter.BindMethod("GetStatus", &Nz::SoundEmitter::GetStatus);
			soundEmitter.BindMethod("GetVelocity", &Nz::Sound::GetVelocity);
			soundEmitter.BindMethod("GetVolume", &Nz::SoundEmitter::GetVolume);

			soundEmitter.BindMethod("IsLooping", &Nz::SoundEmitter::IsLooping);
			soundEmitter.BindMethod("IsSpatialized", &Nz::SoundEmitter::IsSpatialized);

			soundEmitter.BindMethod("Pause", &Nz::SoundEmitter::Pause);
			soundEmitter.BindMethod("Play", &Nz::SoundEmitter::Play);

			soundEmitter.BindMethod("SetAttenuation", &Nz::SoundEmitter::SetAttenuation);
			soundEmitter.BindMethod("SetMinDistance", &Nz::SoundEmitter::SetMinDistance);
			soundEmitter.BindMethod("SetPitch", &Nz::SoundEmitter::SetPitch);
			soundEmitter.BindMethod("SetPosition", (void(Nz::SoundEmitter::*)(const Nz::Vector3f&)) &Nz::SoundEmitter::SetPosition);
			soundEmitter.BindMethod("SetVelocity", (void(Nz::SoundEmitter::*)(const Nz::Vector3f&)) &Nz::SoundEmitter::SetVelocity);
			soundEmitter.BindMethod("SetVolume", &Nz::SoundEmitter::SetVolume);

			soundEmitter.BindMethod("Stop", &Nz::SoundEmitter::Stop);
		}

		/*********************************** Nz::Music **********************************/
		music.Reset("Music");
		{
			music.Inherit(soundEmitter);

			music.BindDefaultConstructor();

			//musicClass.SetMethod("Create", &Nz::Music::Create);
			//musicClass.SetMethod("Destroy", &Nz::Music::Destroy);

			music.BindMethod("EnableLooping", &Nz::Music::EnableLooping);

			music.BindMethod("GetDuration", &Nz::Music::GetDuration);
			music.BindMethod("GetFormat", &Nz::Music::GetFormat);
			music.BindMethod("GetPlayingOffset", &Nz::Music::GetPlayingOffset);
			music.BindMethod("GetSampleCount", &Nz::Music::GetSampleCount);
			music.BindMethod("GetSampleRate", &Nz::Music::GetSampleRate);
			music.BindMethod("GetStatus", &Nz::Music::GetStatus);

			music.BindMethod("IsLooping", &Nz::Music::IsLooping);

			music.BindMethod("OpenFromFile", &Nz::Music::OpenFromFile, Nz::MusicParams());

			music.BindMethod("Pause", &Nz::Music::Pause);
			music.BindMethod("Play", &Nz::Music::Play);

			music.BindMethod("SetPlayingOffset", &Nz::Music::SetPlayingOffset);

			music.BindMethod("Stop", &Nz::Music::Stop);

			// Manual
			music.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Music& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::StringStream ss("Music(");
				ss << instance.GetFilePath() << ')';

				lua.PushString(ss);
				return 1;
			});
		}

		/*********************************** Nz::Sound **********************************/
		sound.Reset("Sound");
		{
			sound.Inherit(soundEmitter);

			sound.BindDefaultConstructor();

			sound.BindMethod("GetBuffer", &Nz::Sound::GetBuffer);

			sound.BindMethod("IsPlayable", &Nz::Sound::IsPlayable);
			sound.BindMethod("IsPlaying", &Nz::Sound::IsPlaying);

			sound.BindMethod("LoadFromFile", &Nz::Sound::LoadFromFile, Nz::SoundBufferParams());

			sound.BindMethod("SetPlayingOffset", &Nz::Sound::SetPlayingOffset);

			// Manual
			sound.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Sound& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::StringStream ss("Sound(");
				if (const Nz::SoundBuffer* buffer = instance.GetBuffer())
					ss << buffer;

				ss << ')';

				lua.PushString(ss);
				return 1;
			});
		}

		/*********************************** Nz::SoundBuffer **********************************/
		soundBuffer.Reset("SoundBuffer");
		{
			soundBuffer.SetConstructor([] (Nz::LuaInstance& lua, Nz::SoundBufferRef* instance, std::size_t argumentCount)
			{
				NazaraUnused(lua);
				NazaraUnused(argumentCount);

				Nz::PlacementNew(instance, Nz::SoundBuffer::New());
				return true;
			});

			soundBuffer.BindMethod("Destroy", &Nz::SoundBuffer::Destroy);

			soundBuffer.BindMethod("GetDuration", &Nz::SoundBuffer::GetDuration);
			soundBuffer.BindMethod("GetFormat", &Nz::SoundBuffer::GetFormat);
			soundBuffer.BindMethod("GetSampleCount", &Nz::SoundBuffer::GetSampleCount);
			soundBuffer.BindMethod("GetSampleRate", &Nz::SoundBuffer::GetSampleRate);

			soundBuffer.BindMethod("IsValid", &Nz::SoundBuffer::IsValid);

			soundBuffer.BindMethod("LoadFromFile", &Nz::SoundBuffer::LoadFromFile, Nz::SoundBufferParams());

			soundBuffer.BindStaticMethod("IsFormatSupported", &Nz::SoundBuffer::IsFormatSupported);

			// Manual
			soundBuffer.BindMethod("Create", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int index = 2;
				Nz::AudioFormat format = lua.Check<Nz::AudioFormat>(&index);
				unsigned int sampleCount = lua.Check<unsigned int>(&index);
				unsigned int sampleRate = lua.Check<unsigned int>(&index);

				std::size_t bufferSize = 0;
				const char* buffer = lua.CheckString(index, &bufferSize);
				lua.ArgCheck(buffer && bufferSize >= sampleCount * sizeof(Nz::Int16), index, "Invalid buffer");

				lua.PushBoolean(instance->Create(format, sampleCount, sampleRate, reinterpret_cast<const Nz::Int16*>(buffer)));
				return 1;
			});

			soundBuffer.BindMethod("GetSamples", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance, std::size_t /*argumentCount*/) -> int
			{
				lua.PushString(reinterpret_cast<const char*>(instance->GetSamples()), instance->GetSampleCount() * sizeof(Nz::Int16));
				return 1;
			});

			soundBuffer.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::StringStream ss("SoundBuffer(");
				if (instance->IsValid())
				{
					Nz::String filePath = instance->GetFilePath();
					if (!filePath.IsEmpty())
						ss << "File: " << filePath << ", ";

					ss << "Duration: " << instance->GetDuration() / 1000.f << "s";
				}
				ss << ')';

				lua.PushString(ss);
				return 1;
			});
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Audio classes
	*/
	void LuaBinding_Audio::Register(Nz::LuaInstance& instance)
	{
		music.Register(instance);
		sound.Register(instance);
		soundBuffer.Register(instance);
		soundEmitter.Register(instance);
	}
}
