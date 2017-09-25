// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STRING_HPP
#define NAZARA_STRING_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <cstdarg>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace Nz
{
	class SerializationContext;

	class NAZARA_CORE_API String
	{
		public:
			enum Flags
			{
				None            = 0x00, // Default mode
				CaseInsensitive = 0x01, // Case insensitive
				HandleUtf8      = 0x02, // Considers bytes as a list of UTF-8 characters
				TrimOnlyLeft    = 0x04, // Trim(med), only cut the left part of the string
				TrimOnlyRight   = 0x08  // Trim(med), only cut the right part of the string
			};

			String();
			explicit String(char character);
			String(std::size_t rep, char character);
			String(std::size_t rep, const char* string);
			String(std::size_t rep, const char* string, std::size_t length);
			String(std::size_t rep, const String& string);
			String(const char* string);
			String(const char* string, std::size_t length);
			String(const std::string& string);
			String(const String& string) = default;
			String(String&& string) noexcept = default;
			~String() = default;

			String& Append(char character);
			String& Append(const char* string);
			String& Append(const char* string, std::size_t length);
			String& Append(const String& string);

			void Clear(bool keepBuffer = false);

			bool Contains(char character, std::intmax_t start = 0, UInt32 flags = None) const;
			bool Contains(const char* string, std::intmax_t start = 0, UInt32 flags = None) const;
			bool Contains(const String& string, std::intmax_t start = 0, UInt32 flags = None) const;

			unsigned int Count(char character, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int Count(const char* string, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int Count(const String& string, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int CountAny(const char* string, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int CountAny(const String& string, std::intmax_t start = 0, UInt32 flags = None) const;

			bool EndsWith(char character, UInt32 flags = None) const;
			bool EndsWith(const char* string, UInt32 flags = None) const;
			bool EndsWith(const char* string, std::size_t length, UInt32 flags = None) const;
			bool EndsWith(const String& string, UInt32 flags = None) const;

			std::size_t Find(char character, std::intmax_t start = 0, UInt32 flags = None) const;
			std::size_t Find(const char* string, std::intmax_t start = 0, UInt32 flags = None) const;
			std::size_t Find(const String& string, std::intmax_t start = 0, UInt32 flags = None) const;
			std::size_t FindAny(const char* string, std::intmax_t start = 0, UInt32 flags = None) const;
			std::size_t FindAny(const String& string, std::intmax_t start = 0, UInt32 flags = None) const;
			std::size_t FindLast(char character, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindLast(const char* string, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindLast(const String& string, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindLastAny(const char* string, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindLastAny(const String& string, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindLastWord(const char* string, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindLastWord(const String& string, std::intmax_t start = -1, UInt32 flags = None) const;
			std::size_t FindWord(const char* string, std::intmax_t start = 0, UInt32 flags = None) const;
			std::size_t FindWord(const String& string, std::intmax_t start = 0, UInt32 flags = None) const;

			char* GetBuffer();
			std::size_t GetCapacity() const;
			std::size_t GetCharacterPosition(std::size_t characterIndex) const;
			const char* GetConstBuffer() const;
			std::size_t GetLength() const;
			std::size_t GetSize() const;
			std::string GetUtf8String() const;
			std::u16string GetUtf16String() const;
			std::u32string GetUtf32String() const;
			std::wstring GetWideString() const;
			String GetWord(unsigned int index, UInt32 flags = None) const;
			std::size_t GetWordPosition(unsigned int index, UInt32 flags = None) const;

			String& Insert(std::intmax_t pos, char character);
			String& Insert(std::intmax_t pos, const char* string);
			String& Insert(std::intmax_t pos, const char* string, std::size_t length);
			String& Insert(std::intmax_t pos, const String& string);

			bool IsEmpty() const;
			bool IsNull() const;
			bool IsNumber(UInt8 radix = 10, UInt32 flags = CaseInsensitive) const;

			bool Match(const char* pattern) const;
			bool Match(const String& pattern) const;

			String& Prepend(char character);
			String& Prepend(const char* string);
			String& Prepend(const char* string, std::size_t length);
			String& Prepend(const String& string);

			unsigned int Replace(char oldCharacter, char newCharacter, std::intmax_t start = 0, UInt32 flags = None);
			unsigned int Replace(const char* oldString, const char* replaceString, std::intmax_t start = 0, UInt32 flags = None);
			unsigned int Replace(const char* oldString, std::size_t oldLength, const char* replaceString, std::size_t replaceLength, std::intmax_t start = 0, UInt32 flags = None);
			unsigned int Replace(const String& oldString, const String& replaceString, std::intmax_t start = 0, UInt32 flags = None);
			unsigned int ReplaceAny(const char* oldCharacters, char replaceCharacter, std::intmax_t start = 0, UInt32 flags = None);
			//unsigned int ReplaceAny(const char* oldCharacters, const char* replaceString, std::intmax_t start = 0, UInt32 flags = None);
			//unsigned int ReplaceAny(const String& oldCharacters, const String& replaceString, std::intmax_t start = 0, UInt32 flags = None);

			void Reserve(std::size_t bufferSize);

			String& Resize(std::intmax_t size, UInt32 flags = None);
			String Resized(std::intmax_t size, UInt32 flags = None) const;

			String& Reverse();
			String Reversed() const;

			String& Set(char character);
			String& Set(std::size_t rep, char character);
			String& Set(std::size_t rep, const char* string);
			String& Set(std::size_t rep, const char* string, std::size_t length);
			String& Set(std::size_t rep, const String& string);
			String& Set(const char* string);
			String& Set(const char* string, std::size_t length);
			String& Set(const std::string& string);
			String& Set(const String& string);
			String& Set(String&& string) noexcept;

			String Simplified(UInt32 flags = None) const;
			String& Simplify(UInt32 flags = None);

			unsigned int Split(std::vector<String>& result, char separation = ' ', std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int Split(std::vector<String>& result, const char* separation, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int Split(std::vector<String>& result, const char* separation, std::size_t length, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int Split(std::vector<String>& result, const String& separation, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int SplitAny(std::vector<String>& result, const char* separations, std::intmax_t start = 0, UInt32 flags = None) const;
			unsigned int SplitAny(std::vector<String>& result, const String& separations, std::intmax_t start = 0, UInt32 flags = None) const;

			bool StartsWith(char character, UInt32 flags = None) const;
			bool StartsWith(const char* string, UInt32 flags = None) const;
			bool StartsWith(const String& string, UInt32 flags = None) const;

			String SubString(std::intmax_t startPos, std::intmax_t endPos = -1) const;
			String SubStringFrom(char character, std::intmax_t startPos = 0, bool fromLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringFrom(const char* string, std::intmax_t startPos = 0, bool fromLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringFrom(const char* string, std::size_t length, std::intmax_t startPos = 0, bool fromLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringFrom(const String& string, std::intmax_t startPos = 0, bool fromLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringTo(char character, std::intmax_t startPos = 0, bool toLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringTo(const char* string, std::intmax_t startPos = 0, bool toLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringTo(const char* string, std::size_t length, std::intmax_t startPos = 0, bool toLast = false, bool include = false, UInt32 flags = None) const;
			String SubStringTo(const String& string, std::intmax_t startPos = 0, bool toLast = false, bool include = false, UInt32 flags = None) const;

			void Swap(String& str);

			bool ToBool(bool* value, UInt32 flags = None) const;
			bool ToDouble(double* value) const;
			bool ToInteger(long long* value, UInt8 radix = 10) const;
			String ToLower(UInt32 flags = None) const;
			String ToUpper(UInt32 flags = None) const;

			String& Trim(UInt32 flags = None);
			String& Trim(char character, UInt32 flags = None);
			String Trimmed(UInt32 flags = None) const;
			String Trimmed(char character, UInt32 flags = None) const;

			// Méthodes STD
			char* begin();
			const char* begin() const;
			char* end();
			const char* end() const;
			void push_front(char c);
			void push_back(char c);
			//char* rbegin();
			//const char* rbegin() const;
			//char* rend();
			//const char* rend() const;

			typedef const char& const_reference;
			typedef char* iterator;
			//typedef char* reverse_iterator;
			typedef char value_type;
			// Méthodes STD

			operator std::string() const;

			char& operator[](std::size_t pos);
			char operator[](std::size_t pos) const;

			String& operator=(char character);
			String& operator=(const char* string);
			String& operator=(const std::string& string);
			String& operator=(const String& string);
			String& operator=(String&& string) noexcept;

			String operator+(char character) const;
			String operator+(const char* string) const;
			String operator+(const std::string& string) const;
			String operator+(const String& string) const;

			String& operator+=(char character);
			String& operator+=(const char* string);
			String& operator+=(const std::string& string);
			String& operator+=(const String& string);

			bool operator==(char character) const;
			bool operator==(const char* string) const;
			bool operator==(const std::string& string) const;

			bool operator!=(char character) const;
			bool operator!=(const char* string) const;
			bool operator!=(const std::string& string) const;

			bool operator<(char character) const;
			bool operator<(const char* string) const;
			bool operator<(const std::string& string) const;

			bool operator<=(char character) const;
			bool operator<=(const char* string) const;
			bool operator<=(const std::string& string) const;

			bool operator>(char character) const;
			bool operator>(const char* string) const;
			bool operator>(const std::string& string) const;

			bool operator>=(char character) const;
			bool operator>=(const char* string) const;
			bool operator>=(const std::string& string) const;

			static String Boolean(bool boolean);
			static int Compare(const String& first, const String& second);
			static inline String Format(const char* format, ...);
			static String FormatVA(const char* format, va_list arg);
			static String Number(float number);
			static String Number(double number);
			static String Number(long double number);
			static String Number(signed char number, UInt8 radix = 10);
			static String Number(unsigned char number, UInt8 radix = 10);
			static String Number(short number, UInt8 radix = 10);
			static String Number(unsigned short number, UInt8 radix = 10);
			static String Number(int number, UInt8 radix = 10);
			static String Number(unsigned int number, UInt8 radix = 10);
			static String Number(long number, UInt8 radix = 10);
			static String Number(unsigned long number, UInt8 radix = 10);
			static String Number(long long number, UInt8 radix = 10);
			static String Number(unsigned long long number, UInt8 radix = 10);
			static String Pointer(const void* ptr);
			static String Unicode(char32_t character);
			static String Unicode(const char* u8String);
			static String Unicode(const char16_t* u16String);
			static String Unicode(const char32_t* u32String);
			static String Unicode(const wchar_t* wString);

			NAZARA_CORE_API friend std::istream& operator>>(std::istream& in, String& string);
			NAZARA_CORE_API friend std::ostream& operator<<(std::ostream& out, const String& string);

			NAZARA_CORE_API friend String operator+(char character, const String& string);
			NAZARA_CORE_API friend String operator+(const char* string, const String& nstring);
			NAZARA_CORE_API friend String operator+(const std::string& string, const String& nstring);

			NAZARA_CORE_API friend bool operator==(const String& first, const String& second);
			NAZARA_CORE_API friend bool operator!=(const String& first, const String& second);
			NAZARA_CORE_API friend bool operator<(const String& first, const String& second);
			NAZARA_CORE_API friend bool operator<=(const String& first, const String& second);
			NAZARA_CORE_API friend bool operator>(const String& first, const String& second);
			NAZARA_CORE_API friend bool operator>=(const String& first, const String& second);

			NAZARA_CORE_API friend bool operator==(char character, const String& nstring);
			NAZARA_CORE_API friend bool operator==(const char* string, const String& nstring);
			NAZARA_CORE_API friend bool operator==(const std::string& string, const String& nstring);

			NAZARA_CORE_API friend bool operator!=(char character, const String& nstring);
			NAZARA_CORE_API friend bool operator!=(const char* string, const String& nstring);
			NAZARA_CORE_API friend bool operator!=(const std::string& string, const String& nstring);

			NAZARA_CORE_API friend bool operator<(char character, const String& nstring);
			NAZARA_CORE_API friend bool operator<(const char* string, const String& nstring);
			NAZARA_CORE_API friend bool operator<(const std::string& string, const String& nstring);

			NAZARA_CORE_API friend bool operator<=(char character, const String& nstring);
			NAZARA_CORE_API friend bool operator<=(const char* string, const String& nstring);
			NAZARA_CORE_API friend bool operator<=(const std::string& string, const String& nstring);

			NAZARA_CORE_API friend bool operator>(char character, const String& nstring);
			NAZARA_CORE_API friend bool operator>(const char* string, const String& nstring);
			NAZARA_CORE_API friend bool operator>(const std::string& string, const String& nstring);

			NAZARA_CORE_API friend bool operator>=(char character, const String& nstring);
			NAZARA_CORE_API friend bool operator>=(const char* string, const String& nstring);
			NAZARA_CORE_API friend bool operator>=(const std::string& string, const String& nstring);

			static const std::size_t npos;

		private:
			struct SharedString;

			String(std::shared_ptr<SharedString>&& sharedString);

			void EnsureOwnership(bool discardContent = false);
			inline void ReleaseString();

			static const std::shared_ptr<SharedString>& GetEmptyString();

			std::shared_ptr<SharedString> m_sharedString;

			struct SharedString
			{
				inline SharedString();
				inline SharedString(std::size_t strSize);
				inline SharedString(std::size_t strSize, std::size_t strCapacity);

				std::size_t capacity;
				std::size_t size;
				std::unique_ptr<char[]> string;
			};
	};

	class AbstractHash;

	inline bool HashAppend(AbstractHash* hash, const String& string);
	NAZARA_CORE_API bool Serialize(SerializationContext& context, const String& string);
	NAZARA_CORE_API bool Unserialize(SerializationContext& context, String* string);
}

namespace std
{
	NAZARA_CORE_API istream& getline(istream& is, Nz::String& str);
	NAZARA_CORE_API istream& getline(istream& is, Nz::String& str, char delim);
	NAZARA_CORE_API void swap(Nz::String& lhs, Nz::String& rhs);

	template<>
	struct hash<Nz::String>;
}

#include <Nazara/Core/String.inl>

#endif // NAZARA_STRING_HPP
