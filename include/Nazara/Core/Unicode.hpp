// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UNICODE_HPP
#define NAZARA_UNICODE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>

namespace Nz
{
	class NAZARA_CORE_API Unicode
	{
		public:
			Unicode() = delete;
			~Unicode() = delete;
			/*
				Unicode category:
				-Values between 0x01 and 0x80 specify the category
				-Values between 0x100 and 0x10000 specify the subcategory
			*/
			enum Category : UInt16
			{
				// Category not handled by Nazara
				Category_NoCategory				  = 0,

				// Letters
				Category_Letter					  = 0x01,						   // L
				Category_Letter_Lowercase		  = Category_Letter | 0x0100,	   // Ll
				Category_Letter_Modifier		  = Category_Letter | 0x0200,	   // Lm
				Category_Letter_Other			  = Category_Letter | 0x0400,	   // Lo
				Category_Letter_Titlecase		  = Category_Letter | 0x0800,	   // Lt
				Category_Letter_Uppercase		  = Category_Letter | 0x1000,	   // Lu

				// Marks
				Category_Mark					  = 0x02,						   // M
				Category_Mark_Enclosing			  = Category_Mark | 0x100,		   // Me
				Category_Mark_NonSpacing		  = Category_Mark | 0x200,		   // Mn
				Category_Mark_SpacingCombining	  = Category_Mark | 0x400,		   // Mc

				// Numbers
				Category_Number					  = 0x04,						   // N
				Category_Number_DecimalDigit	  = Category_Number | 0x100,	   // Nd
				Category_Number_Letter			  = Category_Number | 0x200,	   // Nl
				Category_Number_Other			  = Category_Number | 0x400,	   // No

				// Others
				Category_Other					  = 0x08,						   // C
				Category_Other_Control			  = Category_Other | 0x0100,	   // Cc
				Category_Other_Format			  = Category_Other | 0x0200,	   // Cf
				Category_Other_NotAssigned		  = Category_Other | 0x0400,	   // Cn
				Category_Other_PrivateUse		  = Category_Other | 0x0800,	   // Co
				Category_Other_Surrogate		  = Category_Other | 0x1000,	   // Cs

				// Punctuations
				Category_Punctuation			  = 0x10,						   // P
				Category_Punctuation_Close		  = Category_Punctuation | 0x0100, // Pe
				Category_Punctuation_Connector	  = Category_Punctuation | 0x0200, // Pc
				Category_Punctuation_Dash		  = Category_Punctuation | 0x0400, // Pd
				Category_Punctuation_FinalQuote	  = Category_Punctuation | 0x0800, // Pf
				Category_Punctuation_InitialQuote =	Category_Punctuation | 0x1000, // Pi
				Category_Punctuation_Open		  =	Category_Punctuation | 0x2000, // Ps
				Category_Punctuation_Other		  =	Category_Punctuation | 0x4000, // Po

				// Spaces
				Category_Separator				  = 0x20,						   // Z
				Category_Separator_Line			  = Category_Separator | 0x0100,   // Zl
				Category_Separator_Paragraph	  = Category_Separator | 0x0200,   // Zp
				Category_Separator_Space		  = Category_Separator | 0x0400,   // Zs

				// Symbols
				Category_Symbol					  = 0x40,						   // S
				Category_Symbol_Currency		  = Category_Symbol | 0x0100,	   // Sc
				Category_Symbol_Math			  = Category_Symbol | 0x0200,	   // Sm
				Category_Symbol_Modifier		  = Category_Symbol | 0x0400,	   // Sk
				Category_Symbol_Other			  = Category_Symbol | 0x0800	   // So
			};

			enum Direction : UInt8
			{
				Direction_Arabic_Letter,              // AL
				Direction_Arabic_Number,              // AN
				Direction_Boundary_Neutral,           // BN
				Direction_Common_Separator,           // CS
				Direction_European_Number,            // EN
				Direction_European_Separator,         // ES
				Direction_European_Terminator,        // ET
				Direction_First_Strong_Isolate,       // FSI
				Direction_Left_To_Right,              // L
				Direction_Left_To_Right_Embedding,    // LRE
				Direction_Left_To_Right_Isolate,      // LRI
				Direction_Left_To_Right_Override,     // LRO
				Direction_Nonspacing_Mark,            // NSM
				Direction_Other_Neutral,              // ON
				Direction_Paragraph_Separator,        // B
				Direction_Pop_Directional_Formatting, // PDF
				Direction_Pop_Directional_Isolate,    // PDI
				Direction_Right_To_Left,              // R
				Direction_Right_To_Left_Embedding,    // RLE
				Direction_Right_To_Left_Isolate,      // RLI
				Direction_Right_To_Left_Override,     // RLO
				Direction_Segment_Separator,          // S
				Direction_White_Space                 // WS
			};

			static Category GetCategory(char32_t character);
			static Direction GetDirection(char32_t character);
			static char32_t GetLowercase(char32_t character);
			static char32_t GetTitlecase(char32_t character);
			static char32_t GetUppercase(char32_t character);
	};
}

#endif // NAZARA_UNICODE_HPP
