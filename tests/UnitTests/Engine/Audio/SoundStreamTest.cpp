#include <Nazara/Audio2/SoundStream.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <span>

std::filesystem::path GetAssetDir();

SCENARIO("SoundStream", "[AUDIO][SoundStream]")
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
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/Cat.flac");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 8192_ms);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::Signed16);
				CHECK(soundStream->GetFrameCount() == 786432);
				CHECK(soundStream->GetSampleRate() == 96000);
				CHECK(CompareChannels(soundStream->GetChannels(), { { Nz::AudioChannel::FrontLeft, Nz::AudioChannel::FrontRight } }));
			}
		}

		WHEN("We load a .mp3 file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/file_example_MP3_700KB.mp3");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 27'193'468_us);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::Floating32);
				CHECK(soundStream->GetFrameCount() == 870191);
				CHECK(soundStream->GetSampleRate() == 32000);
				CHECK(CompareChannels(soundStream->GetChannels(), { { Nz::AudioChannel::FrontLeft, Nz::AudioChannel::FrontRight } }));
			}
		}

		WHEN("We load a .ogg file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/The_Brabanconne.ogg");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 63'059'591_us);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::Signed16);
				CHECK(soundStream->GetFrameCount() == 5561856);
				CHECK(soundStream->GetSampleRate() == 44100);
				CHECK(CompareChannels(soundStream->GetChannels(), { { Nz::AudioChannel::FrontLeft, Nz::AudioChannel::FrontRight } }));
			}
		}

		WHEN("We load a .wav file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/explosion1.wav");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 2'490'340_us);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::Floating32);
				CHECK(soundStream->GetFrameCount() == 109824);
				CHECK(soundStream->GetSampleRate() == 44100);
				CHECK(CompareChannels(soundStream->GetChannels(), { { Nz::AudioChannel::Mono } }));
			}
		}
	}
}
