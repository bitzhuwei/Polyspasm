#version 130

uniform sampler2D color;
uniform vec2 invRes;

out vec4 fragColor;

#define PI (3.14159265358979323846264)
#define NSAMP (10)

float gauss(float d)
{
	return (1.0 / (2.0 * PI)) * exp(-d*d/2);
}

void main()
{
#if 1
	float g[NSAMP];
	float inv3sigma = 3.0/(NSAMP);
	for (int i = 0; i < NSAMP; ++i) {
		g[i] = gauss(i*inv3sigma);
	}
	vec3 v = vec3(0);
	ivec2 tc = ivec2(gl_FragCoord.xy);
	for (int x = 1-NSAMP; x < NSAMP; ++x) {
		v += texelFetch(color, tc + ivec2(x, 0), 0).rgb * g[abs(x)];
	}
	//fragColor = vec4(v, 1.0);
	float vignette = 1.5 * min(1, pow(1.5 - length(gl_FragCoord.xy*invRes*2.0 - 1.0), 0.8));
	fragColor = vec4(v, 1.0)*vignette;
	//fragColor = vec4(vec3(vignette), 1.0);
#else
	fragColor = texture(color, gl_FragCoord.xy*invRes);
#endif
}
