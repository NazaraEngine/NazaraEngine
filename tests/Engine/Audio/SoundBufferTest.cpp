#include <Nazara/Audio/SoundBuffer.hpp>
#include <catch2/catch.hpp>

std::filesystem::path GetResourceDir();

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	GIVEN("A sound buffer")
	{
		WHEN("We load a .flac file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetResourceDir() / "Engine/Audio/Cat.flac");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 8192);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleRate() == 96000);
			}
		}

		WHEN("We load a .mp3 file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetResourceDir() / "file_example_MP3_700KB.mp3");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 27193);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleRate() == 32000);
			}
		}

		WHEN("We load a .ogg file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetResourceDir() / "Engine/Audio/The_Brabanconne.ogg");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 63059);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleRate() == 44100);
			}
		}

		WHEN("We load a .wav file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetResourceDir() / "explosion.wav");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 2064);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleRate() == 48000);
			}
		}
	}
}
