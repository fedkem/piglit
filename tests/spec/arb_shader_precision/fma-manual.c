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

/**
 * Test: divide-by-zero
 * Tests "Dividing by 0 results in the appropriately signed IEEE Inf." from
 * ARB_shader_precision.
 */

#include <string.h>

#include "piglit-util-gl-common.h"
#include "common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

   config.supports_gl_compat_version = 10;

   config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static char *TestName = "fma-manual";
static GLuint Prog;

//also do the fma operation manually on the cpu - is this correct?  will it be optimized away?
static float
manual_fma(float a, float b, float c)
{
   return a * b + c;
}

enum piglit_result
piglit_display(void)
{
   float *params;
   unsigned int num_params;
   GLboolean overall_result;
   precision_func_t pf = { 0 };

   pf.name = TestName;
   pf.func_type = THREE_PARAM;
   pf.func_three_param = manual_fma;
   pf.ulps = 0;

   params = load_and_alloc_params(TestName, &num_params);

   overall_result =
      precision_test_fv(Prog, "param", (float_u *) params, num_params, &pf);

   printf("%s: overall test result: %d\n", TestName, overall_result);
   piglit_present_results();
   piglit_report_result(overall_result ? PIGLIT_PASS : PIGLIT_FAIL);
   return overall_result ? PIGLIT_PASS : PIGLIT_FAIL;
}


void
piglit_init(int argc, char **argv)
{
   GLuint vs, fs;
   const char *verttext =
      "uniform vec3 param;\n"
      "varying float vertresult;\n"
      "void main(void) \n"
      "{ \n"
      "   vertresult = param[0] * param[1] + param[2];\n"
      "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
      "} \n";
   const char *fragtext =
      "uniform vec3 param;\n"
      "varying float vertresult;\n"
      "void main(void) \n"
      "{ \n"
      "   float fragresult = param[0] * param[1] + param[2];\n"
      "   gl_FragColor = vec4(vertresult, fragresult, param[0], param[2]);\n"
      "} \n";

   //piglit_require_extension("GL_ARB_fragment_shader");

   if (piglit_width < WIDTH || piglit_height < HEIGHT) {
      printf("%s: window is too small.\n", TestName);
      exit(1);
   }

   piglit_ortho_projection(WIDTH, HEIGHT, GL_FALSE);

   if (setup_unclamped_output()) {
      printf("%s: failed to setup unclamped output, exiting\n", TestName);
      exit(1);
   }

   vs = piglit_compile_shader_text(GL_VERTEX_SHADER, verttext);
   fs = piglit_compile_shader_text(GL_FRAGMENT_SHADER, fragtext);
   Prog = piglit_link_simple_program(vs, fs);
   glUseProgram(Prog);

   glViewport(0, 0, WIDTH, HEIGHT);

}
