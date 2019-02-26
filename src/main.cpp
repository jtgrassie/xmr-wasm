// Copyright (c) 2018, Jethro Grassie
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <emscripten/emscripten.h>
#include "hash.h"
#include "main.h"

struct result_t {
  crypto::hash hash;
  uint32_t nonce;
} result;

EMSCRIPTEN_KEEPALIVE
void allocate_state()
{
  slow_hash_allocate_state();
}

EMSCRIPTEN_KEEPALIVE
void free_state()
{
  slow_hash_free_state();
}

EMSCRIPTEN_KEEPALIVE
uint8_t* hash(uint8_t* block_data, const size_t length, const uint32_t target_low, const uint32_t target_high, const uint32_t height)
{
  memset(&result, 0, sizeof(struct result_t));
  uint64_t target = (uint64_t) target_high << 32 | target_low;
  if(target_high == 0)
    target = 0xFFFFFFFFFFFFFFFFULL / (0xFFFFFFFFULL / ((uint64_t)target));

  uint64_t* p_result = (uint64_t*) (result.hash.data + 24);
  uint32_t* p_nonce = (uint32_t*) (block_data + 39);
  result.nonce = *p_nonce;
  const uint8_t cn_variant = block_data[0] >= 7 ? block_data[0] - 6 : 0;
  while(true)
  {
    crypto::cn_slow_hash(block_data, length, result.hash, cn_variant, height);
    if(*p_result < target)
    {
      break;
    }
    *p_nonce = ++result.nonce;
  }
  return (uint8_t*) &result;
}

int main(int argc, char ** argv)
{
  printf("WASM loaded\n");
}

