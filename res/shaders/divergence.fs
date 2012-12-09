#version 130

uniform sampler2D	w;
uniform vec2		invRes;
uniform float		halfrdx;

out float			div;

void main()
{
	ivec2	tc = ivec2(gl_FragCoord.xy);
	vec2	wL = texelFetchOffset(w, tc, 0, ivec2(-1, 0)).xy;
	vec2	wR = texelFetchOffset(w, tc, 0, ivec2(1, 0)).xy;
	vec2	wT = texelFetchOffset(w, tc, 0, ivec2(0, 1)).xy;
	vec2	wB = texelFetchOffset(w, tc, 0, ivec2(0, -1)).xy;
	
	div = halfrdx * (wR.x-wL.x + wT.y-wB.y);
}
