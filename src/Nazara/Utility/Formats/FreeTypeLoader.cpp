// Copyright (C) 2020 Jérôme Leclercq - 2009 Cruden BV
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/FreeTypeLoader.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include FT_OUTLINE_H
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/FontData.hpp>
#include <Nazara/Utility/FontGlyph.hpp>
#include <memory>
#include <set>
#include <string>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		class FreeTypeLibrary;

		FT_Library s_library;
		FT_Stroker s_stroker;
		std::shared_ptr<FreeTypeLibrary> s_libraryOwner;
		constexpr float s_scaleFactor = 1 << 6;
		constexpr float s_invScaleFactor = 1.f / s_scaleFactor;

		extern "C"
		unsigned long FT_StreamRead(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count)
		{
			// http://www.freetype.org/freetype2/docs/reference/ft2-system_interface.html#FT_Stream_IoFunc
			Stream& inputStream = *static_cast<Stream*>(stream->descriptor.pointer);

			// La valeur de count indique une opération de lecture ou de positionnement
			if (count > 0)
			{
				// Dans le premier cas, une erreur est symbolisée par un retour nul
				if (inputStream.SetCursorPos(offset))
					return static_cast<unsigned long>(inputStream.Read(buffer, count));
				else
					return 0;
			}
			else
			{
				// Dans le second cas, une erreur est symbolisée par un retour non-nul
				if (inputStream.SetCursorPos(offset))
					return 0;
				else
					return 42; // La réponse à la grande question
			}
		}

		extern "C"
		void FT_StreamClose(FT_Stream stream)
		{
			// http://www.freetype.org/freetype2/docs/reference/ft2-system_interface.html#FT_Stream_CloseFunc
			// Les streams dans Nazara ne se ferment pas explicitement
			NazaraUnused(stream);
		}

		class FreeTypeLibrary
		{
			// Cette classe ne sert qu'à être utilisée avec un std::shared_ptr
			// pour ne libérer FreeType que lorsque plus personne ne l'utilise

			public:
				FreeTypeLibrary()
				{
					if (FT_Stroker_New(s_library, &s_stroker) != 0)
					{
						NazaraWarning("Failed to load FreeType stroker, outline will not be possible");
						s_stroker = nullptr; //< Just in case
					}
				}

				~FreeTypeLibrary()
				{
					if (s_stroker)
					{
						FT_Stroker_Done(s_stroker);
						s_stroker = nullptr;
					}

					FT_Done_FreeType(s_library);
					s_library = nullptr;
				}
		};

		class FreeTypeStream : public FontData
		{
			public:
				FreeTypeStream() :
				m_face(nullptr),
				m_library(s_libraryOwner),
				m_characterSize(0)
				{
				}

				~FreeTypeStream()
				{
					if (m_face)
						FT_Done_Face(m_face);
				}

				bool Check()
				{
					// Test d'ouverture (http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Open_Face)
					return FT_Open_Face(s_library, &m_args, -1, nullptr) == 0;
				}

				bool ExtractGlyph(unsigned int characterSize, char32_t character, TextStyleFlags style, float outlineThickness, FontGlyph* dst) override
				{
					#ifdef NAZARA_DEBUG
					if (!dst)
					{
						NazaraError("Glyph destination cannot be null");
						return false;
					}
					#endif

					SetCharacterSize(characterSize);

					if (FT_Load_Char(m_face, character, FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL) != 0)
					{
						NazaraError("Failed to load character");
						return false;
					}

					FT_GlyphSlot glyphSlot = m_face->glyph;

					FT_Glyph glyph;
					if (FT_Get_Glyph(glyphSlot, &glyph) != 0)
					{
						NazaraError("Failed to extract glyph");
						return false;
					}
					CallOnExit destroyGlyph([&]() { FT_Done_Glyph(glyph); });

					const FT_Pos boldStrength = 2 << 6;

					bool embolden = (style & TextStyle_Bold) != 0;
					bool hasOutlineFormat = (glyph->format == FT_GLYPH_FORMAT_OUTLINE);

					dst->advance = (embolden) ? boldStrength >> 6 : 0;

					if (hasOutlineFormat)
					{
						if (embolden)
						{
							// FT_Glyph can be casted to FT_OutlineGlyph if format is FT_GLYPH_FORMAT_OUTLINE
							FT_OutlineGlyph outlineGlyph = reinterpret_cast<FT_OutlineGlyph>(glyph);
							if (FT_Outline_Embolden(&outlineGlyph->outline, boldStrength) != 0)
							{
								NazaraError("Failed to embolden glyph");
								return false;
							}
						}

						if (outlineThickness > 0.f)
						{
							FT_Stroker_Set(s_stroker, static_cast<FT_Fixed>(s_scaleFactor * outlineThickness), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
							if (FT_Glyph_Stroke(&glyph, s_stroker, 1) != 0)
							{
								NazaraError("Failed to outline glyph");
								return false;
							}
						}
					}

					if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1) != 0)
					{
						NazaraError("Failed to convert glyph to bitmap");
						return false;
					}

					FT_Bitmap& bitmap = reinterpret_cast<FT_BitmapGlyph>(glyph)->bitmap;

					// Dans le cas où nous voulons des caractères gras mais que nous n'avons pas pu agir plus tôt
					// nous demandons à FreeType d'agir directement sur le bitmap généré
					if (embolden)
					{
						// http://www.freetype.org/freetype2/docs/reference/ft2-bitmap_handling.html#FT_Bitmap_Embolden
						FT_Bitmap_Embolden(s_library, &bitmap, boldStrength, boldStrength);
					}

					int outlineThicknessInt = static_cast<int>(outlineThickness * 2.f + 0.5f); //< round it
					dst->advance += glyphSlot->metrics.horiAdvance >> 6;
					dst->aabb.x = glyphSlot->metrics.horiBearingX >> 6;
					dst->aabb.y = -(glyphSlot->metrics.horiBearingY >> 6); // Inversion du repère
					dst->aabb.width = (glyphSlot->metrics.width >> 6) + outlineThicknessInt;
					dst->aabb.height = (glyphSlot->metrics.height >> 6) + outlineThicknessInt;

					unsigned int width = bitmap.width;
					unsigned int height = bitmap.rows;

					if (width > 0 && height > 0)
					{
						dst->image.Create(ImageType_2D, PixelFormat_A8, width, height);
						UInt8* pixels = dst->image.GetPixels();

						const UInt8* data = bitmap.buffer;

						// Selon la documentation FreeType, le glyphe peut être encodé en format A8 (huit bits d'alpha par pixel)
						// ou au format A1 (un bit d'alpha par pixel).
						// Cependant dans un cas comme dans l'autre, il nous faut gérer le pitch (les données peuvent ne pas être contigues)
						// ainsi que le padding dans le cas du format A1 (Chaque ligne prends un nombre fixe d'octets)
						if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
						{
							// Format A1
							for (unsigned int y = 0; y < height; ++y)
							{
								for (unsigned int x = 0; x < width; ++x)
									*pixels++ = (data[x/8] & ((1 << (7 - x%8) != 0) ? 255 : 0));

								data += bitmap.pitch;
							}
						}
						else
						{
							// Format A8
							if (bitmap.pitch == static_cast<int>(width*sizeof(UInt8))) // Pouvons-nous copier directement ?
								dst->image.Update(bitmap.buffer); //< Small optimization
							else
							{
								for (unsigned int y = 0; y < height; ++y)
								{
									std::memcpy(pixels, data, width*sizeof(UInt8));
									data += bitmap.pitch;
									pixels += width*sizeof(UInt8);
								}
							}
						}
					}
					else
						dst->image.Destroy(); // On s'assure que l'image ne contient alors rien

					return true;
				}

				std::string GetFamilyName() const override
				{
					return m_face->family_name;
				}

				std::string GetStyleName() const override
				{
					return m_face->style_name;
				}

				bool HasKerning() const override
				{
					return FT_HAS_KERNING(m_face) != 0;
				}

				bool IsScalable() const override
				{
					return FT_IS_SCALABLE(m_face) != 0;
				}

				bool Open()
				{
					return FT_Open_Face(s_library, &m_args, 0, &m_face) == 0;
				}

				int QueryKerning(unsigned int characterSize, char32_t first, char32_t second) const override
				{
					if (FT_HAS_KERNING(m_face))
					{
						SetCharacterSize(characterSize);

						FT_Vector kerning;
						FT_Get_Kerning(m_face, FT_Get_Char_Index(m_face, first), FT_Get_Char_Index(m_face, second), FT_KERNING_DEFAULT, &kerning);

						if (!FT_IS_SCALABLE(m_face))
							return kerning.x; // Taille déjà précisée en pixels dans ce cas

						return kerning.x >> 6;
					}
					else
						return 0;
				}

				unsigned int QueryLineHeight(unsigned int characterSize) const override
				{
					SetCharacterSize(characterSize);

					// http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Size_Metrics
					return m_face->size->metrics.height >> 6;
				}

				float QueryUnderlinePosition(unsigned int characterSize) const override
				{
					if (FT_IS_SCALABLE(m_face))
					{
						SetCharacterSize(characterSize);

						// http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_FaceRec
						return static_cast<float>(FT_MulFix(m_face->underline_position, m_face->size->metrics.y_scale)) * s_invScaleFactor;
					}
					else
						return characterSize / 10.f; // Joker ?
				}

				float QueryUnderlineThickness(unsigned int characterSize) const override
				{
					if (FT_IS_SCALABLE(m_face))
					{
						SetCharacterSize(characterSize);

						// http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_FaceRec
						return static_cast<float>(FT_MulFix(m_face->underline_thickness, m_face->size->metrics.y_scale)) * s_invScaleFactor;
					}
					else
						return characterSize/15.f; // Joker ?
				}

				bool SetFile(const std::filesystem::path& filePath)
				{
					std::unique_ptr<File> file = std::make_unique<File>();
					if (!file->Open(filePath, OpenMode_ReadOnly))
					{
						NazaraError("Failed to open stream from file: " + Error::GetLastError());
						return false;
					}
					m_ownedStream = std::move(file);

					SetStream(*m_ownedStream);
					return true;
				}

				void SetMemory(const void* data, std::size_t size)
				{
					m_ownedStream = std::make_unique<MemoryView>(data, size);
					SetStream(*m_ownedStream);
				}

				void SetStream(Stream& stream)
				{
					m_stream.base = nullptr;
					m_stream.close = FT_StreamClose;
					m_stream.descriptor.pointer = &stream;
					m_stream.read = FT_StreamRead;
					m_stream.pos = 0;
					m_stream.size = static_cast<unsigned long>(stream.GetSize());

					m_args.driver = nullptr;
					m_args.flags = FT_OPEN_STREAM;
					m_args.stream = &m_stream;
				}

				bool SupportsOutline(float /*outlineThickness*/) const override
				{
					return s_stroker != nullptr;
				}

				bool SupportsStyle(TextStyleFlags style) const override
				{
					///TODO
					return style == TextStyle_Regular || style == TextStyle_Bold;
				}

			private:
				void SetCharacterSize(unsigned int characterSize) const
				{
					if (m_characterSize != characterSize)
					{
						FT_Set_Pixel_Sizes(m_face, 0, characterSize);
						m_characterSize = characterSize;
					}
				}

				FT_Open_Args m_args;
				FT_Face m_face;
				FT_StreamRec m_stream;
				std::shared_ptr<FreeTypeLibrary> m_library;
				std::unique_ptr<Stream> m_ownedStream;
				mutable unsigned int m_characterSize;
		};

		bool IsSupported(const std::string& extension)
		{
			///FIXME: Je suppose qu'il en manque quelques unes..
			static std::set<std::string> supportedExtensions = {
				"afm", "bdf", "cff", "cid", "dfont", "fnt", "fon", "otf", "pfa", "pfb", "pfm", "pfr", "sfnt", "ttc", "tte", "ttf"
			};

			return supportedExtensions.find(extension) != supportedExtensions.end();
		}

		Ternary Check(Stream& stream, const FontParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeFreeTypeLoader", &skip) && skip)
				return Ternary::False;

			FreeTypeStream face;
			face.SetStream(stream);

			if (face.Check())
				return Ternary::True;
			else
				return Ternary::False;
		}

		FontRef LoadFile(const std::filesystem::path& filePath, const FontParams& parameters)
		{
			NazaraUnused(parameters);

			std::unique_ptr<FreeTypeStream> face = std::make_unique<FreeTypeStream>();
			if (!face->SetFile(filePath))
			{
				NazaraError("Failed to open file");
				return nullptr;
			}

			if (!face->Open())
			{
				NazaraError("Failed to open face");
				return nullptr;
			}

			FontRef font = Font::New();
			if (font->Create(face.get()))
			{
				face.release();
				return font;
			}
			else
				return nullptr;
		}

		FontRef LoadMemory(const void* data, std::size_t size, const FontParams& parameters)
		{
			NazaraUnused(parameters);

			std::unique_ptr<FreeTypeStream> face(new FreeTypeStream);
			face->SetMemory(data, size);

			if (!face->Open())
			{
				NazaraError("Failed to open face");
				return nullptr;
			}

			FontRef font = Font::New();
			if (font->Create(face.get()))
			{
				face.release();
				return font;
			}
			else
				return nullptr;
		}

		FontRef LoadStream(Stream& stream, const FontParams& parameters)
		{
			NazaraUnused(parameters);

			std::unique_ptr<FreeTypeStream> face(new FreeTypeStream);
			face->SetStream(stream);

			if (!face->Open())
			{
				NazaraError("Failed to open face");
				return nullptr;
			}

			FontRef font = Font::New();
			if (font->Create(face.get()))
			{
				face.release();
				return font;
			}
			else
				return nullptr;
		}
	}

	namespace Loaders
	{
		void RegisterFreeType()
		{
			if (FT_Init_FreeType(&s_library) == 0)
			{
				s_libraryOwner = std::make_shared<FreeTypeLibrary>();
				FontLoader::RegisterLoader(IsSupported, Check, LoadStream, LoadFile, LoadMemory);
			}
			else
			{
				s_library = nullptr; // On s'assure que le pointeur ne pointe pas sur n'importe quoi
				NazaraWarning("Failed to initialize FreeType library");
			}
		}

		void UnregisterFreeType()
		{
			if (s_library)
			{
				FontLoader::UnregisterLoader(IsSupported, Check, LoadStream, LoadFile, LoadMemory);
				s_libraryOwner.reset();
			}
		}
	}
}
