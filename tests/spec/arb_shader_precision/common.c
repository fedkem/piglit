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


#include "common.h"
#include "piglit-util-gl-common.h"


int
setup_unclamped_output(void)
{
   int ret = 0;
   GLuint fb;

   GLuint rb;


   //clear glGetError()
   glGetError();

   glGenFramebuffers(1, &fb);
   ret = glGetError();
   if (ret) {
      printf("glGenFramebuffer error: %x\n", ret);
      return ret;
   }

   glBindFramebuffer(GL_FRAMEBUFFER, fb);
   ret = glGetError();
   if (ret) {
      printf("glBindFramebuffer error: %x\n", ret);
      return ret;
   }

   glGenRenderbuffers( 1, &rb );
   ret = glGetError();
   if (ret) {
      printf("glGenRenderbuffers error: %x\n", ret);
      return ret;
   }

   glBindRenderbuffer( GL_RENDERBUFFER, rb );
   ret = glGetError();
   if (ret) {
      printf("glBindRenderbuffer error: %x\n", ret);
      return ret;
   }

   glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA32F, 2, 2 );
   ret = glGetError();
   if (ret) {
      printf("glRenderbufferStorage error: %x\n", ret);
      return ret;
   }

   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_RENDERBUFFER, rb);
   ret = glGetError();
   if (ret) {
      printf("glFramebufferRenderbuffer error: %x\n", ret);
      return ret;
   }

   return ret;
}

float *
load_and_alloc_params(const char *name, unsigned int *num_params)
{
   float *retarray = NULL;
   FILE *fp;
   char *filename;
   unsigned int filename_sz;
   char prefix[] = "tests/spec/arb_shader_precision/";
   char suffix[] = ".params";
   int scanfret = 0;
   unsigned int count = 0;
   float val;
   bool done = false;

   filename_sz = strlen(prefix) + strlen(name) + strlen(suffix) + 1;
   filename = (char *) malloc(filename_sz);
   memset(filename, 0, filename_sz);
   sprintf(filename, "%s%s%s", prefix, name, suffix);

   fp = fopen(filename, "r");

   if (fp) {
      //parse text into array of floats
      //sorry, not very robust
      //line must lead with a float val in "d.ddde+-dd" format, comments may trail
      do {
         scanfret = fscanf(fp, "%e", &val);
         if (scanfret != EOF) {
            retarray = realloc(retarray, (count + 1) * sizeof(float));
            retarray[count++] = val;
            fscanf(fp, "%*[^\n]%*c");   //comments and endline
         } else {
            done = true;
         }
      } while (!done);
      fclose(fp);
   } else {
      printf("could not open param file: \"%s\"! ", filename);
   }

   *num_params = count;

   free(filename);

   return retarray;
}

bool
precision_test_fv(const unsigned int prog, const char *uniformname,
                  const float_u * params, const unsigned int nmemb,
                  const precision_func_t * pf)
{
   float_u expected;
   float_u probed[4];
   int param_location;
   uint32_t vert_ulps_diff;
   uint32_t frag_ulps_diff;
   bool vert_sign_match;
   bool frag_sign_match;
   bool vert_result = GL_TRUE;
   bool frag_result = GL_TRUE;
   bool overall_result = GL_TRUE;
   int i;
   GLenum glerr;


   param_location = glGetUniformLocation(prog, uniformname);
   glerr = glGetError();
   if (glerr) {
      printf
         ("failed to get uniform location for uniform: \"%s\", test failed! \n",
          uniformname);
      return false;
   }

   //TODO set fenv defaults?

   for (i = 0; i < nmemb; i += pf->func_type) {
      //calculate result on cpu
      switch (pf->func_type) {
      case ONE_PARAM:
         expected = (float_u) pf->func_one_param(params[i].f);
         glUniform1f(param_location, params[i].f);
         glerr = glGetError();
         if (glerr) {
            printf
               ("failed to set uniform value: %1.8e, location: %d %x \n",
                params[i].f, param_location, glerr);
            return false;
         }
         printf("%s(%1.8e)(%x): ", pf->name, 
                params[i].f, params[i].u);
         break;
      case TWO_PARAM:
         expected =
            (float_u) pf->func_two_param(params[i].f, params[i + 1].f);
         glUniform2fv(param_location, 1, (GLfloat *) & params[i].f);
         glerr = glGetError();
         if (glerr) {
            printf
               ("failed to set uniform values: %1.8e %1.8e, %x \n",
                params[i].f, params[i + 1].f, glerr);
            return false;
         }
         printf("%s(%1.8e, %1.8e)(%x, %x): ", pf->name,
                params[i].f, params[i + 1].f, 
                params[i].u, params[i + 1].u);
         break;
      case THREE_PARAM:
         expected =
            (float_u) pf->func_three_param(params[i].f, params[i + 1].f,
                                           params[i + 2].f);
         glUniform3fv(param_location, 1, (GLfloat *) & params[i].f);
         glerr = glGetError();
         if (glerr) {
            printf
               ("failed to set uniform values: %1.8e %1.8e %1.8e, %x \n",
                params[i].f, params[i + 1].f, params[i + 2].f, glerr);
            return false;
         }
         printf("%s(%1.8e, %1.8e, %1.8e)(%x, %x, %x): ", pf->name,
                params[i].f, params[i + 1].f, params[i + 2].f, 
                params[i].u, params[i + 1].u, params[i + 2].u);
         break;
      default:
         printf("unrecognized func_type, test failed! ");
         return false;
      }

      glClear(GL_COLOR_BUFFER_BIT);
      piglit_draw_rect(0, 0, WIDTH, HEIGHT);

      glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, (float *) probed);

      if (probed[0].fields.sign != expected.fields.sign) {
         vert_sign_match = false;
         vert_result = GL_FALSE;
      } else {
         vert_sign_match = true;
         vert_ulps_diff = abs(probed[0].u - expected.u);
         vert_result = vert_ulps_diff <= pf->ulps;
      }

      if (probed[1].fields.sign != expected.fields.sign) {
         frag_sign_match = false;
         frag_result = GL_FALSE;
      } else {
         frag_sign_match = true;
         frag_ulps_diff = abs(probed[1].u - expected.u);
         frag_result = frag_ulps_diff <= pf->ulps;
      }

      printf("cpu: %1.8e (%x) "
             "vert: %1.8e (%x) sign match: %u ulp diff: %u(%u) passed: %d "
             "frag: %1.8e (%x) sign match: %u ulp diff: %u(%u) passed: %d \n",
             expected.f, expected.u,
             probed[0].f, probed[0].u, vert_sign_match, 
             vert_ulps_diff, pf->ulps, vert_result, 
             probed[1].f, probed[1].u, frag_sign_match,
             frag_ulps_diff, pf->ulps, frag_result);

      overall_result = overall_result && vert_result && frag_result;
   }

   return overall_result;
}
