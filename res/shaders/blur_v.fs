#version 130

uniform sampler2D color;

out vec4 fragColor;

#define PI (3.14159265358979323846264)
#define NSAMP (10)

float gauss(float d)
{
	return (1.0 / (2.0 * PI)) * exp(-d*d/2);
}

void main()
{
	ivec2 tc = ivec2(gl_FragCoord.xy);

#if 1
	float g[NSAMP];
	float inv3sigma = 3.0/(NSAMP);
	for (int i = 0; i < NSAMP; ++i) {
		g[i] = gauss(i*inv3sigma);
	}
	vec3 v = vec3(0);
	for (int y = 1-NSAMP; y < NSAMP; ++y) {
		v += texelFetch(color, tc + ivec2(0, y), 0).rgb * g[abs(y)];
	}
	fragColor = vec4(v, 1.0);
#else
	fragColor = texelFetch(color, tc, 0);
#endif
}
