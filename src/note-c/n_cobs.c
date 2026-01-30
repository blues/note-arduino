// Copyright 2023 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdint.h>
#include <string.h>

#define COBS_EOP_OVERHEAD 1
#define COBS_MAX_PACKET_SIZE 254

//**************************************************************************/
/*!
  @brief Decode a string encoded with COBS encoding

  @details Because the decoded length is guaranteed to be less than or equal to
  length, the decode may be done in-place. Default behavior (with eop == 0) is
  to restore all '\0' into output data, but if a different value is specified,
  the input is XOR'ed such that THAT byte is the one that is assumed can't be
  in the input.

  @param  ptr Pointer to the data to decode
  @param  length Length of the data to decode
  @param  eop Byte to use as the end-of-packet marker
  @param  dst Pointer to the buffer for the decoded data

  @return the length of the decoded data

  @note OPTIMIZED: Uses bulk copy operations (memmove for in-place safety)
        instead of byte-by-byte processing, significantly reducing CPU cycles.
 */
/**************************************************************************/
uint32_t _cobsDecode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst)
{
    const uint8_t *start = dst;
    const uint8_t *end = ptr + length;
    uint8_t code = 0xFF;  // Special initial value: 0xFF means "first iteration, don't insert zero"

    while (ptr < end) {
        // Insert a zero byte UNLESS this is the first iteration (code == 0xFF)
        // COBS encoding removes zeros; decoding restores them between blocks
        if (code != 0xFF) {
            *dst++ = 0;
        }

        // Read next code byte (indicates how many data bytes follow)
        code = (*ptr++) ^ eop;

        // code == 0 is the termination marker
        if (code == 0) {
            break;
        }

        // Code byte meanings:
        //   code=1: 0 data bytes follow (zero was here, already inserted above)
        //   code=2: 1 data byte follows
        //   code=0xFF: 254 data bytes follow (full block, no zero after)
        // Therefore: bytesToCopy = code - 1
        uint32_t bytesToCopy = code - 1;

        // Safety: don't read past end of input
        if (ptr + bytesToCopy > end) {
            bytesToCopy = (uint32_t)(end - ptr);
        }

        // OPTIMIZATION: Bulk copy with XOR applied
        if (eop == 0) {
            // Fast path: no XOR needed
            // CRITICAL: Use memmove() not memcpy() because in-place decoding is supported.
            // When dst == original ptr, we're shifting data left (removing code bytes),
            // which creates overlapping source and destination regions.
            // memmove() correctly handles overlapping regions; memcpy() behavior is undefined.
            memmove(dst, ptr, bytesToCopy);
        } else {
            // XOR path: tight array-indexed loop enables SIMD/NEON auto-vectorization.
            // Safe for in-place decode because dst <= ptr (we're removing code bytes),
            // so reads are always ahead of writes when processing left-to-right.
            for (uint32_t i = 0; i < bytesToCopy; i++) {
                dst[i] = ptr[i] ^ eop;
            }
        }

        // Advance pointers
        dst += bytesToCopy;
        ptr += bytesToCopy;
    }

    return (uint32_t)(dst - start);
}

//**************************************************************************/
/*!
  @brief  Encode a string with Consistent Overhead Byte Stuffing (COBS) encoding

  @details behavior (with eop == 0) is to eliminate all '\0' from input data,
  but if a different value is specified, the output is XOR'ed such that THAT
  byte is the one that won't be contained in output.
  https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing

  @param  ptr Pointer to the data to encode
  @param  length Length of the data to encode
  @param  eop Byte to use as the end-of-packet marker
  @param  dst Pointer to the buffer for the encoded data

  @return the length of the encoded data

  @note You may use `_cobsEncodedLength()` to calculate the required size for
        the buffer pointed to by the `dst` parameter.
  @note OPTIMIZED: Uses memchr() to skip to next zero byte and bulk copy
        operations instead of byte-by-byte processing, significantly reducing
        CPU cycles.

  @see _cobsEncodedLength()
 */
/**************************************************************************/
uint32_t _cobsEncode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst)
{
    uint8_t *start = dst;
    uint8_t code = 1;
    uint8_t *code_ptr = dst++;          // Reserve first byte for code

    while (length > 0) {
        // COBS encoding constraint: code byte ranges from 1 to 0xFF (255).
        // code=1 means 0 data bytes follow (next byte is another code or end)
        // code=2 means 1 data byte follows, ..., code=0xFF means 254 data bytes follow.
        // Therefore, maximum bytes we can encode before needing a new code byte is 254.
        uint32_t maxBytes = 0xFF - code;  // 254 when code=1, decreases as code increases

        // Limit search to available data and code byte capacity
        uint32_t searchLen = (length < maxBytes) ? length : maxBytes;

        // OPTIMIZATION: Use memchr() to find next zero byte in O(n) hardware-accelerated search
        // instead of checking each byte individually. This is typically 10-50x faster for
        // finding zeros in long runs of non-zero data.
        uint8_t *zeroPos = (uint8_t *)memchr(ptr, 0, searchLen);

        // Calculate chunk length: bytes to process before hitting zero or code limit
        uint32_t chunkLen;
        if (zeroPos != NULL) {
            // Found zero: process all bytes UP TO (not including) the zero
            chunkLen = (uint32_t)(zeroPos - ptr);
        } else {
            // No zero found: process all searchLen bytes
            chunkLen = searchLen;
        }

        // Bulk copy using memmove() for overlap safety, then XOR in-place if needed
        memmove(dst, ptr, chunkLen);
        if (eop != 0) {
            for (uint32_t i = 0; i < chunkLen; i++) {
                dst[i] ^= eop;
            }
        }

        // Update pointers and remaining length
        dst += chunkLen;
        ptr += chunkLen;
        length -= chunkLen;
        code += chunkLen;

        // Determine why we stopped: hit a zero byte or reached code limit (0xFF)
        if (zeroPos != NULL && length > 0) {
            // Hit a zero byte: write current code, start new block
            *code_ptr = code ^ eop;
            code = 1;
            code_ptr = dst++;
            ptr++;          // Skip over the zero byte in input
            length--;
        } else if (code == 0xFF) {
            // Hit code limit (254 data bytes): write 0xFF code, start new block
            *code_ptr = code ^ eop;
            code = 1;
            code_ptr = dst++;
        }
    }

    // Write final code byte for the last block
    *code_ptr = code ^ eop;

    return (uint32_t)(dst - start);
}

