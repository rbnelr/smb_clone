
#define _CRT_SECURE_NO_WARNINGS 1

#include "stdio.h"
#include <string>
#include <vector>

#include "glad.c"
#include "glfw3.h"

#include "Mmsystem.h"
static void dbg_play_sound () {
	PlaySound(TEXT("recycle.wav"), NULL, SND_FILENAME|SND_ASYNC);
}

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
//#define STBI_ONLY_BMP	1
#define STBI_ONLY_PNG	1
//#define STBI_ONLY_TGA	1
//#define STBI_ONLY_JPEG	1
//#define STBI_ONLY_HDR	1

#include "stb_image.h"

#include "raz_libs/typedefs.hpp"
typedef std::string str;
typedef std::string const& strcr;

#include "raz_libs/vector.hpp"

typedef f32		flt;
typedef s32v2	iv2;
typedef s32v3	iv3;
typedef fv2		v2;
typedef fv3		v3;
typedef fv4		v4;
typedef fm2		m2;
typedef fm3		m3;
typedef fm4		m4;

typedef u8v4 icol;

static constexpr icol irgb (u8 r, u8 g, u8 b) {
	return icol(r,g,b,255);
}

void failed_assert_log () {
	fprintf(stderr, "Assertion failed!\n");
}
void failed_assert_log (cstr format, ...) {
	va_list vl;
	va_start(vl, format);

	fprintf(stderr, "Assertion failed! ");

	vfprintf(stderr, format, vl);

	fprintf(stderr, "\n");

	va_end(vl);
}
#define assert_log(cond, ...) if (!(cond)) failed_assert_log(__VA_ARGS__)

static void glfw_error_proc (int err, cstr msg) {
	fprintf(stderr, "GLFW Error! 0x%x '%s'\n", err, msg);
}

static void APIENTRY ogl_debuproc (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, cstr message, void const* userParam) {

	//if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) return;

	// hiding irrelevant infos/warnings
	switch (id) {
		case 131185: // Buffer detailed info (where the memory lives which is supposed to depend on the usage hint)
		case 1282: // using shader that was not compiled successfully

		case 2: // API_ID_RECOMPILE_FRAGMENT_SHADER performance warning has been generated. Fragment shader recompiled due to state change.
		case 131218: // Program/shader state performance warning: Fragment shader in program 3 is being recompiled based on GL state.

		//case 131154: // Pixel transfer sync with rendering warning

		//case 1282: // Wierd error on notebook when trying to do texture streaming
		//case 131222: // warning with unused shadow samplers ? (Program undefined behavior warning: Sampler object 0 is bound to non-depth texture 0, yet it is used with a program that uses a shadow sampler . This is undefined behavior.), This might just be unused shadow samplers, which should not be a problem
		//case 131218: // performance warning, because of shader recompiling based on some 'key'
		return;
	}

	cstr src_str = "<unknown>";
	switch (source) {
		case GL_DEBUG_SOURCE_API_ARB:				src_str = "GL_DEBUG_SOURCE_API_ARB";				break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:		src_str = "GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB";		break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:	src_str = "GL_DEBUG_SOURCE_SHADER_COMPILER_ARB";	break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:		src_str = "GL_DEBUG_SOURCE_THIRD_PARTY_ARB";		break;
		case GL_DEBUG_SOURCE_APPLICATION_ARB:		src_str = "GL_DEBUG_SOURCE_APPLICATION_ARB";		break;
		case GL_DEBUG_SOURCE_OTHER_ARB:				src_str = "GL_DEBUG_SOURCE_OTHER_ARB";				break;
	}

	cstr type_str = "<unknown>";
	switch (source) {
		case GL_DEBUG_TYPE_ERROR_ARB:				type_str = "GL_DEBUG_TYPE_ERROR_ARB";				break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:	type_str = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB";	break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:	type_str = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB";	break;
		case GL_DEBUG_TYPE_PORTABILITY_ARB:			type_str = "GL_DEBUG_TYPE_PORTABILITY_ARB";			break;
		case GL_DEBUG_TYPE_PERFORMANCE_ARB:			type_str = "GL_DEBUG_TYPE_PERFORMANCE_ARB";			break;
		case GL_DEBUG_TYPE_OTHER_ARB:				type_str = "GL_DEBUG_TYPE_OTHER_ARB";				break;
	}

	cstr severity_str = "<unknown>";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH_ARB:			severity_str = "GL_DEBUG_SEVERITY_HIGH_ARB";		break;
		case GL_DEBUG_SEVERITY_MEDIUM_ARB:			severity_str = "GL_DEBUG_SEVERITY_MEDIUM_ARB";		break;
		case GL_DEBUG_SEVERITY_LOW_ARB:				severity_str = "GL_DEBUG_SEVERITY_LOW_ARB";			break;
	}

	fprintf(stderr, "OpenGL debug proc: severity: %s src: %s type: %s id: %d  %s\n",
		severity_str, src_str, type_str, id, message);
}

