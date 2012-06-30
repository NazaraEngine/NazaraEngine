// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STRING_HPP
#define NAZARA_STRING_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <iosfwd>
#include <string>
#include <vector>

#define NAZARA_CLASS_STRING
#include <Nazara/Core/ThreadSafety.hpp>

class NzAbstractHash;
class NzHashDigest;

class NAZARA_API NzString : public NzHashable
{
	public:
		enum Flags
		{
			None            = 0x00,	// Mode par défaut
			CaseInsensitive = 0x01,	// Insensible à la casse
			HandleUtf8      = 0x02,	// Traite les octets comme une suite de caractères UTF-8
			TrimOnlyLeft    = 0x04, // Trim(med), ne coupe que la partie gauche de la chaîne
			TrimOnlyRight   = 0x08  // Trim(med), ne coupe que la partie droite de la chaîne
		};

		struct SharedString;

		NzString();
		NzString(char character);
		NzString(const char* string);
		NzString(const std::string& string);
		NzString(const NzString& string);
		NzString(NzString&& string);
		NzString(SharedString* sharedString);
		~NzString();

		NzString& Append(char character);
		NzString& Append(const char* string);
		NzString& Append(const NzString& string);

		void Clear(bool keepBuffer = false);

		bool Contains(char character, int start = 0, nzUInt32 flags = None) const;
		bool Contains(const char* string, int start = 0, nzUInt32 flags = None) const;
		bool Contains(const NzString& string, int start = 0, nzUInt32 flags = None) const;

		unsigned int Count(char character, int start = 0, nzUInt32 flags = None) const;
		unsigned int Count(const char* string, int start = 0, nzUInt32 flags = None) const;
		unsigned int Count(const NzString& string, int start = 0, nzUInt32 flags = None) const;
		unsigned int CountAny(const char* string, int start = 0, nzUInt32 flags = None) const;
		unsigned int CountAny(const NzString& string, int start = 0, nzUInt32 flags = None) const;

		bool EndsWith(char character, nzUInt32 flags = None) const;
		bool EndsWith(const char* string, nzUInt32 flags = None) const;
		bool EndsWith(const NzString& string, nzUInt32 flags = None) const;