//**************************************************************************/
/*!
  @brief  Compute the encoding length of unencoded data

  @param  ptr Pointer to the data to encode
  @param  length Length of the data to encode

  @return the length required for encoded data

  @note  The computed length does not include the EOP (end-of-packet) marker
  @note  OPTIMIZED: Uses memchr() to skip to zeros instead of checking every
         byte, matching the optimization strategy in _cobsEncode().
 */
/**************************************************************************/
uint32_t _cobsEncodedLength(const uint8_t *ptr, uint32_t length)
{
    uint32_t encodedLen = 1;  // Start with 1 for the first code byte
    uint8_t code = 1;

    while (length > 0) {
        // Calculate max bytes we can process before hitting code limit (0xFF)
        uint32_t maxBytes = 0xFF - code;
        uint32_t searchLen = (length < maxBytes) ? length : maxBytes;

        // OPTIMIZATION: Use memchr() to find next zero instead of byte-by-byte scan
        const uint8_t *zeroPos = (const uint8_t *)memchr(ptr, 0, searchLen);

        uint32_t chunkLen;
        if (zeroPos != NULL) {
            // Found zero: count bytes up to (not including) the zero
            chunkLen = (uint32_t)(zeroPos - ptr);
        } else {
            // No zero found: count all searchLen bytes
            chunkLen = searchLen;
        }

        // Add data bytes to output length
        encodedLen += chunkLen;
        ptr += chunkLen;
        length -= chunkLen;
        code += chunkLen;

        // Check if we need a new code byte
        if (zeroPos != NULL && length > 0) {
            // Hit a zero: will need a new code byte for next block
            encodedLen++;
            code = 1;
            ptr++;          // Skip the zero
            length--;
        } else if (code == 0xFF) {
            // Hit code limit: will need a new code byte for next block
            encodedLen++;
            code = 1;
        }
    }

    return encodedLen;
}

//**************************************************************************/
/*!
  @brief  Compute the max encoding length for a given length of unencoded data

  @param  length Length of the data to encode

  @return The max length required to encode the data

  @note  Since the contents of the buffer are unknown, then we must assume
          that the entire buffer has no end-of-packet markers. This would
          require the injection of overhead bytes (as opposed to the
          replacement of end-of-packet markers with overhead bytes) at
          intervals of 255, thus producing the worst case scenario.
  @note  An additional byte is added for the EOP (end-of-packet) marker.
 */
/**************************************************************************/
uint32_t _cobsEncodedMaxLength(uint32_t length)
{
    const uint32_t overheadBytes = (length == 0) + ((length != 0) * ((length / COBS_MAX_PACKET_SIZE) + ((length % COBS_MAX_PACKET_SIZE) > 0)));
    return (length + overheadBytes + COBS_EOP_OVERHEAD);
}

//**************************************************************************/
/*!
  @brief Compute the maximum length of unencoded data that can fit into a
  buffer of specified length.

  @param  bufLen Length of the buffer in bytes

  @return the length of unencoded data

  @note  An additional byte for the EOP (end-of-packet) marker is assumed.
 */
/**************************************************************************/
uint32_t _cobsGuaranteedFit(uint32_t bufLen)
{
    // encodedLen = unencodedLen + codeBytesLen
    // e = u + c (e = encoded (sorry Euler), and c = code bytes (sorry Einstein))
    // u = e - c
    // c = ⌈u / 254⌉ (the ceiling of u divided by 254)
    //
    // Rearranging the ceiling equation:
    // (c - 1) < u / 254 <= c
    // 254(c - 1) < u <= 254c
    //
    // Substitute u from first equation:
    // 254(c - 1) < e - c <= 254c
    // 254c - 254 < e - c <= 254c
    // 255c < e + 254 AND e <= 255c
    //
    // Thus:
    // e <= 255c < e + 254
    // e / 255 <= c < (e + 254) / 255
    //
    // Knowing that c is an integer, we can express c as:
    // c = ⌊(e + 254) / 255⌋ (the floor of (e + 254) divided by 255)
    //
    // Substitute c back into the original equation for u:
    // u = e - ⌊(e + 254) / 255⌋
    const uint32_t encodedLen = (bufLen == 0) + ((bufLen != 0) * (bufLen - COBS_EOP_OVERHEAD));
    return (encodedLen - ((encodedLen + COBS_MAX_PACKET_SIZE) / 255));
}