static bool load_text_file (strcr filepath, str* text) {
	
	FILE* f = fopen(filepath.c_str(), "rb"); // we don't want "\r\n" to "\n" conversion, because it interferes with our file size calculation
	if (!f) return false;

	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);

	text->resize(filesize);

	size_t ret = fread(&(*text)[0], 1,text->size(), f);
	if (ret != (size_t)filesize) return false;

	return true;
}

struct Shader {
	str								vert_filename;
	str								frag_filename;

	GLuint							prog;

	static bool get_shader_compile_log (GLuint shad, str* log) {
		GLsizei log_len;
		{
			GLint temp = 0;
			glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &temp);
			log_len = (GLsizei)temp;
		}

		if (log_len <= 1) {
			return false; // no log available
		} else {
			// GL_INFO_LOG_LENGTH includes the null terminator, but it is not allowed to write the null terminator in str, so we have to allocate one additional char and then resize it away at the end

			log->resize(log_len);

			GLsizei written_len = 0;
			glGetShaderInfoLog(shad, log_len, &written_len, &(*log)[0]);
			assert_log(written_len == (log_len -1));

			log->resize(written_len);

			return true;
		}
	}
	static bool get_program_link_log (GLuint prog, str* log) {
		GLsizei log_len;
		{
			GLint temp = 0;
			glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &temp);
			log_len = (GLsizei)temp;
		}

		if (log_len <= 1) {
			return false; // no log available
		} else {
			// GL_INFO_LOG_LENGTH includes the null terminator, but it is not allowed to write the null terminator in str, so we have to allocate one additional char and then resize it away at the end

			log->resize(log_len);

			GLsizei written_len = 0;
			glGetProgramInfoLog(prog, log_len, &written_len, &(*log)[0]);
			assert_log(written_len == (log_len -1));

			log->resize(written_len);

			return true;
		}
	}

	static bool load_shader (GLenum type, strcr filename, GLuint* shad) {
		*shad = glCreateShader(type);

		str source;
		if (!load_text_file(filename, &source)) {
			fprintf(stderr, "Could not load shader source");
			return false;
		}

		{
			cstr ptr = source.c_str();
			glShaderSource(*shad, 1, &ptr, NULL);
		}

		glCompileShader(*shad);

		bool success;
		{
			GLint status;
			glGetShaderiv(*shad, GL_COMPILE_STATUS, &status);

			str log_str;
			bool log_avail = get_shader_compile_log(*shad, &log_str);

			success = status == GL_TRUE;
			if (!success) {
				// compilation failed
				assert_log(false, "OpenGL error in shader compilation \"%s\"!\n>>>\n%s\n<<<\n", filename.c_str(), log_avail ? log_str.c_str() : "<no log available>");
			} else {
				// compilation success
				if (log_avail) {
					assert_log(false, "OpenGL shader compilation log \"%s\":\n>>>\n%s\n<<<\n", filename.c_str(), log_str.c_str());
				}
			}
		}

		return success;
	}
	bool load_program () {

		prog = glCreateProgram();

		GLuint vert;
		GLuint frag;

		bool compile_success = true;
		
		bool vert_success = load_shader(GL_VERTEX_SHADER,		vert_filename, &vert);
		bool frag_success = load_shader(GL_FRAGMENT_SHADER,		frag_filename, &frag);

		if (!(vert_success && frag_success)) {
			glDeleteProgram(prog);
			prog = 0;
			return false;
		}

		glAttachShader(prog, vert);
		glAttachShader(prog, frag);

		glLinkProgram(prog);

		bool success;
		{
			GLint status;
			glGetProgramiv(prog, GL_LINK_STATUS, &status);

			str log_str;
			bool log_avail = get_program_link_log(prog, &log_str);

			success = status == GL_TRUE;
			if (!success) {
				// linking failed
				assert_log(false, "OpenGL error in shader linkage \"%s\"|\"%s\"!\n>>>\n%s\n<<<\n", vert_filename.c_str(), frag_filename.c_str(), log_avail ? log_str.c_str() : "<no log available>");
			} else {
				// linking success
				if (log_avail) {
					assert_log(false, "OpenGL shader linkage log \"%s\"|\"%s\":\n>>>\n%s\n<<<\n", vert_filename.c_str(), frag_filename.c_str(), log_str.c_str());
				}
			}
		}

		glDetachShader(prog, vert);
		glDetachShader(prog, frag);

		glDeleteShader(vert);
		glDeleteShader(frag);

		return success;
	}
	
};

