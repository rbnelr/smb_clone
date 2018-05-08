#version 330 core // version 3.3

in		vec2	attr_pos_clip;
in		float	attr_z_clip;
in		vec2	attr_uv;
in		vec4	attr_tint_col;

out		vec2	vs_uv;
out		vec4	vs_tint_col;

// for debugging
out		vec3	vs_barycentric;

const vec3[] BARYCENTRIC = vec3[] ( vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) );

void main () {
	gl_Position =		vec4(attr_pos_clip, attr_z_clip, 1);
	vs_uv =				attr_uv;
	vs_tint_col =		attr_tint_col;

	vs_barycentric = BARYCENTRIC[gl_VertexID % 3];
}
