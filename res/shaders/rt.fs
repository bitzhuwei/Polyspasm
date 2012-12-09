#version 330
#extension GL_EXT_gpu_shader4 : enable

#define INFINITY (1e30)
#define PI (3.1415926535897932384626433832795)
#define EPSILON (0.000001)
#define RAY_OFFSET (0.0005)

uniform usampler2D	octree;
uniform usampler1D	blockColors;
uniform usampler1D	blockEmittance;
uniform sampler2D	prevSamples;
uniform usampler2D	prevSampleCount;

uniform vec2		invRes;
uniform int			depth;// octree depth
uniform float		alpha;
uniform vec3		origin;
uniform vec2		seed;

in vec3				cameraRay;

layout(location = 0) out vec4	sample;
layout(location = 1) out uint	sampleCount;

vec3	d;
vec3	o;
vec3	n = vec3(0, 1, 0);
vec3	l = normalize(vec3(0.1, 1, -0.2));
vec2	state = gl_FragCoord.xy + seed*113456.0;

float rand_float()
{
	uint n = floatBitsToUint(state.y * 214013.0 + state.x * 2531011.0);
	n = n * (n * n * 15731u + 789221u);
	n = (n >> 9u) | 0x3F800000u;

	return 2.0 - uintBitsToFloat(n);
}

vec2 rand_vec()
{
	state = vec2(rand_float(), rand_float());
	return state;
}

int locate(int node)
{
	ivec2	size = textureSize(octree, 0);
	ivec2	tc = ivec2(node % size.x, node / size.x);
	return int(texelFetch(octree, tc, 0).r);
}

#define GTYPE float

/**
 * Ray-octree intersection test
 * Returns the intersecting block type
 */
int intersect()
{
	int		node;
	int		level;
	int		type = -1;
	
	ivec3	x;
	ivec3	l;
	float	tNear = INFINITY;
	float	t;
	
	GTYPE	g0;
	GTYPE	g1;
	float	inv;
	float	lev;
	
	while (true) {
		
		// add small offset past the intersection to avoid
		// recursion to the same octree node!
		x = ivec3(floor(o + d * RAY_OFFSET));
		
		node = 0;
		level = depth;
		l = x >> level;
		
		if (l.x != 0 || l.y != 0 || l.z != 0) {
			// outside octree!
			return 0;
		}
		
		type = locate(node);
		while (type == -1) {
			level -= 1;
			l = x >> level;
			node = locate(node + 1 + (((l.x&1)<<2) | ((l.y&1)<<1) | (l.z&1)));
			type = locate(node);
		}

		if (type != 0) {
			// hit a non-air node
			return type;
		}

		// exit current octree node:

		lev = 1<<level;
		
		inv = 1.0 / d.x;
		t = (l.x*lev - o.x) * inv;
		g0 = GTYPE(t > EPSILON);
		tNear = t * g0 + tNear * (1-g0);
		n = vec3(1, 0, 0);
		
		t += lev * inv;
		g1 = GTYPE(t < tNear && t > EPSILON);
		tNear = t * g1 + tNear * (1-g1);
		n = vec3(-1, 0, 0) * g1 + n * (1-g1);

		inv = 1.0 / d.y;
		t = (l.y*lev - o.y) * inv;
		g0 = GTYPE(t < tNear && t > EPSILON);
		tNear = t * g0 + tNear * (1-g0);
		n = vec3(0, 1, 0) * g0 + n * (1-g0);
		
		t += lev * inv;
		g1 = GTYPE(t < tNear && t > EPSILON);
		tNear = t * g1 + tNear * (1-g1);
		n = vec3(0, -1, 0) * g1 + n * (1-g1);

		inv = 1.0 / d.z;
		t = (l.z*lev - o.z) * inv;
		g0 = GTYPE(t < tNear && t > EPSILON);
		tNear = t * g0 + tNear * (1-g0);
		n = vec3(0, 0, 1) * g0 + n * (1-g0);
		
		t += lev * inv;
		g1 = GTYPE(t < tNear && t > EPSILON);
		tNear = t * g1 + tNear * (1-g1);
		n = vec3(0, 0, -1) * g1 + n * (1-g1);

		o = d * tNear + o;
		tNear = INFINITY;
	}
}

void reflect_diffuse()
{
	// random point on unit disk
	vec2	x = rand_vec();
	float	r = sqrt(x.x);
	float	theta = 2 * PI * x.y;

	// project point on hemisphere in tangent space
	vec3	t = vec3(r * cos(theta), r * sin(theta), sqrt(1 - x.x));
	
	// transform from tangent space to world space
	vec3 w;
	vec3 u;
	vec3 v;
	
	if (abs(n.x) > 0.1) {
		w = vec3(0, 1, 0);
	} else {
		w = vec3(1, 0, 0);
	}
	
	u = normalize(cross(w, n));
	v = cross(u, n);
	
	d = u * t.x + v * t.y + n * t.z;

	o = RAY_OFFSET * d + o;
}

void main()
{
	d = normalize(cameraRay);
	o = vec3(256.01, 256.01, 256.01) - origin;
	int		type = intersect();
	if (type != 0) {
		sample.rgb = n;
	} else {
		sample.rgb = vec3(0);
		float m = max(dot(d, vec3(0.0, 1.0, 0.0))+0.2, 0.0)/1.2;
		sample.rgb = (vec3(0.9, 1.0, 0.05)*alpha +
			(1-alpha)*(
				vec3(0.1, 0.37, 0.57) * m +
				vec3(0.80, 0.87, 0.99) * (1-m)));
	}
	sample.a = 1.0;
}
