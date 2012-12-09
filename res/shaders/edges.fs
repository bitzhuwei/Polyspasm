#version 130

uniform sampler2D color;
uniform vec2 invRes;

out vec4 fragColor;

/* Use sobel operator to find edges */
void main()
{
	vec4	v = vec4(0);
	vec4	h = vec4(0);

	ivec2	tc = ivec2(gl_FragCoord.xy);
	
	// Vertical
	v += texelFetchOffset(color, tc, 0, ivec2(1, 1));
	v += 2*texelFetchOffset(color, tc, 0, ivec2(1, 0));
	v += texelFetchOffset(color, tc, 0, ivec2(1, -1));
	v -= texelFetchOffset(color, tc, 0, ivec2(-1, 1));
	v -= 2*texelFetchOffset(color, tc, 0, ivec2(-1, 0));
	v -= texelFetchOffset(color, tc, 0, ivec2(-1, -1));
	
	// Horizontal
	h += texelFetchOffset(color, tc, 0, ivec2(1, 1));
	h += 2*texelFetchOffset(color, tc, 0, ivec2(0, 1));
	h += texelFetchOffset(color, tc, 0, ivec2(-1, 1));
	h -= texelFetchOffset(color, tc, 0, ivec2(1, -1));
	h -= 2*texelFetchOffset(color, tc, 0, ivec2(0, -1));
	h -= texelFetchOffset(color, tc, 0, ivec2(-1, -1));
	fragColor = vec4(length(h.xyz)+length(v.xyz));
}