struct Display {
	GLFWwindow*	wnd;
	iv2			dim;
};
Display disp;

v2 mouse_pos_01_bottom_up = -1;

v2 dbg_cam_center_world; // inited before main loop, this _should_ be okay
flt dbg_cam_pixel_scale = 6;

v2 dbg_cam_size_world = 0;
v2 drag_begin_mouse_pos_world = 0;

v2 dbg_mouse_drag_calc_cam_center_world (v2 dbg_cam_size_world) {
	// v2 mouse_pos_world = (mouse_pos_01_bottom_up -0.5f) * dbg_cam_size_world +dbg_cam_center_world;
	// drag_begin_mouse_pos_world

	v2 dbg_cam_center_world = drag_begin_mouse_pos_world -((mouse_pos_01_bottom_up -0.5f) * dbg_cam_size_world);

	return dbg_cam_center_world;
}

bool dbg_mouse_cam_dragging = false;

void begin_dbg_mouse_drag () {
	v2 size = dbg_cam_size_world; // this is one frame old
	drag_begin_mouse_pos_world = (mouse_pos_01_bottom_up -0.5f) * size +dbg_cam_center_world;
	dbg_mouse_cam_dragging = true;
}
void end_dbg_mouse_drag () {
	dbg_cam_center_world = dbg_mouse_drag_calc_cam_center_world(dbg_cam_size_world);
	dbg_mouse_cam_dragging = false;
}

void glfw_mouse_button_event (GLFWwindow* window, int button, int action, int mods) {
	bool went_down = action == GLFW_PRESS;

	switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			if (went_down) {
				begin_dbg_mouse_drag();
			} else {
				end_dbg_mouse_drag();
			}
			break;
	}
}
void glfw_mouse_scroll (GLFWwindow* window, double xoffset, double yoffset) {
	static flt zoom_log = log2f(dbg_cam_pixel_scale);

	zoom_log += (flt)yoffset * 0.1f;

	dbg_cam_pixel_scale = powf(2, zoom_log);
}

