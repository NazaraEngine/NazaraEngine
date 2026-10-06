// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Formats/libopusencSaver.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <frozen/string.h>
#include <frozen/unordered_set.h>
#include <opus/opusenc.h>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		static constexpr OpusEncCallbacks s_callbacks = {
			.write = [](void* userdata, const unsigned char* data, opus_int32 length) -> int
			{
				Stream* stream = static_cast<Stream*>(userdata);
				return SafeCast<opus_int32>(stream->Write(data, length)) != length;
			},
			.close = [](void* /*userdata*/) -> int
			{
				return 0;
			}
		};

		bool IsOpusSupported(std::string_view extension)
		{
			constexpr auto s_supportedExtensions = frozen::make_unordered_set<frozen::string>({ ".oga", ".ogg", ".ogm", ".ogv", ".ogx", ".opus" });

			return s_supportedExtensions.find(extension) != s_supportedExtensions.end();
		}

		bool SaveOpusSoundBuffer(const SoundBuffer& soundBuffer, std::string_view format, Stream& stream, const SoundBufferParams& parameters)
		{
			OggOpusComments* comments = ope_comments_create();
			ope_comments_add(comments, "ENCODER", "Nazara Engine (libopusenc)");
			NAZARA_DEFER(ope_comments_destroy(comments););

			SoundBuffer convertedSoundBuffer;
			const SoundBuffer* targetSoundBuffer = &soundBuffer;
			switch (soundBuffer.GetFormat())
			{
				case AudioFormat::Unknown:
				{
					NazaraError("invalid audio format");
					return false;
				}

				case AudioFormat::Floating32:
				case AudioFormat::Signed16:
					break;

				case AudioFormat::Signed24:
				case AudioFormat::Signed32:
					convertedSoundBuffer = soundBuffer.ConvertToFormatCopy(AudioFormat::Floating32);
					targetSoundBuffer = &convertedSoundBuffer;
					break;

				case AudioFormat::Unsigned8:
					convertedSoundBuffer = soundBuffer.ConvertToFormatCopy(AudioFormat::Signed16);
					targetSoundBuffer = &convertedSoundBuffer;
					break;
			}

			std::span<const AudioChannel> channels = soundBuffer.GetChannels();

			constexpr std::array<const AudioChannel, 1> monoChannels = { { AudioChannel::Mono } };
			constexpr std::array<const AudioChannel, 2> stereoChannels = { { AudioChannel::FrontLeft, AudioChannel::FrontRight } };

			int channelFamily = 1;
			if (std::equal(channels.begin(), channels.end(), monoChannels.begin(), monoChannels.end()) || std::equal(channels.begin(), channels.end(), stereoChannels.begin(), stereoChannels.end()))
				channelFamily = 0;

			int error;
			OggOpusEnc* encoder = ope_encoder_create_callbacks(&s_callbacks, &stream, comments, SafeCaster(soundBuffer.GetSampleRate()), SafeCaster(channels.size()), channelFamily, &error);
			if (!encoder)
			{
				NazaraError("Failed to create opus encoder: {}", ope_strerror(error));
				return false;
			}

			NAZARA_DEFER(ope_encoder_destroy(encoder););

			if (targetSoundBuffer->GetFormat() == AudioFormat::Floating32)
				error = ope_encoder_write_float(encoder, static_cast<const float*>(targetSoundBuffer->GetSamples()), SafeCaster(targetSoundBuffer->GetFrameCount()));
			else
			{
				NazaraAssert(targetSoundBuffer->GetFormat() == AudioFormat::Signed16);
				error = ope_encoder_write(encoder, static_cast<const Int16*>(targetSoundBuffer->GetSamples()), SafeCaster(targetSoundBuffer->GetFrameCount()));
			}

			if (error != 0)
			{
				NazaraError("Failed to encode audio: {}", ope_strerror(error));
				return false;
			}

			if (ope_encoder_drain(encoder) != 0)
			{
				NazaraError("Failed to finalize audio encoding: {}", ope_strerror(error));
				return false;
			}

			return true;
		}
	}

	namespace Loaders
	{
		SoundBufferSaver::Entry GetSoundBufferSaver_libopusenc()
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			SoundBufferSaver::Entry saverEntry;
			saverEntry.formatSupport = IsOpusSupported;
			saverEntry.streamSaver = SaveOpusSoundBuffer;

			return saverEntry;
		}
	}
}
