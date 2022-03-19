#include <Nazara/Audio/SoundStream.hpp>
#include <catch2/catch.hpp>

std::filesystem::path GetResourceDir();

SCENARIO("SoundStream", "[AUDIO][SoundStream]")
{
	GIVEN("A sound buffer")
	{
		WHEN("We load a .flac file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetResourceDir() / "Engine/Audio/Cat.flac");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 8192);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleRate() == 96000);
			}
		}

		WHEN("We load a .mp3 file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetResourceDir() / "file_example_MP3_700KB.mp3");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 27193);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleRate() == 32000);
			}
		}

		WHEN("We load a .ogg file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetResourceDir() / "Engine/Audio/The_Brabanconne.ogg");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 63059);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleRate() == 44100);
			}
		}

		WHEN("We load a .wav file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetResourceDir() / "explosion.wav");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 2064);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleRate() == 48000);
			}
		}
	}
}
