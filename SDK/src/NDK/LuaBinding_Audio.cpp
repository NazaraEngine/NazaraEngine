// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaBinding.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds Audio module to Lua
	*/

	void LuaBinding::BindAudio()
	{
		/*********************************** Nz::Music **********************************/
		musicClass.Inherit(soundEmitter);

		musicClass.BindDefaultConstructor();

		//musicClass.SetMethod("Create", &Nz::Music::Create);
		//musicClass.SetMethod("Destroy", &Nz::Music::Destroy);

		musicClass.BindMethod("EnableLooping", &Nz::Music::EnableLooping);

		musicClass.BindMethod("GetDuration", &Nz::Music::GetDuration);
		musicClass.BindMethod("GetFormat", &Nz::Music::GetFormat);
		musicClass.BindMethod("GetPlayingOffset", &Nz::Music::GetPlayingOffset);
		musicClass.BindMethod("GetSampleCount", &Nz::Music::GetSampleCount);
		musicClass.BindMethod("GetSampleRate", &Nz::Music::GetSampleRate);
		musicClass.BindMethod("GetStatus", &Nz::Music::GetStatus);

		musicClass.BindMethod("IsLooping", &Nz::Music::IsLooping);

		musicClass.BindMethod("OpenFromFile", &Nz::Music::OpenFromFile, Nz::MusicParams());

		musicClass.BindMethod("Pause", &Nz::Music::Pause);
		musicClass.BindMethod("Play", &Nz::Music::Play);

		musicClass.BindMethod("SetPlayingOffset", &Nz::Music::SetPlayingOffset);

		musicClass.BindMethod("Stop", &Nz::Music::Stop);

		// Manual
		musicClass.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Music& music) -> int
		{
			Nz::StringStream stream("Music(");
			stream << music.GetFilePath() << ')';

			lua.PushString(stream);
			return 1;
		});

		/*********************************** Nz::Sound **********************************/
		soundClass.Inherit(soundEmitter);

		soundClass.BindDefaultConstructor();

		soundClass.BindMethod("GetBuffer", &Nz::Sound::GetBuffer);

		soundClass.BindMethod("IsPlayable", &Nz::Sound::IsPlayable);
		soundClass.BindMethod("IsPlaying", &Nz::Sound::IsPlaying);

		soundClass.BindMethod("LoadFromFile", &Nz::Sound::LoadFromFile, Nz::SoundBufferParams());

		soundClass.BindMethod("SetPlayingOffset", &Nz::Sound::SetPlayingOffset);

		// Manual
		soundClass.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Sound& sound) -> int
		{
			Nz::StringStream stream("Sound(");
			if (const Nz::SoundBuffer* buffer = sound.GetBuffer())
				stream << buffer;

			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		/*********************************** Nz::SoundBuffer **********************************/
		soundBuffer.SetConstructor([] (Nz::LuaInstance& lua, Nz::SoundBufferRef* instance)
		{
			NazaraUnused(lua);

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
		soundBuffer.BindMethod("Create", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance) -> int
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

		soundBuffer.BindMethod("GetSamples", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance) -> int
		{
			lua.PushString(reinterpret_cast<const char*>(instance->GetSamples()), instance->GetSampleCount() * sizeof(Nz::Int16));
			return 1;
		});

		soundBuffer.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::SoundBufferRef& instance) -> int
		{
			Nz::StringStream stream("SoundBuffer(");
			if (instance->IsValid())
			{
				Nz::String filePath = instance->GetFilePath();
				if (!filePath.IsEmpty())
					stream << "File: " << filePath << ", ";

				stream << "Duration: " << instance->GetDuration() / 1000.f << "s";
			}
			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		/*********************************** Nz::SoundEmitter **********************************/
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

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Audio classes
	*/

	void LuaBinding::RegisterAudio(Nz::LuaInstance& instance)
	{
		musicClass.Register(instance);
		soundClass.Register(instance);
		soundBuffer.Register(instance);
		soundEmitter.Register(instance);
	}
}
