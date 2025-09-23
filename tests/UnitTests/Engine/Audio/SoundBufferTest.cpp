#include <Nazara/Audio/SoundBuffer.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

std::filesystem::path GetAssetDir();

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	using namespace Nz::Literals;

	GIVEN("A sound buffer")
	{
		WHEN("We load a .flac file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/Cat.flac");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 8192_ms);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleCount() == 1572864);
				CHECK(soundBuffer->GetSampleRate() == 96000);
			}
		}

		WHEN("We load a .mp3 file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/file_example_MP3_700KB.mp3");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 27'193'468_us);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleCount() == 1740382);
				CHECK(soundBuffer->GetSampleRate() == 32000);
			}
		}

		WHEN("We load a .ogg file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/The_Brabanconne.ogg");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 63'059'591_us);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundBuffer->GetSampleCount() == 5561856);
				CHECK(soundBuffer->GetSampleRate() == 44100);
			}
		}

		WHEN("We load a .wav file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/explosion1.wav");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 2'490'340_us);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::I16_Mono);
				CHECK(soundBuffer->GetSampleCount() == 109824);
				CHECK(soundBuffer->GetSampleRate() == 44100);
			}
		}
	}
}
