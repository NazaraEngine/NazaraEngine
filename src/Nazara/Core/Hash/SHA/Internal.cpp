// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

/*
 * FILE:	sha2.c
 * AUTHOR:	Aaron D. Gifford
 *		http://www.aarongifford.com/computers/sha.html
 *
 * Copyright (c) 2000-2003, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: sha2.c,v 1.4 2004/01/07 22:58:18 adg Exp $
 */

#include <Nazara/Core/Hash/SHA/Internal.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{

	/*** ENDIAN REVERSAL MACROS *******************************************/
	#ifdef NAZARA_LITTLE_ENDIAN

	#define REVERSE32(w,x)	{ \
		UInt32 tmp = (w); \
		tmp = (tmp >> 16) | (tmp << 16); \
		(x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
	}

	#define REVERSE64(w,x)	{ \
		UInt64 tmp = (w); \
		tmp = (tmp >> 32) | (tmp << 32); \
		tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) | \
			  ((tmp & 0x00ff00ff00ff00ffULL) << 8); \
		(x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
			  ((tmp & 0x0000ffff0000ffffULL) << 16); \
	}
	#endif

	/*
	 * Macro for incrementally adding the unsigned 64-bit integer n to the
	 * unsigned 128-bit integer (represented using a two-element array of
	 * 64-bit words):
	 */
	#define ADDINC128(w,n)	{ \
		(w)[0] += static_cast<UInt64>(n); \
		if ((w)[0] < (n)) { \
			(w)[1]++; \
		} \
	}


	/*** THE SIX LOGICAL FUNCTIONS ****************************************/
	/*
	 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
	 *
	 *   NOTE:  In the original SHA-256/384/512 document, the shift-right
	 *   function was named R and the rotate-right function was called S.
	 *   (See: http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf on the
	 *   web.)
	 *
	 *   The newer NIST FIPS 180-2 document uses a much clearer naming
	 *   scheme, SHR for shift-right, ROTR for rotate-right, and ROTL for
	 *   rotate-left.  (See:
	 *   http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
	 *   on the web.)
	 *
	 *   WARNING: These macros must be used cautiously, since they reference
	 *   supplied parameters sometimes more than once, and thus could have
	 *   unexpected side-effects if used without taking this into account.
	 */
	/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
	#define SHR(b,x) 		((x) >> (b))
	/* 32-bit Rotate-right (used in SHA-256): */
	#define ROTR32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
	/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
	#define ROTR64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))
	/* 32-bit Rotate-left (used in SHA-1): */
	#define ROTL32(b,x)	(((x) << (b)) | ((x) >> (32 - (b))))

	/* Two logical functions used in SHA-1, SHA-254, SHA-256, SHA-384, and SHA-512: */
	#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
	#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

	/* Function used in SHA-1: */
	#define Parity(x,y,z)	((x) ^ (y) ^ (z))

	/* Four logical functions used in SHA-256: */
	#define Sigma0_256(x)	(ROTR32(2,  (x)) ^ ROTR32(13, (x)) ^ ROTR32(22, (x)))
	#define Sigma1_256(x)	(ROTR32(6,  (x)) ^ ROTR32(11, (x)) ^ ROTR32(25, (x)))
	#define sigma0_256(x)	(ROTR32(7,  (x)) ^ ROTR32(18, (x)) ^ SHR(   3 , (x)))
	#define sigma1_256(x)	(ROTR32(17, (x)) ^ ROTR32(19, (x)) ^ SHR(   10, (x)))

	/* Four of six logical functions used in SHA-384 and SHA-512: */
	#define Sigma0_512(x)	(ROTR64(28, (x)) ^ ROTR64(34, (x)) ^ ROTR64(39, (x)))
	#define Sigma1_512(x)	(ROTR64(14, (x)) ^ ROTR64(18, (x)) ^ ROTR64(41, (x)))
	#define sigma0_512(x)	(ROTR64( 1, (x)) ^ ROTR64( 8, (x)) ^ SHR(    7, (x)))
	#define sigma1_512(x)	(ROTR64(19, (x)) ^ ROTR64(61, (x)) ^ SHR(    6, (x)))

	/*** INTERNAL FUNCTION PROTOTYPES *************************************/

	/* SHA-224 and SHA-256: */
	void SHA256_Internal_Init(SHA_CTX*, const UInt32*);
	void SHA256_Internal_Last(SHA_CTX*);
	void SHA256_Internal_Transform(SHA_CTX*, const UInt32*);

	/* SHA-384 and SHA-512: */
	void SHA512_Internal_Init(SHA_CTX*, const UInt64*);
	void SHA512_Internal_Last(SHA_CTX*);
	void SHA512_Internal_Transform(SHA_CTX*, const UInt64*);


	/*** SHA2 INITIAL HASH VALUES AND CONSTANTS ***************************/

	/* Hash constant words K for SHA-1: */
	#define K1_0_TO_19	0x5a827999UL
	#define K1_20_TO_39	0x6ed9eba1UL
	#define K1_40_TO_59	0x8f1bbcdcUL
	#define K1_60_TO_79	0xca62c1d6UL

	/* Initial hash value H for SHA-1: */
	const static UInt32 sha1_initial_hash_value[5] = {
		0x67452301UL,
		0xefcdab89UL,
		0x98badcfeUL,
		0x10325476UL,
		0xc3d2e1f0UL
	};

	/* Hash constant words K for SHA-224 and SHA-256: */
	const static UInt32 K256[64] = {
		0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
		0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
		0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
		0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
		0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
		0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
		0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
		0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
		0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
		0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
		0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
		0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
		0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
		0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
		0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
		0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
	};

	/* Initial hash value H for SHA-224: */
	const static UInt32 sha224_initial_hash_value[8] = {
		0xc1059ed8UL,
		0x367cd507UL,
		0x3070dd17UL,
		0xf70e5939UL,
		0xffc00b31UL,
		0x68581511UL,
		0x64f98fa7UL,
		0xbefa4fa4UL
	};

	/* Initial hash value H for SHA-256: */
	const static UInt32 sha256_initial_hash_value[8] = {
		0x6a09e667UL,
		0xbb67ae85UL,
		0x3c6ef372UL,
		0xa54ff53aUL,
		0x510e527fUL,
		0x9b05688cUL,
		0x1f83d9abUL,
		0x5be0cd19UL
	};

	/* Hash constant words K for SHA-384 and SHA-512: */
	const static UInt64 K512[80] = {
		0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
		0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
		0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
		0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
		0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
		0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
		0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
		0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
		0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
		0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
		0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
		0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
		0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
		0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
		0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
		0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
		0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
		0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
		0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
		0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
		0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
		0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
		0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
		0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
		0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
		0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
		0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
		0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
		0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
		0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
		0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
		0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
		0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
		0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
		0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
		0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
		0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
		0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
		0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
		0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
	};

	/* Initial hash value H for SHA-384 */
	const static UInt64 sha384_initial_hash_value[8] = {
		0xcbbb9d5dc1059ed8ULL,
		0x629a292a367cd507ULL,
		0x9159015a3070dd17ULL,
		0x152fecd8f70e5939ULL,
		0x67332667ffc00b31ULL,
		0x8eb44a8768581511ULL,
		0xdb0c2e0d64f98fa7ULL,
		0x47b5481dbefa4fa4ULL
	};

	/* Initial hash value H for SHA-512 */
	const static UInt64 sha512_initial_hash_value[8] = {
		0x6a09e667f3bcc908ULL,
		0xbb67ae8584caa73bULL,
		0x3c6ef372fe94f82bULL,
		0xa54ff53a5f1d36f1ULL,
		0x510e527fade682d1ULL,
		0x9b05688c2b3e6c1fULL,
		0x1f83d9abfb41bd6bULL,
		0x5be0cd19137e2179ULL
	};


	/*** SHA-1: ***********************************************************/
	void SHA1_Init(SHA_CTX* context)
	{
		/* Sanity check: */
		std::memcpy(context->s1.state, sha1_initial_hash_value, sizeof(UInt32) * 5);
		std::memset(context->s1.buffer, 0, 64);
		context->s1.bitcount = 0;
	}

	#ifdef NAZARA_LITTLE_ENDIAN

	#define ROUND1_0_TO_15(a,b,c,d,e)				\
		REVERSE32(*data++, W1[j]);				\
		(e) = ROTL32(5, (a)) + Ch((b), (c), (d)) + (e) +	\
			 K1_0_TO_19 + W1[j];	\
		(b) = ROTL32(30, (b));		\
		j++;

	#else // NAZARA_LITTLE_ENDIAN

	#define ROUND1_0_TO_15(a,b,c,d,e)				\
		(e) = ROTL32(5, (a)) + Ch((b), (c), (d)) + (e) +	\
			 K1_0_TO_19 + ( W1[j] = *data++ );		\
		(b) = ROTL32(30, (b));	\
		j++;

	#endif // NAZARA_LITTLE_ENDIAN

	#define ROUND1_16_TO_19(a,b,c,d,e)	\
		T1 = W1[(j+13)&0x0f] ^ W1[(j+8)&0x0f] ^ W1[(j+2)&0x0f] ^ W1[j&0x0f];	\
		(e) = ROTL32(5, a) + Ch(b,c,d) + e + K1_0_TO_19 + ( W1[j&0x0f] = ROTL32(1, T1) );	\
		(b) = ROTL32(30, b);	\
		j++;

	#define ROUND1_20_TO_39(a,b,c,d,e)	\
		T1 = W1[(j+13)&0x0f] ^ W1[(j+8)&0x0f] ^ W1[(j+2)&0x0f] ^ W1[j&0x0f];	\
		(e) = ROTL32(5, a) + Parity(b,c,d) + e + K1_20_TO_39 + ( W1[j&0x0f] = ROTL32(1, T1) );	\
		(b) = ROTL32(30, b);	\
		j++;

	#define ROUND1_40_TO_59(a,b,c,d,e)	\
		T1 = W1[(j+13)&0x0f] ^ W1[(j+8)&0x0f] ^ W1[(j+2)&0x0f] ^ W1[j&0x0f];	\
		(e) = ROTL32(5, a) + Maj(b,c,d) + e + K1_40_TO_59 + ( W1[j&0x0f] = ROTL32(1, T1) );	\
		(b) = ROTL32(30, b);	\
		j++;

	#define ROUND1_60_TO_79(a,b,c,d,e)	\
		T1 = W1[(j+13)&0x0f] ^ W1[(j+8)&0x0f] ^ W1[(j+2)&0x0f] ^ W1[j&0x0f];	\
		(e) = ROTL32(5, a) + Parity(b,c,d) + e + K1_60_TO_79 + ( W1[j&0x0f] = ROTL32(1, T1) );	\
		(b) = ROTL32(30, b);	\
		j++;

	namespace
	{
		void SHA1_Internal_Transform(SHA_CTX* context, const UInt32* data)
		{
			UInt32 a, b, c, d, e;
			UInt32 T1, *W1;
			int	j;

			W1 = reinterpret_cast<UInt32*>(context->s1.buffer);

			/* Initialize registers with the prev. intermediate value */
			a = context->s1.state[0];
			b = context->s1.state[1];
			c = context->s1.state[2];
			d = context->s1.state[3];
			e = context->s1.state[4];

			j = 0;

			/* Rounds 0 to 15 unrolled: */
			ROUND1_0_TO_15(a,b,c,d,e);
			ROUND1_0_TO_15(e,a,b,c,d);
			ROUND1_0_TO_15(d,e,a,b,c);
			ROUND1_0_TO_15(c,d,e,a,b);
			ROUND1_0_TO_15(b,c,d,e,a);
			ROUND1_0_TO_15(a,b,c,d,e);
			ROUND1_0_TO_15(e,a,b,c,d);
			ROUND1_0_TO_15(d,e,a,b,c);
			ROUND1_0_TO_15(c,d,e,a,b);
			ROUND1_0_TO_15(b,c,d,e,a);
			ROUND1_0_TO_15(a,b,c,d,e);
			ROUND1_0_TO_15(e,a,b,c,d);
			ROUND1_0_TO_15(d,e,a,b,c);
			ROUND1_0_TO_15(c,d,e,a,b);
			ROUND1_0_TO_15(b,c,d,e,a);
			ROUND1_0_TO_15(a,b,c,d,e);

			/* Rounds 16 to 19 unrolled: */
			ROUND1_16_TO_19(e,a,b,c,d);
			ROUND1_16_TO_19(d,e,a,b,c);
			ROUND1_16_TO_19(c,d,e,a,b);
			ROUND1_16_TO_19(b,c,d,e,a);

			/* Rounds 20 to 39 unrolled: */
			ROUND1_20_TO_39(a,b,c,d,e);
			ROUND1_20_TO_39(e,a,b,c,d);
			ROUND1_20_TO_39(d,e,a,b,c);
			ROUND1_20_TO_39(c,d,e,a,b);
			ROUND1_20_TO_39(b,c,d,e,a);
			ROUND1_20_TO_39(a,b,c,d,e);
			ROUND1_20_TO_39(e,a,b,c,d);
			ROUND1_20_TO_39(d,e,a,b,c);
			ROUND1_20_TO_39(c,d,e,a,b);
			ROUND1_20_TO_39(b,c,d,e,a);
			ROUND1_20_TO_39(a,b,c,d,e);
			ROUND1_20_TO_39(e,a,b,c,d);
			ROUND1_20_TO_39(d,e,a,b,c);
			ROUND1_20_TO_39(c,d,e,a,b);
			ROUND1_20_TO_39(b,c,d,e,a);
			ROUND1_20_TO_39(a,b,c,d,e);
			ROUND1_20_TO_39(e,a,b,c,d);
			ROUND1_20_TO_39(d,e,a,b,c);
			ROUND1_20_TO_39(c,d,e,a,b);
			ROUND1_20_TO_39(b,c,d,e,a);

			/* Rounds 40 to 59 unrolled: */
			ROUND1_40_TO_59(a,b,c,d,e);
			ROUND1_40_TO_59(e,a,b,c,d);
			ROUND1_40_TO_59(d,e,a,b,c);
			ROUND1_40_TO_59(c,d,e,a,b);
			ROUND1_40_TO_59(b,c,d,e,a);
			ROUND1_40_TO_59(a,b,c,d,e);
			ROUND1_40_TO_59(e,a,b,c,d);
			ROUND1_40_TO_59(d,e,a,b,c);
			ROUND1_40_TO_59(c,d,e,a,b);
			ROUND1_40_TO_59(b,c,d,e,a);
			ROUND1_40_TO_59(a,b,c,d,e);
			ROUND1_40_TO_59(e,a,b,c,d);
			ROUND1_40_TO_59(d,e,a,b,c);
			ROUND1_40_TO_59(c,d,e,a,b);
			ROUND1_40_TO_59(b,c,d,e,a);
			ROUND1_40_TO_59(a,b,c,d,e);
			ROUND1_40_TO_59(e,a,b,c,d);
			ROUND1_40_TO_59(d,e,a,b,c);
			ROUND1_40_TO_59(c,d,e,a,b);
			ROUND1_40_TO_59(b,c,d,e,a);

			/* Rounds 60 to 79 unrolled: */
			ROUND1_60_TO_79(a,b,c,d,e);
			ROUND1_60_TO_79(e,a,b,c,d);
			ROUND1_60_TO_79(d,e,a,b,c);
			ROUND1_60_TO_79(c,d,e,a,b);
			ROUND1_60_TO_79(b,c,d,e,a);
			ROUND1_60_TO_79(a,b,c,d,e);
			ROUND1_60_TO_79(e,a,b,c,d);
			ROUND1_60_TO_79(d,e,a,b,c);
			ROUND1_60_TO_79(c,d,e,a,b);
			ROUND1_60_TO_79(b,c,d,e,a);
			ROUND1_60_TO_79(a,b,c,d,e);
			ROUND1_60_TO_79(e,a,b,c,d);
			ROUND1_60_TO_79(d,e,a,b,c);
			ROUND1_60_TO_79(c,d,e,a,b);
			ROUND1_60_TO_79(b,c,d,e,a);
			ROUND1_60_TO_79(a,b,c,d,e);
			ROUND1_60_TO_79(e,a,b,c,d);
			ROUND1_60_TO_79(d,e,a,b,c);
			ROUND1_60_TO_79(c,d,e,a,b);
			ROUND1_60_TO_79(b,c,d,e,a);

			/* Compute the current intermediate hash value */
			context->s1.state[0] += a;
			context->s1.state[1] += b;
			context->s1.state[2] += c;
			context->s1.state[3] += d;
			context->s1.state[4] += e;
		}
	}

	void SHA1_Update(SHA_CTX* context, const UInt8* data, std::size_t len)
	{
		if (len == 0)
			/* Calling with no data is valid - we do nothing */
			return;

		unsigned int usedspace = (context->s1.bitcount >> 3) % 64;
		if (usedspace > 0)
		{
			/* Calculate how much free space is available in the buffer */
			unsigned int freespace = 64 - usedspace;

			if (len >= freespace)
			{
				/* Fill the buffer completely and process it */
				std::memcpy(&context->s1.buffer[usedspace], data, freespace);
				context->s1.bitcount += freespace << 3;
				len -= freespace;
				data += freespace;
				SHA1_Internal_Transform(context, reinterpret_cast<UInt32*>(context->s1.buffer));
			}
			else
			{
				/* The buffer is not yet full */
				std::memcpy(&context->s1.buffer[usedspace], data, len);
				context->s1.bitcount += len << 3;
				return;
			}
		}

		while (len >= 64)
		{
			/* Process as many complete blocks as we can */
			SHA1_Internal_Transform(context, reinterpret_cast<const UInt32*>(data));
			context->s1.bitcount += 512;
			len -= 64;
			data += 64;
		}

		if (len > 0)
		{
			/* There's left-overs, so save 'em */
			std::memcpy(context->s1.buffer, data, len);
			context->s1.bitcount += len << 3;
		}
	}

	void SHA1_End(SHA_CTX* context, UInt8* digest)
	{
		UInt32* d = reinterpret_cast<UInt32*>(digest);
		unsigned int usedspace = (context->s1.bitcount >> 3) % 64;

		if (usedspace == 0)
		{
			/* Set-up for the last transform: */
			std::memset(context->s1.buffer, 0, 56);

			/* Begin padding with a 1 bit: */
			*context->s1.buffer = 0x80;
		}
		else
		{
			/* Begin padding with a 1 bit: */
			context->s1.buffer[usedspace++] = 0x80;

			if (usedspace <= 56)
				/* Set-up for the last transform: */
				std::memset(&context->s1.buffer[usedspace], 0, 56 - usedspace);
			else
			{
				if (usedspace < 64)
					std::memset(&context->s1.buffer[usedspace], 0, 64 - usedspace);

				/* Do second-to-last transform: */
				SHA1_Internal_Transform(context, reinterpret_cast<UInt32*>(context->s1.buffer));

				/* And set-up for the last transform: */
				std::memset(context->s1.buffer, 0, 56);
			}
		}
		/* Set the bit count: */
	#ifdef NAZARA_LITTLE_ENDIAN
		/* Convert FROM host byte order */
		REVERSE64(context->s1.bitcount,context->s1.bitcount);
	#endif
		UInt64* length = reinterpret_cast<UInt64*>(&context->s1.buffer[56]);
		*length = context->s1.bitcount;

		/* Final transform: */
		SHA1_Internal_Transform(context, reinterpret_cast<UInt32*>(context->s1.buffer));

		/* Save the hash data for output: */
	#ifdef NAZARA_LITTLE_ENDIAN
		{
			/* Convert TO host byte order */
			for (int j = 0; j < (SHA1_DIGEST_LENGTH >> 2); j++)
			{
				REVERSE32(context->s1.state[j],context->s1.state[j]);
				*d++ = context->s1.state[j];
			}
		}
	#else
		std::memcpy(d, context->s1.state, SHA1_DIGEST_LENGTH);
	#endif
	}


	/*** SHA-256: *********************************************************/
	void SHA256_Internal_Init(SHA_CTX* context, const UInt32* ihv)
	{
		std::memcpy(context->s256.state, ihv, sizeof(UInt32) * 8);
		std::memset(context->s256.buffer, 0, 64);
		context->s256.bitcount = 0;
	}

	void SHA256_Init(SHA_CTX* context)
	{
		SHA256_Internal_Init(context, sha256_initial_hash_value);
	}

	#ifdef NAZARA_LITTLE_ENDIAN

	#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h)	\
		REVERSE32(*data++, W256[j]); \
		T1 = (h) + Sigma1_256(e) + Ch((e), (f), (g)) + \
				 K256[j] + W256[j]; \
		(d) += T1; \
		(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
		j++

	#else // NAZARA_LITTLE_ENDIAN

	#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h)	\
		T1 = (h) + Sigma1_256(e) + Ch((e), (f), (g)) + \
			 K256[j] + (W256[j] = *data++); \
		(d) += T1; \
		(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
		j++

	#endif // NAZARA_LITTLE_ENDIAN

	#define ROUND256(a,b,c,d,e,f,g,h)	\
		s0 = W256[(j+1)&0x0f]; \
		s0 = sigma0_256(s0); \
		s1 = W256[(j+14)&0x0f]; \
		s1 = sigma1_256(s1); \
		T1 = (h) + Sigma1_256(e) + Ch((e), (f), (g)) + K256[j] + \
			 (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0); \
		(d) += T1; \
		(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
		j++

	void SHA256_Internal_Transform(SHA_CTX* context, const UInt32* data)
	{
		UInt32 a, b, c, d, e, f, g, h;
		UInt32 T1, *W256;
		int	j;

		W256 = reinterpret_cast<UInt32*>(context->s256.buffer);

		/* Initialize registers with the prev. intermediate value */
		a = context->s256.state[0];
		b = context->s256.state[1];
		c = context->s256.state[2];
		d = context->s256.state[3];
		e = context->s256.state[4];
		f = context->s256.state[5];
		g = context->s256.state[6];
		h = context->s256.state[7];

		j = 0;
		do
		{
			/* Rounds 0 to 15 (unrolled): */
			ROUND256_0_TO_15(a,b,c,d,e,f,g,h);
			ROUND256_0_TO_15(h,a,b,c,d,e,f,g);
			ROUND256_0_TO_15(g,h,a,b,c,d,e,f);
			ROUND256_0_TO_15(f,g,h,a,b,c,d,e);
			ROUND256_0_TO_15(e,f,g,h,a,b,c,d);
			ROUND256_0_TO_15(d,e,f,g,h,a,b,c);
			ROUND256_0_TO_15(c,d,e,f,g,h,a,b);
			ROUND256_0_TO_15(b,c,d,e,f,g,h,a);
		}
		while (j < 16);

		/* Now for the remaining rounds to 64: */
		do
		{
			UInt32 s0, s1;

			ROUND256(a,b,c,d,e,f,g,h);
			ROUND256(h,a,b,c,d,e,f,g);
			ROUND256(g,h,a,b,c,d,e,f);
			ROUND256(f,g,h,a,b,c,d,e);
			ROUND256(e,f,g,h,a,b,c,d);
			ROUND256(d,e,f,g,h,a,b,c);
			ROUND256(c,d,e,f,g,h,a,b);
			ROUND256(b,c,d,e,f,g,h,a);
		}
		while (j < 64);

		/* Compute the current intermediate hash value */
		context->s256.state[0] += a;
		context->s256.state[1] += b;
		context->s256.state[2] += c;
		context->s256.state[3] += d;
		context->s256.state[4] += e;
		context->s256.state[5] += f;
		context->s256.state[6] += g;
		context->s256.state[7] += h;
	}

	void SHA256_Update(SHA_CTX* context, const UInt8 *data, std::size_t len)
	{
		if (len == 0)
			/* Calling with no data is valid - we do nothing */
			return;

		unsigned int usedspace = (context->s256.bitcount >> 3) % 64;
		if (usedspace > 0)
		{
			/* Calculate how much free space is available in the buffer */
			unsigned int freespace = 64 - usedspace;

			if (len >= freespace)
			{
				/* Fill the buffer completely and process it */
				std::memcpy(&context->s256.buffer[usedspace], data, freespace);
				context->s256.bitcount += freespace << 3;
				len -= freespace;
				data += freespace;
				SHA256_Internal_Transform(context, reinterpret_cast<UInt32*>(context->s256.buffer));
			}
			else
			{
				/* The buffer is not yet full */
				std::memcpy(&context->s256.buffer[usedspace], data, len);
				context->s256.bitcount += len << 3;
				return;
			}
		}

		while (len >= 64)
		{
			/* Process as many complete blocks as we can */
			SHA256_Internal_Transform(context, reinterpret_cast<const UInt32*>(data));
			context->s256.bitcount += 512;
			len -= 64;
			data += 64;
		}

		if (len > 0)
		{
			/* There's left-overs, so save 'em */
			std::memcpy(context->s256.buffer, data, len);
			context->s256.bitcount += len << 3;
		}
	}

	void SHA256_Internal_Last(SHA_CTX* context)
	{
		unsigned int usedspace = (context->s256.bitcount >> 3) % 64;
	#ifdef NAZARA_LITTLE_ENDIAN
		/* Convert FROM host byte order */
		REVERSE64(context->s256.bitcount,context->s256.bitcount);
	#endif
		if (usedspace > 0)
		{
			/* Begin padding with a 1 bit: */
			context->s256.buffer[usedspace++] = 0x80;

			if (usedspace <= 56)
				/* Set-up for the last transform: */
				std::memset(&context->s256.buffer[usedspace], 0, 56 - usedspace);
			else
			{
				if (usedspace < 64)
					std::memset(&context->s256.buffer[usedspace], 0, 64 - usedspace);

				/* Do second-to-last transform: */
				SHA256_Internal_Transform(context, reinterpret_cast<UInt32*>(context->s256.buffer));

				/* And set-up for the last transform: */
				std::memset(context->s256.buffer, 0, 56);
			}
		}
		else
		{
			/* Set-up for the last transform: */
			std::memset(context->s256.buffer, 0, 56);

			/* Begin padding with a 1 bit: */
			*context->s256.buffer = 0x80;
		}

		/* Set the bit count: */
		UInt64* length = reinterpret_cast<UInt64*>(&context->s256.buffer[56]);
		*length = context->s256.bitcount;

		/* Final transform: */
		SHA256_Internal_Transform(context, reinterpret_cast<UInt32*>(context->s256.buffer));
	}

	void SHA256_End(SHA_CTX* context, UInt8* digest)
	{
		UInt32* d = reinterpret_cast<UInt32*>(digest);

		/* If no digest buffer is passed, we don't bother doing this: */
		if (digest)
		{
			SHA256_Internal_Last(context);

			/* Save the hash data for output: */
	#ifdef NAZARA_LITTLE_ENDIAN
			{
				/* Convert TO host byte order */
				for (int j = 0; j < (SHA256_DIGEST_LENGTH >> 2); j++)
				{
					REVERSE32(context->s256.state[j],context->s256.state[j]);
					*d++ = context->s256.state[j];
				}
			}
	#else
			std::memcpy(d, context->s256.state, SHA256_DIGEST_LENGTH);
	#endif
		}
	}


	/*** SHA-224: *********************************************************/
	void SHA224_Init(SHA_CTX* context)
	{
		SHA256_Internal_Init(context, sha224_initial_hash_value);
	}

	void SHA224_Update(SHA_CTX* context, const UInt8 *data, std::size_t len)
	{
		SHA256_Update(context, data, len);
	}

	void SHA224_End(SHA_CTX* context, UInt8* digest)
	{
		UInt32* d = reinterpret_cast<UInt32*>(digest);

		/* If no digest buffer is passed, we don't bother doing this: */
		if (digest)
		{
			SHA256_Internal_Last(context);

			/* Save the hash data for output: */
	#ifdef NAZARA_LITTLE_ENDIAN
			{
				/* Convert TO host byte order */
				for (int j = 0; j < (SHA224_DIGEST_LENGTH >> 2); j++)
				{
					REVERSE32(context->s256.state[j],context->s256.state[j]);
					*d++ = context->s256.state[j];
				}
			}
	#else
			std::memcpy(d, context->s256.state, SHA224_DIGEST_LENGTH);
	#endif
		}
	}


	/*** SHA-512: *********************************************************/
	void SHA512_Internal_Init(SHA_CTX* context, const UInt64* ihv)
	{
		std::memcpy(context->s512.state, ihv, sizeof(UInt64) * 8);
		std::memset(context->s512.buffer, 0, 128);
		context->s512.bitcount[0] = context->s512.bitcount[1] =  0;
	}

	void SHA512_Init(SHA_CTX* context)
	{
		SHA512_Internal_Init(context, sha512_initial_hash_value);
	}

	/* Unrolled SHA-512 round macros: */
	#ifdef NAZARA_LITTLE_ENDIAN

	#define ROUND512_0_TO_15(a,b,c,d,e,f,g,h)	\
		REVERSE64(*data++, W512[j]); \
		T1 = (h) + Sigma1_512(e) + Ch((e), (f), (g)) + \
				 K512[j] + W512[j]; \
		(d) += T1, \
		(h) = T1 + Sigma0_512(a) + Maj((a), (b), (c)), \
		j++


	#else // NAZARA_LITTLE_ENDIAN

	#define ROUND512_0_TO_15(a,b,c,d,e,f,g,h)	\
		T1 = (h) + Sigma1_512(e) + Ch((e), (f), (g)) + \
				 K512[j] + (W512[j] = *data++); \
		(d) += T1; \
		(h) = T1 + Sigma0_512(a) + Maj((a), (b), (c)); \
		j++

	#endif // NAZARA_LITTLE_ENDIAN

	#define ROUND512(a,b,c,d,e,f,g,h)	\
		s0 = W512[(j+1)&0x0f]; \
		s0 = sigma0_512(s0); \
		s1 = W512[(j+14)&0x0f]; \
		s1 = sigma1_512(s1); \
		T1 = (h) + Sigma1_512(e) + Ch((e), (f), (g)) + K512[j] + \
				 (W512[j&0x0f] += s1 + W512[(j+9)&0x0f] + s0); \
		(d) += T1; \
		(h) = T1 + Sigma0_512(a) + Maj((a), (b), (c)); \
		j++

	void SHA512_Internal_Transform(SHA_CTX* context, const UInt64* data)
	{
		UInt64 a, b, c, d, e, f, g, h, s0, s1;
		UInt64 T1, *W512 = reinterpret_cast<UInt64*>(context->s512.buffer);
		int	j;

		/* Initialize registers with the prev. intermediate value */
		a = context->s512.state[0];
		b = context->s512.state[1];
		c = context->s512.state[2];
		d = context->s512.state[3];
		e = context->s512.state[4];
		f = context->s512.state[5];
		g = context->s512.state[6];
		h = context->s512.state[7];

		j = 0;
		do
		{
			ROUND512_0_TO_15(a,b,c,d,e,f,g,h);
			ROUND512_0_TO_15(h,a,b,c,d,e,f,g);
			ROUND512_0_TO_15(g,h,a,b,c,d,e,f);
			ROUND512_0_TO_15(f,g,h,a,b,c,d,e);
			ROUND512_0_TO_15(e,f,g,h,a,b,c,d);
			ROUND512_0_TO_15(d,e,f,g,h,a,b,c);
			ROUND512_0_TO_15(c,d,e,f,g,h,a,b);
			ROUND512_0_TO_15(b,c,d,e,f,g,h,a);
		}
		while (j < 16);

		/* Now for the remaining rounds up to 79: */
		do
		{
			ROUND512(a,b,c,d,e,f,g,h);
			ROUND512(h,a,b,c,d,e,f,g);
			ROUND512(g,h,a,b,c,d,e,f);
			ROUND512(f,g,h,a,b,c,d,e);
			ROUND512(e,f,g,h,a,b,c,d);
			ROUND512(d,e,f,g,h,a,b,c);
			ROUND512(c,d,e,f,g,h,a,b);
			ROUND512(b,c,d,e,f,g,h,a);
		}
		while (j < 80);

		/* Compute the current intermediate hash value */
		context->s512.state[0] += a;
		context->s512.state[1] += b;
		context->s512.state[2] += c;
		context->s512.state[3] += d;
		context->s512.state[4] += e;
		context->s512.state[5] += f;
		context->s512.state[6] += g;
		context->s512.state[7] += h;
	}

	void SHA512_Update(SHA_CTX* context, const UInt8 *data, std::size_t len)
	{
		if (len == 0)
			/* Calling with no data is valid - we do nothing */
			return;

		unsigned int usedspace = (context->s512.bitcount[0] >> 3) % 128;
		if (usedspace > 0)
		{
			/* Calculate how much free space is available in the buffer */
			unsigned int freespace = 128 - usedspace;

			if (len >= freespace)
			{
				/* Fill the buffer completely and process it */
				std::memcpy(&context->s512.buffer[usedspace], data, freespace);
				ADDINC128(context->s512.bitcount, freespace << 3);
				len -= freespace;
				data += freespace;
				SHA512_Internal_Transform(context, reinterpret_cast<UInt64*>(context->s512.buffer));
			}
			else
			{
				/* The buffer is not yet full */
				std::memcpy(&context->s512.buffer[usedspace], data, len);
				ADDINC128(context->s512.bitcount, len << 3);
				return;
			}
		}

		while (len >= 128)
		{
			/* Process as many complete blocks as we can */
			SHA512_Internal_Transform(context, reinterpret_cast<const UInt64*>(data));
			ADDINC128(context->s512.bitcount, 1024);
			len -= 128;
			data += 128;
		}

		if (len > 0)
		{
			/* There's left-overs, so save 'em */
			std::memcpy(context->s512.buffer, data, len);
			ADDINC128(context->s512.bitcount, len << 3);
		}
	}

	void SHA512_Internal_Last(SHA_CTX* context)
	{
		unsigned int usedspace;

		usedspace = (context->s512.bitcount[0] >> 3) % 128;
	#ifdef NAZARA_LITTLE_ENDIAN
		/* Convert FROM host byte order */
		REVERSE64(context->s512.bitcount[0],context->s512.bitcount[0]);
		REVERSE64(context->s512.bitcount[1],context->s512.bitcount[1]);
	#endif
		if (usedspace > 0)
		{
			/* Begin padding with a 1 bit: */
			context->s512.buffer[usedspace++] = 0x80;

			if (usedspace <= 112)
				/* Set-up for the last transform: */
				std::memset(&context->s512.buffer[usedspace], 0, 112 - usedspace);
			else
			{
				if (usedspace < 128)
					std::memset(&context->s512.buffer[usedspace], 0, 128 - usedspace);

				/* Do second-to-last transform: */
				SHA512_Internal_Transform(context, reinterpret_cast<UInt64*>(context->s512.buffer));

				/* And set-up for the last transform: */
				std::memset(context->s512.buffer, 0, 112);
			}
		}
		else
		{
			/* Prepare for final transform: */
			std::memset(context->s512.buffer, 0, 112);

			/* Begin padding with a 1 bit: */
			*context->s512.buffer = 0x80;
		}

		/* Store the length of input data (in bits): */
		UInt64* length = reinterpret_cast<UInt64*>(&context->s512.buffer[112]);
		length[0] = context->s512.bitcount[1];
		length[1] = context->s512.bitcount[0];

		/* Final transform: */
		SHA512_Internal_Transform(context, reinterpret_cast<UInt64*>(context->s512.buffer));
	}

	void SHA512_End(SHA_CTX* context, UInt8* digest)
	{
		UInt64* d = reinterpret_cast<UInt64*>(digest);

		/* If no digest buffer is passed, we don't bother doing this: */
		if (digest)
		{
			SHA512_Internal_Last(context);

			/* Save the hash data for output: */
	#ifdef NAZARA_LITTLE_ENDIAN
			{
				/* Convert TO host byte order */
				for (int j = 0; j < (SHA512_DIGEST_LENGTH >> 3); j++)
				{
					REVERSE64(context->s512.state[j],context->s512.state[j]);
					*d++ = context->s512.state[j];
				}
			}
	#else
			std::memcpy(d, context->s512.state, SHA512_DIGEST_LENGTH);
	#endif
		}
	}


	/*** SHA-384: *********************************************************/
	void SHA384_Init(SHA_CTX* context)
	{
		SHA512_Internal_Init(context, sha384_initial_hash_value);
	}

	void SHA384_Update(SHA_CTX* context, const UInt8* data, std::size_t len)
	{
		SHA512_Update(context, data, len);
	}

	void SHA384_End(SHA_CTX* context, UInt8* digest)
	{
		UInt64* d = reinterpret_cast<UInt64*>(digest);

		/* If no digest buffer is passed, we don't bother doing this: */
		if (digest)
		{
			SHA512_Internal_Last(context);

			/* Save the hash data for output: */
	#ifdef NAZARA_LITTLE_ENDIAN
			{
				/* Convert TO host byte order */
				for (int j = 0; j < (SHA384_DIGEST_LENGTH >> 3); j++)
				{
					REVERSE64(context->s512.state[j],context->s512.state[j]);
					*d++ = context->s512.state[j];
				}
			}
	#else
			std::memcpy(d, context->s512.state, SHA384_DIGEST_LENGTH);
	#endif
		}
	}
}
