#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/ImageStream.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

std::filesystem::path GetAssetDir();

void CompareFrames(const Nz::ImageStream& gif, std::vector<Nz::UInt8>& frameData, const Nz::Image& referenceImage)
{
	Nz::Vector2ui size = gif.GetSize();
	REQUIRE(referenceImage.GetSize() == Nz::Vector3ui(size, 1));
	REQUIRE(referenceImage.GetFormat() == gif.GetPixelFormat()); //< TODO: Convert?

	REQUIRE(frameData.size() == Nz::PixelFormatInfo::ComputeSize(gif.GetPixelFormat(), size.x, size.y, 1));
	REQUIRE(std::memcmp(frameData.data(), referenceImage.GetConstPixels(), frameData.size()) == 0);
}

SCENARIO("Streamed images", "[Core][ImageStream]")
{
	using namespace Nz::Literals;

	std::vector<Nz::UInt8> frameData;

	struct ExpectedFrame
	{
		std::shared_ptr<Nz::Image> referenceImage;
		Nz::Time time;
	};

	std::filesystem::path resourcePath = GetAssetDir();

	WHEN("Loading GIF files")
	{
		GIVEN("canvas_bgnd.gif")
		{
			std::array expectedFrames = {
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_bgnd/0.png"),
					0_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_bgnd/1.png"),
					1000_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_bgnd/2.png"),
					2000_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_bgnd/3.png"),
					3000_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_bgnd/4.png"),
					4000_ms
				}
			};

			std::shared_ptr<Nz::ImageStream> gif = Nz::ImageStream::OpenFromFile(resourcePath / "Utility/GIF/canvas_bgnd.gif");
			REQUIRE(gif);

			Nz::Vector2ui size = gif->GetSize();
			CHECK(size == Nz::Vector2ui(100, 100));
			CHECK(gif->GetFrameCount() == expectedFrames.size());
			CHECK(gif->GetPixelFormat() == Nz::PixelFormat::RGBA8);

			frameData.resize(Nz::PixelFormatInfo::ComputeSize(gif->GetPixelFormat(), size.x, size.y, 1));

			// Decode all frames in order
			Nz::Time frameTime;
			for (ExpectedFrame& expectedFrame : expectedFrames)
			{
				REQUIRE(expectedFrame.referenceImage);
				REQUIRE(gif->DecodeNextFrame(frameData.data(), &frameTime));

				CHECK(frameTime == expectedFrame.time);

				CompareFrames(*gif, frameData, *expectedFrame.referenceImage);
			}

			// Decoding the post-the-end frame fails but gives the end frametime
			REQUIRE_FALSE(gif->DecodeNextFrame(frameData.data(), &frameTime));
			CHECK(frameTime == 5000_ms);

			// Decode frames in arbitrary order, to ensure results are corrects
			for (std::size_t frameIndex : { 2, 0, 3, 1, 4 })
			{
				INFO("Decoding frame " << frameIndex);

				ExpectedFrame& expectedFrame = expectedFrames[frameIndex];
				gif->Seek(frameIndex);

				REQUIRE(gif->DecodeNextFrame(frameData.data(), &frameTime));
				CHECK(frameTime == expectedFrame.time);

				CompareFrames(*gif, frameData, *expectedFrame.referenceImage);
			}
		}

		GIVEN("canvas_prev.gif")
		{
			std::array expectedFrames = {
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_prev/0.png"),
					0_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_prev/1.png"),
					100_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_prev/2.png"),
					1100_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_prev/3.png"),
					2100_ms
				},
				ExpectedFrame{
					Nz::Image::LoadFromFile(resourcePath / "Utility/GIF/canvas_prev/4.png"),
					3100_ms
				}
			};

			std::shared_ptr<Nz::ImageStream> gif = Nz::ImageStream::OpenFromFile(resourcePath / "Utility/GIF/canvas_prev.gif");
			REQUIRE(gif);

			Nz::Vector2ui size = gif->GetSize();
			CHECK(size == Nz::Vector2ui(100, 100));
			CHECK(gif->GetFrameCount() == expectedFrames.size());
			CHECK(gif->GetPixelFormat() == Nz::PixelFormat::RGBA8);

			frameData.resize(Nz::PixelFormatInfo::ComputeSize(gif->GetPixelFormat(), size.x, size.y, 1));

			// Decode all frames in order
			Nz::Time frameTime;
			for (ExpectedFrame& expectedFrame : expectedFrames)
			{
				REQUIRE(expectedFrame.referenceImage);
				REQUIRE(gif->DecodeNextFrame(frameData.data(), &frameTime));

				CHECK(frameTime == expectedFrame.time);

				CompareFrames(*gif, frameData, *expectedFrame.referenceImage);
			}

			// Decoding the post-the-end frame fails but gives the end frametime
			REQUIRE_FALSE(gif->DecodeNextFrame(frameData.data(), &frameTime));
			CHECK(frameTime == 4100_ms);

			// Decode frames in arbitrary order, to ensure results are corrects
			for (std::size_t frameIndex : { 2, 0, 3, 1, 4 })
			{
				INFO("Decoding frame " << frameIndex);

				ExpectedFrame& expectedFrame = expectedFrames[frameIndex];
				gif->Seek(frameIndex);

				REQUIRE(gif->DecodeNextFrame(frameData.data(), &frameTime));
				CHECK(frameTime == expectedFrame.time);

				CompareFrames(*gif, frameData, *expectedFrame.referenceImage);
			}
		}
	}
}
