#version 130

uniform sampler2D	src;
uniform sampler2D	dest;

out vec4 fragColor;

void main()
{
	ivec2	tc = ivec2(gl_FragCoord.xy);
	vec4	s = texelFetch(src, tc, 0);
	vec4	d = texelFetch(dest, tc, 0);
	fragColor = vec4(d.r + s.r);
}
