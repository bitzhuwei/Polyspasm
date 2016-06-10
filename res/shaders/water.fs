#version 130

uniform sampler2D	sky_front;
uniform sampler2D	sky_back;
uniform sampler2D	sky_left;
uniform sampler2D	sky_right;
uniform sampler2D	sky_up;
uniform sampler2D	sky_down;
uniform sampler2D	depthFront;
uniform sampler2D	depthBack;

uniform float		alpha;
uniform vec2		invRes;

in vec3				worldPos;
in vec3				pos;
in vec3				norm;

out vec4			fragColor;

const float			water_ior = 1.333;
const float			air_ior = 1.000293;
const float			eta = air_ior / water_ior;
const float			near = 0.1;
const float			far =  5.0;

uniform usampler1D	perm;
uniform usampler1D	perm12;
uniform vec2		offset;

const vec2 grad[12] = vec2[12](
	vec2(1,1),vec2(-1,1),vec2(1,-1),vec2(-1,-1),
    vec2(1,0),vec2(-1,0),vec2(1,0),vec2(-1,0),
    vec2(0,1),vec2(0,-1),vec2(0,1),vec2(0,-1)
);

const vec2 FG = vec2( 0.5*(sqrt(3.0)-1.0), (3.0-sqrt(3.0))/6.0 );

int permutation(ivec2 id)
{
	int	p0 = int(texelFetch(perm, id.y, 0).x);
	return int(texelFetch(perm12, id.x + p0, 0).x);
}

/* 2D simplex noise (improved perlin noise)
 * Based on article and code by Stefan Gustavson:
 * http://www.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
 */
float simplex(vec2 x)
{
	float	u = dot(x, FG.xx);
	ivec2	i = ivec2(floor(x + u));
	float	v = dot(i, FG.yy);
	vec2	x0 = x - (i-v);
	ivec2	i1;

	if (x0.x>x0.y)
		i1 = ivec2(1, 0);
	else
		i1 = ivec2(0, 1);

	vec2	x1 = x0 - i1 + FG.yy;
	vec2	x2 = x0 + vec2(FG.y * 2.0 - 1.0);

	ivec2	id = ivec2(i % 256);
	ivec3	gi = ivec3(permutation(id), permutation(id + i1),
		permutation(id + ivec2(1)));

	vec3	n;
	vec3	t = vec3(0.5) - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2));
	if (t.x < 0.0) {
		n.x = 0.0;
	} else {
		t.x *= t.x;
		n.x = t.x * t.x * dot(grad[gi.x], x0);
	}
	if (t.y < 0.0) {
		n.y = 0.0;
	} else {
		t.y *= t.y;
		n.y = t.y * t.y * dot(grad[gi.y], x1);
	}
	if (t.z < 0.0) {
		n.z = 0.0;
	} else {
		t.z *= t.z;
		n.z = t.z * t.z * dot(grad[gi.z], x2);
	}

	return (n.x+n.y+n.z) * 35.0 + 0.5;
}

void main()
{
#if 0
	vec3	d = normalize(pos);
	vec3	n = normalize(norm);
	vec3	r = refract(d, -n, eta);
#else
	const float	scale = 10.7;
	const float	delta = 0.005;
	const float	magnitude = 0.2;// wave magnitude
	
	vec2	x0 = offset + scale * worldPos.xy;
	vec2	x1 = offset + scale * (worldPos.xy + vec2(delta, 0.0));
	vec2	y1 = offset + scale * (worldPos.xy + vec2(0.0, delta));
	
	vec3	w = normalize(norm);
	vec3	u;
	vec3	v;

	if (abs(w.y) > 0.9) {
		u = vec3(1.0, 0.0, 0.0);
	} else {
		u = vec3(0.0, 1.0, 0.0);
	}

	v = normalize(cross(w, u));
	u = cross(v, w);
	
	float	dx = simplex(x1) - simplex(x0);
	float	dy = simplex(y1) - simplex(x0);
	vec3	r = normalize(pos + magnitude * (v*dx + u*dy));
#endif

	const float	rf = 0.5;
	
	float		mx = 0.0;
	vec2		uv = vec2(0.0);
	
	if (r.x > mx) {
		mx = r.x;
		uv = vec2(-r.z, -r.y);
		fragColor = texture(sky_right, vec2(rf) - rf * uv / mx);
	}
	
	if (-r.x > mx) {
		mx = -r.x;
		uv = vec2(r.z, -r.y);
		fragColor = texture(sky_left, vec2(rf) - rf * uv / mx);
	}
	
	if (r.y > mx) {
		mx = r.y;
		uv = vec2(-r.x, -r.z);
		fragColor = texture(sky_up, vec2(rf) - rf * uv / mx);
	}
	
	if (-r.y > mx) {
		mx = -r.y;
		uv = vec2(-r.x, r.z);
		fragColor = texture(sky_down, vec2(rf) - rf * uv / mx);
	}
	
	if (r.z > mx) {
		mx = r.z;
		uv = vec2(r.x, -r.y);
		fragColor = texture(sky_back, vec2(rf) - rf * uv / mx);
	}
	
	if (-r.z > mx) {
		mx = -r.z;
		uv = vec2(-r.x, -r.y);
		fragColor = texture(sky_front, vec2(rf) - rf * uv / mx);
	}
	
	float		front = (2.0 * near) / (far + near - texture(depthFront, gl_FragCoord.xy * invRes).r * (far-near));
	float		back = (2.0 * near) / (far + near - texture(depthBack, gl_FragCoord.xy * invRes).r * (far-near));
	float		m = 0.45 + (back - front)*7.0;
	fragColor = mix(fragColor*1.2, vec4(0.26, 0.33, 1.0, 1.0), m);
	fragColor.a = alpha;
}
