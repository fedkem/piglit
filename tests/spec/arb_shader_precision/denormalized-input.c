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
 * Test: denormalized-input
 * Tests "a denormalized value input into a shader can be flushed to zero"
 * from ARB_shader_precision.
 */

#include "piglit-util-gl-common.h"
#include "common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

   config.supports_gl_core_version = 33;

   config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static char *TestName = "denormalized-input";
static GLuint Prog;

static float
denormalized_input(float f)
{
   //simulate flushing to zero
   return 0.0f;
}

enum piglit_result
piglit_display(void)
{
   float *params;
   unsigned int num_params;
   GLboolean overall_result;
   precision_func_t pf = { 0 };

   pf.name = TestName;
   pf.func_type = ONE_PARAM;
   pf.func_one_param = denormalized_input;
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
   const char *verttext =
      "#version 330\n"
      "uniform float param;\n"
      "in vec4 piglit_vertex;\n"
      "out float vertresult;\n"
      "void main(void) \n"
      "{ \n"
      "   vertresult = param;\n"
      "   gl_Position = piglit_vertex;\n"
      "} \n";
   const char *fragtext =
      "#version 330\n"
      "uniform float param;\n"
      "in float vertresult;\n"
      "out vec4 outputColor;\n"
      "void main(void) \n"
      "{ \n"
      "   float fragresult = param;\n"
      "   outputColor = vec4(vertresult, fragresult, param, 1.0);\n"
      "} \n";

   //piglit_require_extension("GL_ARB_fragment_shader");

   if (piglit_width < WIDTH || piglit_height < HEIGHT) {
      printf("%s: window is too small.\n", TestName);
      exit(1);
   }

   if (setup_unclamped_output()) {
      printf("%s: failed to setup unclamped output, exiting\n", TestName);
      exit(1);
   }

   Prog = piglit_build_simple_program(verttext, fragtext);
   glUseProgram(Prog);

   glViewport(0, 0, WIDTH, HEIGHT);

}
