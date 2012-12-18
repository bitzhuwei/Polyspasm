#version 130
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

/*layout(location = 0)*/ out vec4	sample;
/*layout(location = 1)*/ out uint	sampleCount;

vec3	d;
vec3	o;
vec3	n = vec3(0, 1, 0);
vec3	l = normalize(vec3(0.1, 1, -0.2));
vec2	state = gl_FragCoord.xy + seed*113456.0;

float rand_float()
{
#if 1
	int n = int(state.x * 40.0 + state.y * 6400.0);
	n = (n << 13) ^ n;
	float r = 1.0 - float( (n * (n * n * 15731 + 789221) +
		1376312589) & 0x7fffffff) * (1.0 / 1073741824.0);
	return r * 0.5 + 0.5;
#else
	uint n = floatBitsToUint(state.y * 214013.0 + state.x * 2531011.0);
	n = n * (n * n * 15731u + 789221u);
	n = (n >> 9u) | 0x3F800000u;

	return 2.0 - uintBitsToFloat(n);
#endif
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

const vec3	normals[6] = vec3[6](
	vec3( 1, 0, 0),
	vec3(-1, 0, 0),
	vec3( 0, 1, 0),
	vec3( 0,-1, 0),
	vec3( 0, 0, 1),
	vec3( 0, 0,-1)
);

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
	float	t;
	
	bool	g0;
	bool	g1;
	bool	g2;
	float	lev;
	
	float	x0, x1, y0, y1, z0, z1;
	uint	nx, ny, nz, ni;

	float	rdx = 1.0/d.x;
	float	rdy = 1.0/d.y;
	float	rdz = 1.0/d.z;
	
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
		
		x0 = (l.x*lev - o.x) * rdx;
		x1 = x0 + lev * rdx;
		g0 = x0 > EPSILON;
		g1 = x1 > EPSILON;
		x0 = x0 * uint(g0) + INFINITY * uint(!g0);
		x1 = x1 * uint(g1) + INFINITY * uint(!g1);
		g2 = x0 < x1;
		x0 = x0 * uint(g2) + x1 * uint(!g2);
		nx = 0u * uint(g2) + 1u * uint(!g2);
		
		y0 = (l.y*lev - o.y) * rdy;
		y1 = y0 + lev * rdy;
		g0 = y0 > EPSILON;
		g1 = y1 > EPSILON;
		y0 = y0 * uint(g0) + INFINITY * uint(!g0);
		y1 = y1 * uint(g1) + INFINITY * uint(!g1);
		g2 = y0 < y1;
		y0 = y0 * uint(g2) + y1 * uint(!g2);
		ny = 2u * uint(g2) + 3u * uint(!g2);
		
		z0 = (l.z*lev - o.z) * rdz;
		z1 = z0 + lev * rdz;
		g0 = z0 > EPSILON;
		g1 = z1 > EPSILON;
		z0 = z0 * uint(g0) + INFINITY * uint(!g0);
		z1 = z1 * uint(g1) + INFINITY * uint(!g1);
		g2 = z0 < z1;
		z0 = z0 * uint(g2) + z1 * uint(!g2);
		nz = 4u * uint(g2) + 5u * uint(!g2);
		
		g2 = x0 < y0;
		t  = x0 * uint(g2) + y0 * uint(!g2);
		ni = nx * uint(g2) + ny * uint(!g2);
		
		g2 = t < z0;
		t  =  t * uint(g2) + z0 * uint(!g2);
		ni = ni * uint(g2) + nz * uint(!g2);

		o = d * t + o;
		n = normals[ni];
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

#define MAX_RAY_DEPTH (2)

bool kill(int ray_depth)
{
	return ray_depth > MAX_RAY_DEPTH;
}

vec3 pt()
{
	vec3	attenuation = vec3(1);
	vec3	emittance = vec3(0);
	int		ray_depth = 0;
	float	sky = 1.0;
#if 1
	vec2	r = 0.004 * rand_vec();
	
	vec3	w = normalize(cameraRay);
	vec3	u;
	vec3	v;
	
	if (abs(w.x) > 0.1) {
		u = vec3(0.0, 1.0, 0.0);
	} else {
		u = vec3(1.0, 0.0, 0.0);
	}
	
	v = normalize(cross(w, u));
	u = cross(v, w);
	
	d = w + r.x * v + r.y * u;
	//d = normalize(d);
#else
	//d = normalize(cameraRay);
	d = cameraRay;
#endif
	o = vec3(256.01, 256.01, 256.01) - origin;
	while (true) {
		int type = intersect();
		if (type == 0)
			break;
		
		if (kill(ray_depth)) {
			attenuation = vec3(0, 0, 0);
			break;
		}
		
		vec3	color = texelFetch(blockColors, type & 0xFF, 0).bgr;
		color /= 255.0;
		vec3	emit = texelFetch(blockEmittance, type & 0xFF, 0).bgr;
		emit /= 255.0;
		emittance += attenuation * emit * 2;
		attenuation *= color;
		
		ray_depth += 1;
		sky = 0.0;
		
		reflect_diffuse();
	}
	
	attenuation += emittance;
	attenuation = attenuation * (1-sky);
	float m = max(dot(d, vec3(0.0, 1.0, 0.0))+0.2, 0.0)/1.2;
	attenuation += sky * (vec3(0.9, 1.0, 0.05)*alpha +
	(1-alpha)*(
		vec3(0.1, 0.37, 0.57) * m +
		vec3(0.80, 0.87, 0.99) * (1-m)));
	return attenuation;
}

#undef BLUR
#undef RESET

void main()
{
	vec2	texcoord = gl_FragCoord.xy * invRes;
	ivec2	tc = ivec2(texcoord * textureSize(prevSampleCount, 0));
	vec3	psamp = texelFetch(prevSamples, tc, 0).rgb;
#ifdef BLUR
	uint	pcount = uint(min(255, texelFetch(prevSampleCount, tc, 0).r + 1));
#else
	uint	pcount = texelFetch(prevSampleCount, tc, 0).r;
#endif
#ifdef RESET
	pcount = 0;
#endif
	uint	ncount = min(255u, pcount + 1u);
	sampleCount = ncount;
	
	if (ncount == pcount) {
		sample.rgb = psamp;
	} else {
		vec3	light = pt();
		
		sample.rgb = psamp * (pcount/float(ncount)) + (light * (1.0 /float(ncount)));
	}
	
	sample.a = 1.0;
}
