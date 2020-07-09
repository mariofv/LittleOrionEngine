/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define perlin_B 0x100
#define perlin_BM 0xff

#define perlin_N 0x1000
#define perlin_NP 12   /* 2^N */
#define perlin_NM 0xfff

static int perlin_p[perlin_B + perlin_B + 2];
static float perlin_g3[perlin_B + perlin_B + 2][3];
static float perlin_g2[perlin_B + perlin_B + 2][2];
static float perlin_g1[perlin_B + perlin_B + 2];
static int perlin_start = 1;

static void perlin_init(void);

#define perlin_s_curve(t) ( t * t * (3. - 2. * t) )

#define perlin_lerp(t, a, b) ( a + t * (b - a) )

#define perlin_setup(i,b0,b1,r0,r1)\
	t = vec[i] + perlin_N;\
	b0 = ((int)t) & perlin_BM;\
	b1 = (b0+1) & perlin_BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

double noise1(double arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (perlin_start) {
		perlin_start = 0;
		perlin_init();
	}

	perlin_setup(0, bx0, bx1, rx0, rx1);

	sx = perlin_s_curve(rx0);

	u = rx0 * perlin_g1[perlin_p[bx0]];
	v = rx1 * perlin_g1[perlin_p[bx1]];

	return perlin_lerp(sx, u, v);
}

float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register int i, j;

	if (perlin_start) {
		perlin_start = 0;
		perlin_init();
	}

	perlin_setup(0, bx0, bx1, rx0, rx1);
	perlin_setup(1, by0, by1, ry0, ry1);

	i = perlin_p[bx0];
	j = perlin_p[bx1];

	b00 = perlin_p[i + by0];
	b10 = perlin_p[j + by0];
	b01 = perlin_p[i + by1];
	b11 = perlin_p[j + by1];

	sx = perlin_s_curve(rx0);
	sy = perlin_s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = perlin_g2[b00]; u = at2(rx0, ry0);
	q = perlin_g2[b10]; v = at2(rx1, ry0);
	a = perlin_lerp(sx, u, v);

	q = perlin_g2[b01]; u = at2(rx0, ry1);
	q = perlin_g2[b11]; v = at2(rx1, ry1);
	b = perlin_lerp(sx, u, v);

	return perlin_lerp(sy, a, b);
}

float noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	register int i, j;

	if (perlin_start) {
		perlin_start = 0;
		perlin_init();
	}

	perlin_setup(0, bx0, bx1, rx0, rx1);
	perlin_setup(1, by0, by1, ry0, ry1);
	perlin_setup(2, bz0, bz1, rz0, rz1);

	i = perlin_p[bx0];
	j = perlin_p[bx1];

	b00 = perlin_p[i + by0];
	b10 = perlin_p[j + by0];
	b01 = perlin_p[i + by1];
	b11 = perlin_p[j + by1];

	t = perlin_s_curve(rx0);
	sy = perlin_s_curve(ry0);
	sz = perlin_s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = perlin_g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
	q = perlin_g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
	a = perlin_lerp(t, u, v);

	q = perlin_g3[b01 + bz0]; u = at3(rx0, ry1, rz0);
	q = perlin_g3[b11 + bz0]; v = at3(rx1, ry1, rz0);
	b = perlin_lerp(t, u, v);

	c = perlin_lerp(sy, a, b);

	q = perlin_g3[b00 + bz1]; u = at3(rx0, ry0, rz1);
	q = perlin_g3[b10 + bz1]; v = at3(rx1, ry0, rz1);
	a = perlin_lerp(t, u, v);

	q = perlin_g3[b01 + bz1]; u = at3(rx0, ry1, rz1);
	q = perlin_g3[b11 + bz1]; v = at3(rx1, ry1, rz1);
	b = perlin_lerp(t, u, v);

	d = perlin_lerp(sy, a, b);

	return perlin_lerp(sz, c, d);
}

static void normalize2(float v[2])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(float v[3])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

static void perlin_init(void)
{
	int i, j, k;

	for (i = 0; i < perlin_B; i++) {
		perlin_p[i] = i;

		perlin_g1[i] = (float)((rand() % (perlin_B + perlin_B)) - perlin_B) / perlin_B;

		for (j = 0; j < 2; j++)
			perlin_g2[i][j] = (float)((rand() % (perlin_B + perlin_B)) - perlin_B) / perlin_B;
		normalize2(perlin_g2[i]);

		for (j = 0; j < 3; j++)
			perlin_g3[i][j] = (float)((rand() % (perlin_B + perlin_B)) - perlin_B) / perlin_B;
		normalize3(perlin_g3[i]);
	}

	while (--i) {
		k = perlin_p[i];
		perlin_p[i] = perlin_p[j = rand() % perlin_B];
		perlin_p[j] = k;
	}

	for (i = 0; i < perlin_B + 2; i++) {
		perlin_p[perlin_B + i] = perlin_p[i];
		perlin_g1[perlin_B + i] = perlin_g1[i];
		for (j = 0; j < 2; j++)
			perlin_g2[perlin_B + i][j] = perlin_g2[i][j];
		for (j = 0; j < 3; j++)
			perlin_g3[perlin_B + i][j] = perlin_g3[i][j];
	}
}