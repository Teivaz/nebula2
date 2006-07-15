/* mhash_sha512_sha384.h
 *
 * The sha512 and sha384 hash functions.
 * Copyright (C) 2004 B. Poettering
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

 
#ifndef MHASH_SHA512_SHA384_H_INCLUDED
#define MHASH_SHA512_SHA384_H_INCLUDED

/* SHA512 and SHA384 */

#define SHA512_DIGEST_SIZE 64
#define SHA384_DIGEST_SIZE 48
#define SHA512_SHA384_DATA_SIZE 128

/* State is kept internally as 8 64-bit words. */
#define _SHA512_SHA384_STATE_LENGTH 8

typedef struct sha512_sha384_ctx
{
  word64 state[_SHA512_SHA384_STATE_LENGTH];  /* State variables */
  word64 bitcount_low, bitcount_high;         /* Bit counter */
  byte block[SHA512_SHA384_DATA_SIZE];        /* SHA512/384 data buffer */
  unsigned int index;                         /* index into buffer */
} SHA512_SHA384_CTX;

void
sha512_init(struct sha512_sha384_ctx *ctx);

void
sha384_init(struct sha512_sha384_ctx *ctx);

void
sha512_sha384_update(struct sha512_sha384_ctx *ctx, const byte *data, 
              unsigned length);

void
sha512_sha384_final(struct sha512_sha384_ctx *ctx);

void
sha512_digest(const struct sha512_sha384_ctx *ctx, byte *digest);

void
sha384_digest(const struct sha512_sha384_ctx *ctx, byte *digest);


#endif /* MHASH_SHA512_SHA384_H_INCLUDED */
