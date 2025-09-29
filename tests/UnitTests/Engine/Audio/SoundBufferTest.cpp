#include <Nazara/Audio2/SoundBuffer.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <span>

std::filesystem::path GetAssetDir();

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	using namespace Nz::Literals;

	auto CompareChannels = [](std::span<const Nz::AudioChannel> channels1, std::span<const Nz::AudioChannel> referenceChannels)
	{
		return std::equal(channels1.begin(), channels1.end(), referenceChannels.begin(), referenceChannels.end());
	};

	GIVEN("A sound buffer")
	{
		WHEN("We load a .flac file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/Cat.flac");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 8192_ms);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::Signed16);
				CHECK(soundBuffer->GetFrameCount() == 786432);
				CHECK(soundBuffer->GetSampleRate() == 96000);
				CHECK(CompareChannels(soundBuffer->GetChannels(), { { Nz::AudioChannel::FrontLeft, Nz::AudioChannel::FrontRight } }));
			}
		}

		WHEN("We load a .mp3 file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/file_example_MP3_700KB.mp3");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 27'193'468_us);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::Signed16);
				CHECK(soundBuffer->GetFrameCount() == 870191);
				CHECK(soundBuffer->GetSampleRate() == 32000);
				CHECK(CompareChannels(soundBuffer->GetChannels(), { { Nz::AudioChannel::FrontLeft, Nz::AudioChannel::FrontRight } }));
			}
		}

		WHEN("We load a .ogg file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/The_Brabanconne.ogg");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 63'059'591_us);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::Signed16);
				CHECK(soundBuffer->GetFrameCount() == 2780928);
				CHECK(soundBuffer->GetSampleRate() == 44100);
				CHECK(CompareChannels(soundBuffer->GetChannels(), { { Nz::AudioChannel::FrontLeft, Nz::AudioChannel::FrontRight } }));
			}
		}

		WHEN("We load a .wav file")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/explosion1.wav");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundBuffer->GetDuration() == 2'490'340_us);
				CHECK(soundBuffer->GetFormat() == Nz::AudioFormat::Signed16);
				CHECK(soundBuffer->GetFrameCount() == 109824);
				CHECK(soundBuffer->GetSampleRate() == 44100);
				CHECK(CompareChannels(soundBuffer->GetChannels(), { { Nz::AudioChannel::Mono } }));
			}
		}
	}
}
