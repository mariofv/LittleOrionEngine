in vec2 frag_position;
out vec4 FragColor;

layout (std140) uniform Matrices
{
  mat4 model;
  mat4 proj;
	mat4 view;
} matrices;

struct Grid
{
  float grid_size;
  float cell_size;
  vec4 thin_color;
  vec4 thick_color;
};
uniform Grid grid;

float log10(float x)
{
  return (1 / log(10.0)) * log(x);
}

vec2 saturate(vec2 x)
{
  return clamp(x, 0.0, 1.0);
}

float saturate(float x)
{
  return clamp(x, 0.0, 1.0);
}

void main()
{
  // UV is grid space coordinate of pixel.
  vec2 uv = frag_position;

  // Find screen-space derivates of grid space. [1]
  vec2 dudv = vec2(
    length(vec2(dFdx(uv.x), dFdy(uv.x))),
    length(vec2(dFdx(uv.y), dFdy(uv.y)))
  );

  // Define minimum number of pixels between cell lines before LOD switch should occur.
  const float min_pixels_between_cells = 25.f;

  // Calc lod-level [2].
  float lod_level = max(0, log10((length(dudv) * min_pixels_between_cells) / grid.cell_size) + 1);
  float lod_fade = fract(lod_level);

  // Calc cell sizes for lod0, lod1 and lod2.
  float lod0_cs = grid.cell_size * pow(10, floor(lod_level));
  float lod1_cs = lod0_cs * 10.f;
  float lod2_cs = lod1_cs * 10.f;

  // Allow each anti-aliased line to cover up to 2 pixels.
  dudv *= 2;

  // Calculate unsigned distance to cell line center for each lod [3]
  vec2 lod0_cross_a = 1.f - abs(saturate(mod(uv, lod0_cs) / dudv) * 2 - 1.f);
  // Pick max of x,y to get a coverage alpha value for lod
  float lod0_a = max(lod0_cross_a.x, lod0_cross_a.y);
  vec2 lod1_cross_a = 1.f - abs(saturate(mod(uv, lod1_cs) / dudv) * 2 - 1.f);
  float lod1_a = max(lod1_cross_a.x, lod1_cross_a.y);
  vec2 lod2_cross_a = 1.f - abs(saturate(mod(uv, lod2_cs) / dudv) * 2 - 1.f);
  float lod2_a = max(lod2_cross_a.x, lod2_cross_a.y);

  // Blend between falloff colors to handle LOD transition [4]
  vec4 c = lod2_a > 0 ? grid.thick_color : lod1_a > 0 ? mix(grid.thick_color, grid.thin_color, lod_fade) : grid.thin_color;

  // Calculate opacity falloff based on distance to grid extents and gracing angle. [5]
  vec3 view_dir = vec3(matrices.view[0][2], matrices.view[1][2], matrices.view[2][2]);
  view_dir = normalize(view_dir);
  vec3 surface_vec = vec3(0.0, 1.0, 0.0);
  float op_gracing = 1.f - pow(1.f - abs(dot(view_dir, surface_vec)), 16);
  float op_distance = (1.f - saturate(length(uv) / grid.grid_size));
  float op = op_gracing * op_distance;

  // Blend between LOD level alphas and scale with opacity falloff. [6]
  c.a *= (lod2_a > 0 ? lod2_a : lod1_a > 0 ? lod1_a : (lod0_a * (1-lod_fade))) * op;

	FragColor =  c;
	FragColor.a *= 0.5;
}