		unsigned int Find(char character, int start = 0, nzUInt32 flags = None) const;
		unsigned int Find(const char* string, int start = 0, nzUInt32 flags = None) const;
		unsigned int Find(const NzString& string, int start = 0, nzUInt32 flags = None) const;
		unsigned int FindAny(const char* string, int start = 0, nzUInt32 flags = None) const;
		unsigned int FindAny(const NzString& string, int start = 0, nzUInt32 flags = None) const;
		unsigned int FindLast(char character, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindLast(const char *string, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindLast(const NzString& string, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindLastAny(const char* string, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindLastAny(const NzString& string, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindLastWord(const char* string, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindLastWord(const NzString& string, int start = -1, nzUInt32 flags = None) const;
		unsigned int FindWord(const char* string, int start = 0, nzUInt32 flags = None) const;
		unsigned int FindWord(const NzString& string, int start = 0, nzUInt32 flags = None) const;

		char* GetBuffer();
		unsigned int GetCapacity() const;
		const char* GetConstBuffer() const;
		unsigned int GetLength() const;
		unsigned int GetSize() const;
		char* GetUtf8Buffer(unsigned int* size = nullptr) const;
		char16_t* GetUtf16Buffer(unsigned int* size = nullptr) const;
		char32_t* GetUtf32Buffer(unsigned int* size = nullptr) const;
		wchar_t* GetWideBuffer(unsigned int* size = nullptr) const;

		NzString GetWord(unsigned int index, nzUInt32 flags = None) const;
		unsigned int GetWordPosition(unsigned int index, nzUInt32 flags = None) const;

		NzString& Insert(int pos, char character);
		NzString& Insert(int pos, const char* string);
		NzString& Insert(int pos, const NzString& string);

		bool IsEmpty() const;
		bool IsNull() const;
		bool IsNumber(nzUInt8 radix = 10, nzUInt32 flags = CaseInsensitive) const;

		bool Match(const char* pattern) const;
		bool Match(const NzString& pattern) const;

		NzString& Prepend(char character);
		NzString& Prepend(const char* string);
		NzString& Prepend(const NzString& string);

		unsigned int Replace(char oldCharacter, char newCharacter, int start = 0, nzUInt32 flags = None);
		unsigned int Replace(const char* oldString, const char* replaceString, int start = 0, nzUInt32 flags = None);
		unsigned int Replace(const NzString& oldString, const NzString& replaceString, int start = 0, nzUInt32 flags = None);
		unsigned int ReplaceAny(const char* oldCharacters, char replaceCharacter, int start = 0, nzUInt32 flags = None);
		unsigned int ReplaceAny(const char* oldCharacters, const char* replaceString, int start = 0, nzUInt32 flags = None);
		unsigned int ReplaceAny(const NzString& oldCharacters, const NzString& replaceString, int start = 0, nzUInt32 flags = None);

		void Reserve(unsigned int bufferSize);

		NzString& Resize(int size, char character = ' ');
		NzString Resized(int size, char character = ' ') const;

		NzString& Reverse();
		NzString Reversed() const;

		NzString Simplified(nzUInt32 flags = None) const;
		NzString& Simplify(nzUInt32 flags = None);

		unsigned int Split(std::vector<NzString>& result, char separation = ' ', int start = 0, nzUInt32 flags = None) const;
		unsigned int Split(std::vector<NzString>& result, const char* separation, int start = 0, nzUInt32 flags = None) const;
		unsigned int Split(std::vector<NzString>& result, const NzString& separation, int start = 0, nzUInt32 flags = None) const;
		unsigned int SplitAny(std::vector<NzString>& result, const char* separations, int start = 0, nzUInt32 flags = None) const;
		unsigned int SplitAny(std::vector<NzString>& result, const NzString& separations, int start = 0, nzUInt32 flags = None) const;

		bool StartsWith(char character, nzUInt32 flags = None) const;
		bool StartsWith(const char* string, nzUInt32 flags = None) const;
		bool StartsWith(const NzString& string, nzUInt32 flags = None) const;

		NzString Substr(int startPos, int endPos = -1) const;
		NzString SubstrFrom(char character, int startPos = 0, bool fromLast = false, bool include = false, nzUInt32 flags = None) const;
		NzString SubstrFrom(const char *string, int startPos = 0, bool fromLast = false, bool include = false, nzUInt32 flags = None) const;
		NzString SubstrFrom(const NzString& string, int startPos = 0, bool fromLast = false, bool include = false, nzUInt32 flags = None) const;
		NzString SubstrTo(char character, int startPos = 0, bool toLast = false, bool include = false, nzUInt32 flags = None) const;
		NzString SubstrTo(const char *string, int startPos = 0, bool toLast = false, bool include = false, nzUInt32 flags = None) const;
		NzString SubstrTo(const NzString& string, int startPos = 0, bool toLast = false, bool include = false, nzUInt32 flags = None) const;

		void Swap(NzString& str);

		bool ToBool(bool* value, nzUInt32 flags = None) const;
		bool ToDouble(double* value) const;
		bool ToInteger(long long* value, nzUInt8 radix = 10) const;
		NzString ToLower(nzUInt32 flags = None) const;
		NzString ToUpper(nzUInt32 flags = None) const;

		NzString& Trim(nzUInt32 flags = None);
		NzString& Trim(char character, nzUInt32 flags = None);
		NzString Trimmed(nzUInt32 flags = None) const;
		NzString Trimmed(char character, nzUInt32 flags = None) const;

		// Méthodes compatibles STD
		char* begin();
		const char* begin() const;
		char* end();
		const char* end() const;
		void push_front(char c);
		void push_back(char c);
		/*char* rbegin();
		const char* rbegin() const;
		char* rend();
		const char* rend() const;*/

		typedef const char& const_reference;
		typedef char* iterator;
		//typedef char* reverse_iterator;
		typedef char value_type;
		// Méthodes compatibles STD

		operator std::string() const;

		char& operator[](unsigned int pos);
		char operator[](unsigned int pos) const;

		NzString& operator=(char character);
		NzString& operator=(const char* string);
		NzString& operator=(const std::string& string);
		NzString& operator=(const NzString& string);
		NzString& operator=(NzString&& string);

		NzString operator+(char character) const;
		NzString operator+(const char* string) const;
		NzString operator+(const std::string& string) const;
		NzString operator+(const NzString& string) const;

		NzString& operator+=(char character);
		NzString& operator+=(const char* string);
		NzString& operator+=(const std::string& string);
		NzString& operator+=(const NzString& string);

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

		static NzString Boolean(bool boolean);
		static int Compare(const NzString& first, const NzString& second);
		static NzString Number(float number);
		static NzString Number(double number);
		static NzString Number(long double number);
		static NzString Number(signed char number, nzUInt8 radix = 10);
		static NzString Number(unsigned char number, nzUInt8 radix = 10);
		static NzString Number(short number, nzUInt8 radix = 10);
		static NzString Number(unsigned short number, nzUInt8 radix = 10);
		static NzString Number(int number, nzUInt8 radix = 10);
		static NzString Number(unsigned int number, nzUInt8 radix = 10);
		static NzString Number(long number, nzUInt8 radix = 10);
		static NzString Number(unsigned long number, nzUInt8 radix = 10);
		static NzString Number(long long number, nzUInt8 radix = 10);
		static NzString Number(unsigned long long number, nzUInt8 radix = 10);
		static NzString Pointer(const void* ptr);
		static NzString Unicode(char32_t character);
		static NzString Unicode(const char* u8String);
		static NzString Unicode(const char16_t* u16String);
		static NzString Unicode(const char32_t* u32String);
		static NzString Unicode(const wchar_t* wString);

		NAZARA_API friend std::istream& operator>>(std::istream& in, NzString& string);
		NAZARA_API friend std::ostream& operator<<(std::ostream& out, const NzString& string);

		NAZARA_API friend NzString operator+(char character, const NzString& string);
		NAZARA_API friend NzString operator+(const char* string, const NzString& nstring);
		NAZARA_API friend NzString operator+(const std::string& string, const NzString& nstring);

		NAZARA_API friend bool operator==(const NzString& first, const NzString& second);
		NAZARA_API friend bool operator!=(const NzString& first, const NzString& second);
		NAZARA_API friend bool operator<(const NzString& first, const NzString& second);
		NAZARA_API friend bool operator<=(const NzString& first, const NzString& second);
		NAZARA_API friend bool operator>(const NzString& first, const NzString& second);
		NAZARA_API friend bool operator>=(const NzString& first, const NzString& second);

		NAZARA_API friend bool operator==(char character, const NzString& nstring);
		NAZARA_API friend bool operator==(const char* string, const NzString& nstring);
		NAZARA_API friend bool operator==(const std::string& string, const NzString& nstring);

		NAZARA_API friend bool operator!=(char character, const NzString& nstring);
		NAZARA_API friend bool operator!=(const char* string, const NzString& nstring);
		NAZARA_API friend bool operator!=(const std::string& string, const NzString& nstring);

		NAZARA_API friend bool operator<(char character, const NzString& nstring);
		NAZARA_API friend bool operator<(const char* string, const NzString& nstring);
		NAZARA_API friend bool operator<(const std::string& string, const NzString& nstring);

		NAZARA_API friend bool operator<=(char character, const NzString& nstring);
		NAZARA_API friend bool operator<=(const char* string, const NzString& nstring);
		NAZARA_API friend bool operator<=(const std::string& string, const NzString& nstring);

		NAZARA_API friend bool operator>(char character, const NzString& nstring);
		NAZARA_API friend bool operator>(const char* string, const NzString& nstring);
		NAZARA_API friend bool operator>(const std::string& string, const NzString& nstring);

		NAZARA_API friend bool operator>=(char character, const NzString& nstring);
		NAZARA_API friend bool operator>=(const char* string, const NzString& nstring);
		NAZARA_API friend bool operator>=(const std::string& string, const NzString& nstring);

		struct NAZARA_API SharedString
		{
			SharedString() :
			refCount(1)
			{
			}

			SharedString(unsigned short referenceCount, unsigned int bufferSize, unsigned int stringSize, char* str) :
			capacity(bufferSize),
			size(stringSize),
			string(str),
			refCount(referenceCount)
			{
			}

			unsigned int capacity;
			unsigned int size;
			char* string;

			unsigned short refCount;
			NazaraMutex(mutex)
		};

		static SharedString emptyString;
		static unsigned int npos;

	private:
		void EnsureOwnership();
		bool FillHash(NzHashImpl* hash) const;
		void ReleaseString();

		SharedString* m_sharedString;
};

namespace std
{
	NAZARA_API istream& getline(istream& is, NzString& str);
	NAZARA_API istream& getline(istream& is, NzString& str, char delim);
	NAZARA_API void swap(NzString& lhs, NzString& rhs);
}

#undef NAZARA_CLASS_STRING

#endif // NAZARA_STRING_HPP
