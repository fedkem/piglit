/*
 * Copyright © 2013 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/** @file gl-layer.c
 *
 * Section 4.4.7(Framebuffer Objects) From GL spec 3.2 core:
 *
 * When rendering to a layered framebuffer, each fragment generated by
 * the GL is assigned a layer number. The layer number for a fragment
 * is zero if
 *
 *      - geometry shaders are disabled, or
 *
 *      - The current geometry shader does not statically assign a
 *      value to the built-in output variable gl_Layer.
 *
 *
 * Test
 *    Draw a rect onto a layered texture with two different programs. One
 *  program will contain a geometry shader that doesn't assign a value to
 *  gl_Layer, while the other program will not contain a geometry shader.
 *
 * End Result
 *    The first layer of both textures used, will have their values be green or
 *  (0, 1, 0), and the remaining layers will be not set.
 */

 #include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 32;
	config.supports_gl_core_version   = 32;

PIGLIT_GL_TEST_CONFIG_END

const char *vs_source = {
	"#version 150\n"
	"in vec4 piglit_vertex;\n"
	"out vec4 vert;\n"
	"void main() {\n"
	"	gl_Position = piglit_vertex;\n"
	"	vert = piglit_vertex;\n"
	"}\n"
};

const char *gs_source = {
	"#version 150\n"
	"layout(triangles) in;\n"
	"layout(triangle_strip, max_vertices = 3) out;\n"
	"in vec4 vert[3];\n"
	"\n"
	"void main()\n"
	"{\n"
	"	for(int i = 0; i < 3; i++) {\n"
	"		gl_Position = vert[i];\n"
	"		EmitVertex();\n"
	"	}\n"
	"}\n"
};

const char *fs_source = {
	"#version 150\n"
	"void main() {\n"
	"	gl_FragColor = vec4(0, 1, 0, 1);\n"
	"}\n"
};

bool check_framebuffer_status(GLenum target, GLenum expected) {
	GLenum observed = glCheckFramebufferStatus(target);
	if(expected != observed) {
		printf("Unexpected framebuffer status!\n"
		       "  Observed: %s\n  Expected: %s\n",
		       piglit_get_gl_enum_name(observed),
		       piglit_get_gl_enum_name(expected));
		return false;
	}
	return true;
}

bool
probe_texture_layered_rgb(GLuint texture, int x, int y, int z,
			  int w, int h, int d, float *expected)
{
	int k;
	GLuint fbo;
	GLint prev_read_fbo;
	GLint prev_draw_fbo;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prev_draw_fbo);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prev_read_fbo);

	for(k = 0; k < d; k++ ) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					  texture, 0, z + k);

		if(!piglit_probe_rect_rgb(0, 0, w, h, &expected[k*3])) {
			printf("Layer: %i\n", z + k);
			return false;
		}
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_draw_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, prev_read_fbo);

	glDeleteFramebuffers(1, &fbo);

	return true;
}


bool test_gl_layer(GLint prog, int layers, float *expected)
{
	bool pass = true;
	GLuint fbo, texture;

	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &texture);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, 10,
		     10, 2, 0, GL_RGB, GL_FLOAT, NULL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			     texture, 0);

	if(!check_framebuffer_status(GL_FRAMEBUFFER, GL_FRAMEBUFFER_COMPLETE) ||
	   !piglit_check_gl_error(GL_NO_ERROR)) {
		piglit_report_result(PIGLIT_FAIL);
	}

	glUseProgram(prog);

	piglit_draw_rect(-1, -1, 2, 2);

	pass = probe_texture_layered_rgb(texture, 0, 0, 0,
					 10, 10, 2, expected) && pass;

	glDeleteTextures(1, &texture);
	glDeleteFramebuffers(1, &fbo);

	pass = piglit_check_gl_error(GL_NO_ERROR) && pass;
	return pass;
}

void
piglit_init(int argc, char **argv)
{
	int i;
	GLint program[2];
	bool pass = true;

	float expected[] = {
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0
	};

	program[0] = piglit_build_simple_program_multiple_shaders(
						GL_VERTEX_SHADER,   vs_source,
						GL_GEOMETRY_SHADER, gs_source,
						GL_FRAGMENT_SHADER, fs_source,
						0);

	program[1] = piglit_build_simple_program(vs_source, fs_source);

	for( i = 0; i < 2; i++) {
		pass = test_gl_layer(program[i], 2, expected) && pass;
	}

	pass = piglit_check_gl_error(GL_NO_ERROR) && pass;
	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result
piglit_display(void)
{
	/* UNREACHABLE */
	return PIGLIT_FAIL;
}
