#version 130

uniform sampler2D source;

out vec4 fragColor;

#define NSAMP (2)

/* box blur */
void main()
{
	ivec2 tc = ivec2(gl_FragCoord.xy);
	vec3 v = vec3(0);
	float	rnum = pow(NSAMP*2.0-1.0, -2.0);
	for (int x = 1-NSAMP; x < NSAMP; ++x) {
		for (int y = 1-NSAMP; y < NSAMP; ++y) {
			v += texelFetch(source, tc + ivec2(x, y), 0).rgb * rnum;
		}
	}
	fragColor = vec4(v, 1.0);
}
