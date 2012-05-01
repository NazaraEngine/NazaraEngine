/*#include <Nazara/Core/ByteArray.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

NzByteArray::NzByteArray() :
m_sharedArray(&emptyArray)
{
}

NzByteArray::NzByteArray(const nzUInt8* buffer, unsigned int bufferLength)
{
	if (bufferLength > 0)
	{
		m_sharedArray = new SharedArray;
		m_sharedArray->allocatedSize = bufferLength;
		m_sharedArray->buffer = new nzUInt8[bufferLength];
		m_sharedArray->size = bufferLength;
		std::memcpy(m_sharedArray->buffer, buffer, bufferLength*sizeof(nzUInt8));
	}
	else
		m_sharedArray = &emptyArray;
}

NzByteArray::NzByteArray(const NzByteArray& buffer) :
m_sharedArray(buffer.m_sharedArray)
{
	if (m_sharedArray != &emptyArray)
	{
		NazaraMutexLock(m_sharedArray->mutex);
		m_sharedArray->refCount++;
		NazaraMutexUnlock(m_sharedArray->mutex);
	}
}

NzByteArray::NzByteArray(NzByteArray&& buffer) :
m_sharedArray(buffer.m_sharedArray)
{
	buffer.m_sharedArray = &emptyArray;
}

NzByteArray::NzByteArray(SharedArray* sharedArray) :
m_sharedArray(sharedArray)
{
}

NzByteArray::~NzByteArray()
{
	ReleaseArray();
}

unsigned int NzByteArray::Capacity() const
{
	return m_sharedArray->allocatedSize;
}

void NzByteArray::Clear()
{
	ReleaseArray();
}

const nzUInt8* NzByteArray::GetBuffer() const
{
	return m_sharedArray->buffer;
}

unsigned int NzByteArray::GetSize() const
{
	return m_sharedArray->size;
}

NzByteArray& NzByteArray::Insert(int pos, const nzUInt8* buffer, unsigned int bufferLength)
{
	if (bufferLength == 0)
		return *this;

	if (m_sharedArray->size == 0)
		return operator=(string);

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedArray->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedArray->size);

	// Si le buffer est déjà suffisamment grand
	if (m_sharedArray->allocatedSize >= m_sharedArray->size + bufferLength)
	{
		EnsureOwnership();

		std::memmove(&m_sharedArray->buffer[start+bufferLength], &m_sharedArray->buffer[start], m_sharedArray->size*sizeof(nzUInt8));
		std::memcpy(&m_sharedArray->buffer[start], buffer, bufferLength*sizeof(nzUInt8));

		m_sharedArray->size += bufferLength;
	}
	else
	{
		unsigned int newSize = m_sharedArray->size+bufferLength;
		nzUInt8* newBuffer = new nzUInt8[newSize+1];

		nzUInt8* ptr = newBuffer;
		const nzUInt8* s = m_sharedArray->buffer;

		while (ptr != &newBuffer[start])
			*ptr++ = *s++;

		while (ptr != &newBuffer[start+bufferLength])
			*ptr++ = *buffer++;

		std::strcpy(ptr, s);

		ReleaseString();
		m_sharedArray = new SharedString;
		m_sharedArray->allocatedSize = newSize;
		m_sharedArray->buffer = newBuffer;
		m_sharedArray->size = newSize;
	}

	return *this;
}

NzByteArray& NzByteArray::Insert(int pos, const NzByteArray& byteArray);

bool NzByteArray::IsEmpty() const
{
	return m_sharedArray->size == 0;
}

void NzByteArray::Reserve(unsigned int bufferSize)
{
	if (m_sharedArray->allocatedSize >= bufferSize)
		return;

	char* ptr = new char[bufferSize+1];
	if (m_sharedArray->size > 0)
		std::strcpy(ptr, m_sharedArray->buffer);

	unsigned int size = m_sharedArray->size;

	ReleaseArray();
	m_sharedArray = new SharedString;
	m_sharedArray->allocatedSize = bufferSize;
	m_sharedArray->buffer = ptr;
	m_sharedArray->size = size;
}

NzByteArray& NzByteArray::Resize(int size, nzUInt8 byte = '\0');
NzByteArray NzByteArray::Resized(int size, nzUInt8 byte = '\0') const;

		NzByteArray SubArray(int startPos, int endPos = -1) const;

		void Swap(NzByteArray& byteArray);

		NzByteArray& Trim(nzUInt8 byte = '\0');
		NzByteArray Trimmed(nzUInt8 byte = '\0') const;

		// Méthodes compatibles STD
		nzUInt8* begin();
		const nzUInt8* begin() const;
		nzUInt8* end();
		const nzUInt8* end() const;
		void push_front(nzUInt8 c);
		void push_back(nzUInt8 c);

		typedef const nzUInt8& const_reference;
		typedef nzUInt8* iterator;
		//typedef nzUInt8* reverse_iterator;
		typedef nzUInt8 value_type;
		// Méthodes compatibles STD

		nzUInt8& operator[](unsigned int pos);
		nzUInt8 operator[](unsigned int pos) const;

		NzByteArray& operator=(const NzByteArray& byteArray);
		NzByteArray& operator=(NzByteArray&& byteArray);

		NzByteArray operator+(const NzByteArray& byteArray) const;
		NzByteArray& operator+=(const NzByteArray& byteArray);

		static int Compare(const NzByteArray& first, const NzByteArray& second);

		static SharedArray emptyArray;

	private:
		void EnsureOwnership();
		bool FillHash(NzHashImpl* hash) const;
		void ReleaseArray();
*/