static Display init_engine () {
	glfwSetErrorCallback(glfw_error_proc);

	assert_log(glfwInit() != 0, "glfwInit() failed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	bool GL_VAOS_REQUIRED = true;

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

	Display disp = {};
	disp.wnd = glfwCreateWindow(1280, 720, u8"2D Game", NULL, NULL);

	glfwSetMouseButtonCallback(disp.wnd,	glfw_mouse_button_event);
	glfwSetScrollCallback(disp.wnd,			glfw_mouse_scroll);

	glfwMakeContextCurrent(disp.wnd);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (GLAD_GL_ARB_debug_output) {
		glDebugMessageCallbackARB(ogl_debuproc, 0);
		//DEBUG_OUTPUT_SYNCHRONOUS_ARB this exists -> if ogl_debuproc needs to be thread safe
	}

	GLuint vao; // one global vao for everything

	if (GL_VAOS_REQUIRED) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

	return disp;
}

int main (int argc, char** argv) {

	disp = init_engine();

	// Rendering
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SCISSOR_TEST); // to render to sub area of window to enable having black bars around game screen
	
	iv2 tile_res = 16;
	flt pixel_scale;

	int zlayer_min = -100;
	int zlayer_max = +100;

	struct Vertex {
		v2		pos_clip;
		f32		z_clip;
		v2		uv;
		icol	tint_col;
	};

	Shader shad_sprite = { "shaders/sprite.vert", "shaders/sprite.frag" };
	shad_sprite.load_program();
	GLint loc_pos =		glGetAttribLocation(shad_sprite.prog, "attr_pos_clip");
	GLint loc_z =		glGetAttribLocation(shad_sprite.prog, "attr_z_clip");
	GLint loc_uv =		glGetAttribLocation(shad_sprite.prog, "attr_uv");
	GLint loc_tint =	glGetAttribLocation(shad_sprite.prog, "attr_tint_col");

	GLint tex_tex_unit = 0;
	GLint tex_loc = glGetUniformLocation(shad_sprite.prog, "tex");
	glUniform1i(tex_loc, tex_tex_unit);

	GLuint vbo;
	{
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glEnableVertexAttribArray(loc_pos);
		glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos_clip));

		glEnableVertexAttribArray(loc_z);
		glVertexAttribPointer(loc_z, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, z_clip));
	
		glEnableVertexAttribArray(loc_uv);
		glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glEnableVertexAttribArray(loc_tint);
		glVertexAttribPointer(loc_tint, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, tint_col));
	}

	struct Texture {
		GLuint	gl;
		iv2		dim;
	};
	auto load_tile_texture = [&] (cstr texture_filepath) -> Texture {
		Texture ret;

		stbi_set_flip_vertically_on_load(true); // OpenGL has textues bottom-up

		int n = 4;
		u8* data = stbi_load(texture_filepath, &ret.dim.x,&ret.dim.y, &n, 0);
		if (!data) fprintf(stderr, "Texture \"%s\" could not be loaded", texture_filepath);

		glGenTextures(1, &ret.gl);

		glBindTexture(GL_TEXTURE_2D, ret.gl);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ret.dim.x,ret.dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		return ret;
	};
	Texture tex_tileset = load_tile_texture("assets_src/tileset.png");
	Texture tex_mario = load_tile_texture("assets_src/mario.png");

	std::vector<Vertex> vbo_data_world;
	std::vector<Vertex> vbo_data_mario;
	
	// Visuals
	iv3 smb_sky_col = iv3(91,146,248);
	v3 smb_sky_colf = (v3)smb_sky_col / 255;

	// Gameplay
	v2 mario_pos = v2(3, 0);

	dbg_cam_center_world = mario_pos +0.5f;
	dbg_cam_pixel_scale = 6;

	v2 game_camera_pos_world = v2(0, -1.75f);
	v2 game_camera_size_world = v2(16,15); // nes had res of 256x240 and the game had a tile size of 16x16

	for (int frame_i=0;; frame_i++) {
		
		glfwPollEvents();
		
		if (glfwWindowShouldClose(disp.wnd)) break;

		{
			f64 x, y;
			glfwGetCursorPos(disp.wnd, &x, &y);
			iv2 mouse_pos_px = iv2((int)x, (int)y);
			mouse_pos_01_bottom_up = ((v2)mouse_pos_px +0.5f) / (v2)disp.dim;
			mouse_pos_01_bottom_up.y = 1 -mouse_pos_01_bottom_up.y;
		}

		glfwGetFramebufferSize(disp.wnd, &disp.dim.x, &disp.dim.y);
		
		bool dbg_camera = true;
		
		v2 camera_pos_world; // camera has origin in lower left corner
		v2 camera_size_world;

		iv2 camera_dim_px;
		
		if (!dbg_camera) {
			#if 0
			// render black bars
			{
				glViewport(0,0, disp.dim.x,disp.dim.y);

				glClearColor(0,0,0, 255);
				glClear(GL_COLOR_BUFFER_BIT);
			}

			iv2 camera_screen_scissor_rect_pos;
			iv2 camera_screen_dim;
			{
				iv2 original_screen_dim_px = tile_res * (iv2)camera_size_world;
			
				iv2 max_pixel_scale_xy = max(1, disp.dim / original_screen_dim_px);

				int biggest_possible_pixel_scale = min(max_pixel_scale_xy.x, max_pixel_scale_xy.y);
				pixel_scale = biggest_possible_pixel_scale;

				camera_screen_dim = pixel_scale * original_screen_dim_px;

				assert_log(all(camera_screen_dim <= disp.dim));
				assert_log(any(camera_screen_dim == disp.dim));

				iv2 remain_px = disp.dim -camera_screen_dim;

				iv2 lower_bar_sz_px = remain_px / 2;

				camera_screen_scissor_rect_pos = lower_bar_sz_px;
			}

			{
				glViewport(camera_screen_scissor_rect_pos.x,camera_screen_scissor_rect_pos.y, camera_screen_dim.x,camera_screen_dim.y);
				glScissor(camera_screen_scissor_rect_pos.x,camera_screen_scissor_rect_pos.y, camera_screen_dim.x,camera_screen_dim.y);

				glClearColor(smb_sky_colf.x,smb_sky_colf.y,smb_sky_colf.z, 255);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			}
			#endif
		} else {
			
			f32 tiles_y = (flt)disp.dim.y / (tile_res.y * dbg_cam_pixel_scale);

			camera_size_world = tiles_y;
			camera_size_world.x *= (flt)disp.dim.x / (flt)disp.dim.y;
			dbg_cam_size_world = camera_size_world;

			if (dbg_mouse_cam_dragging)
				dbg_cam_center_world = dbg_mouse_drag_calc_cam_center_world(dbg_cam_size_world);

			camera_pos_world = dbg_cam_center_world -camera_size_world / 2;

			camera_dim_px = disp.dim;

			glViewport(0,0, camera_dim_px.x,camera_dim_px.y);
			glScissor(0,0, camera_dim_px.x,camera_dim_px.y);

			glClearColor(smb_sky_colf.x,smb_sky_colf.y,smb_sky_colf.z, 255);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}

		// render game
		glUseProgram(shad_sprite.prog);
		
		auto emit_sprite = [&] (std::vector<Vertex>* vbo_data, Texture const& tex, v2 pos_world, iv2 tile, int zlayer, iv2 tiles_dim, iv2 offs_px=0, icol tint_col=255) {
			// pos
			v2 pos_cam_rel = pos_world +((v2)offs_px / (v2)tile_res) -camera_pos_world;
			
			v2 pos_cam_normalized = pos_cam_rel / camera_size_world;
			v2 sz_cam_normalized = (v2)tiles_dim / camera_size_world;

			v2 pos_clip = pos_cam_normalized * 2 -1;
			v2 sz_clip = sz_cam_normalized * 2;

			// z
			flt z_clip = map((flt)zlayer, (flt)zlayer_min -1, (flt)zlayer_max +1, +0.5f, -0.5f); // map zlayer range to somewhere within [+1,-1] range

			// uv
			iv2 tiles_on_tilemap = tex.dim / tile_res;

			tile.y = tiles_on_tilemap.y -tiles_dim.y -tile.y; // tile pos is top-down, but texture is bottom-up
			
			// bias uvs to fix nearest filtering rounding to wrong texel for some reason
			flt bias = 0 ? 0.001f : 0; // this is not needed ? -_-
			v2 uv_l = (((v2)tile             +0) * (v2)tile_res +bias) / (v2)tex.dim;
			v2 uv_h = (((v2)tile +(v2)tiles_dim) * (v2)tile_res -bias) / (v2)tex.dim;

			// doesnt really work
			v2 uv_bias = 0.01f / (v2)tile_res; // to fix nearest filtering rounding to wrong texel for some reason, could be solved by not interpolating uvs from 0-1 but instead from 1/32 to 21/32 (which corresponds to the pixel center uvs for 16x16 tiles)

			for (auto p : { v2(1,0),v2(1,1),v2(0,0), v2(0,0),v2(1,1),v2(0,1) })
				vbo_data->push_back({ pos_clip +sz_clip * p, z_clip, lerp(uv_l, uv_h, p), tint_col });
		};
		
		{ // world
			vbo_data_world.clear();
			
			iv2 ground_block =			iv2(0,0);
			iv2 question_mark_block =	iv2(24,0);
			iv2 coin_block =			iv2(1,0);
			iv2 cloud_tiles =			iv2(0, 20);

			auto emit_block = [&] (v2 pos_world, iv2 tile, int zlayer, iv2 tiles_dim=1) {
				emit_sprite(&vbo_data_world, tex_tileset, pos_world, tile, zlayer, tiles_dim, 0);
			};
			auto emit_cloud = [&] (v2 pos_world, int length, int zlayer) {
				for (int i=0; i<length; ++i) {
					iv2 tile;
					if (		i == 0 )		tile = cloud_tiles +iv2(0,0);
					else if (	i == length-1 )	tile = cloud_tiles +iv2(2,0);
					else						tile = cloud_tiles + iv2(1, 0);

					emit_sprite(&vbo_data_world, tex_tileset, pos_world +(v2)iv2(i,  0), tile, zlayer, iv2(1,2), 0);
				}
			};
			auto emit_bush = [&] (v2 pos_world, int length, int zlayer) {
				for (int i=0; i<length; ++i) {
					iv2 tile;
					if (		i == 0 )		tile = cloud_tiles +iv2(0,0);
					else if (	i == length-1 )	tile = cloud_tiles +iv2(2,0);
					else						tile = cloud_tiles + iv2(1, 0);

					emit_sprite(&vbo_data_world, tex_tileset, pos_world +(v2)iv2(i,  0), tile, zlayer, iv2(1,2), 0, irgb(140,214,0));
				}
			};

			for (int j=-1; j>=-2; --j)
				for (int i=0; i<50; ++i)
					emit_block((v2)iv2(i, j), ground_block, 0);
			
			emit_block((v2)iv2(8, 3), question_mark_block, 0);
			
			emit_block((v2)iv2(15, 3), coin_block, 0);
			emit_block((v2)iv2(16, 3), question_mark_block, 0);
			emit_block((v2)iv2(17, 3), coin_block, 0);
			emit_block((v2)iv2(18, 3), question_mark_block, 0);
			emit_block((v2)iv2(19, 3), coin_block, 0);

			emit_block((v2)iv2(17, 7), question_mark_block, 0);

			emit_cloud(v2(0,  3), 3, -2);
			emit_cloud(v2(0,  6), 4, -2);
			emit_cloud(v2(0,  9), 5, -2);
			emit_cloud(v2(0, 12), 7, -2);

			emit_bush(v2(8, -1), 5, -1);

			if (vbo_data_world.size() != 0) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vbo_data_world.size(), &vbo_data_world[0], GL_STREAM_DRAW);

				glActiveTexture(GL_TEXTURE0 +tex_tex_unit);
				glBindTexture(GL_TEXTURE_2D, tex_tileset.gl);

				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vbo_data_world.size());
			}
		}
		{ // world
			vbo_data_mario.clear();

			iv2 mario_tile = iv2(0,0);

			emit_sprite(&vbo_data_mario, tex_mario, mario_pos, mario_tile, +1, iv2(1,1), iv2(0, -1));
			
			if (vbo_data_mario.size() != 0) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vbo_data_mario.size(), &vbo_data_mario[0], GL_STREAM_DRAW);

				glActiveTexture(GL_TEXTURE0 +tex_tex_unit);
				glBindTexture(GL_TEXTURE_2D, tex_mario.gl);

				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vbo_data_mario.size());
			}
		}

		glfwSwapBuffers(disp.wnd);

	}

	glfwDestroyWindow(disp.wnd);
	glfwTerminate();

	return 0;
}
