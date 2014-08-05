/*
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL VMWARE AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdbool.h>
#include <stdint.h>

#define WIDTH 1
#define HEIGHT 1

typedef union
{
   float f;
   int32_t i;
   uint32_t u;
   //portability: works for small endian only :|
   struct
   {
      uint32_t significand:23;
      uint32_t exponent:8;
      uint32_t sign:1;
   } fields;

} float_u;

typedef enum
{
   ONE_PARAM = 1,
   TWO_PARAM,
   THREE_PARAM
} func_type_e;

typedef struct
{
   char *name;
   func_type_e func_type;
   union
   {
      float (*func_one_param) (float);
      float (*func_two_param) (float, float);
      float (*func_three_param) (float, float, float);
   };
   unsigned int ulps;
} precision_func_t;


int setup_unclamped_output(void);
float *load_and_alloc_params(const char *name, unsigned int *num_params);
bool precision_test_fv(const unsigned int prog, const char *uniformname,
                       const float_u * params, const unsigned int nmemb,
                       const precision_func_t * pf);
