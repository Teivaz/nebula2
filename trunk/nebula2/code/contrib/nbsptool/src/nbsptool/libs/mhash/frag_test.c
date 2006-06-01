/* frag_test.c
 * 
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
 * 
 */


/* 
 * Most modern hash functions work in a block oriented way. In order
 * to fit the mhash interface (which accepts an arbitrary amount of
 * data at each call of the mhash function) the input data has to be
 * cut into equally sized chunks, which subsequently are processed one
 * by one.
 *
 * The following code checks all the hash functions included in the
 * mhash library for the proper implementation of the mentioned
 * "chunking algorithm". A data stream is generated, cut into pieces
 * and fed into two instances of mhash in parallel. As the cutting
 * points of the two input streams differ, the two calculated hash
 * values match only if the "chunking" algorithm is implemented
 * properly.
 */

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "nbsptool/libs/mhash/mhash.h"


#define MAX_DIGEST_SIZE 256
#define MAX_INPUT_SIZE 256


int frag_test(hashid hashid)
{
  unsigned char digest1[MAX_DIGEST_SIZE]; /* enough space to hold digests */
  unsigned char digest2[MAX_DIGEST_SIZE];
  unsigned char buf1[MAX_INPUT_SIZE + 1];
  unsigned char buf2[MAX_INPUT_SIZE];
  MHASH td1, td2;
  size_t input_size, digest_size;
  int i, offs, left;
  unsigned char val = 0;

  input_size = mhash_get_hash_pblock(hashid);
  assert(input_size <= MAX_INPUT_SIZE);

  digest_size = mhash_get_block_size(hashid);
  assert(digest_size <= MAX_DIGEST_SIZE);

  td1 = mhash_init(hashid);               /* get two mhash instances */
  td2 = mhash_init(hashid);

  for(i = offs = 0; i < 2 * input_size; i++, val++) /* first part */
    {
      memset(buf1, val, input_size + 1);  /* the first instance gets framgments */
      mhash(td1, buf1, input_size + 1);   /* of size (input_size+1)             */

      left = input_size - offs;           /* the second instance gets fragments */
      memset(buf2 + offs, val, left);     /* of size input_size                 */
      mhash(td2, buf2, input_size);

      offs = (input_size + 1) - left;
      memset(buf2, val, offs);
      if (offs == input_size)
    {
      mhash(td2, buf2, input_size);
      offs = 0;
    }
    }
  mhash(td2, buf2, offs);

  for(i = offs = 0; i < 2 * input_size; i++, val++) /* second part */
    {
      memset(buf1, val, input_size - 1);  /* the first instance gets framgments */
      mhash(td1, buf1, input_size - 1);   /* of size (input_size-1)             */
      
      if (offs == 0)                      /* the second instance gets fragments */
    {                                 /* of size input_size                 */
      offs = input_size - 1;
      memset(buf2, val, offs);
    }
      else
    {
      left = input_size - offs;
      memset(buf2 + offs, val, left);

      mhash(td2, buf2, input_size);
      offs = (input_size - 1) - left;
      memset(buf2, val, offs);
    }
    }
  mhash(td2, buf2, offs);

  mhash_deinit(td1, digest1);   /* return 1 if the calculated hash values match */
  mhash_deinit(td2, digest2);
  return ! strncmp(digest1, digest2, digest_size);
}

int main(void)
{
  hashid hashid;
  const char *s;
  int ok, allok = 1;

  for(hashid = 0; hashid <= mhash_count(); hashid++)
    if ((s = mhash_get_hash_name_static(hashid)) &&
    mhash_get_hash_pblock(hashid))
    {
      printf("Checking fragmentation capabilities of %s: ", s); 
      fflush(stdout);
      printf((ok = frag_test(hashid)) ? "OK\n" : "Failed\n");
      allok &= ok;
    }
  return allok ? 0 : 1;
}
