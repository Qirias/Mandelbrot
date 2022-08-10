#version 430
const int TILE_W      = 32;
const int TILE_H      = 32;
const ivec2 TILE_SIZE = ivec2(TILE_W, TILE_H);
layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform float c_x, c_y, c_z, c_i;
double zreal = 0.0, zimaginary = 0.0;

// clang-format off
const vec3 color_map[] = {
    vec3(0.0, 0.0, 0.0),
    vec3(0.23, 0.11, 0.05),
    vec3(0.1,  0.03, 0.1), 
    vec3(0.04, 0.0,  0.18),
    vec3(0.02, 0.02, 0.29),
    vec3(0.0,  0.03, 0.39),
    vec3(0.05, 0.17, 0.54),
    vec3(0.09, 0.32, 0.69),
    vec3(0.22, 0.49, 0.82),
    vec3(0.52, 0.71, 0.9),
    vec3(0.82, 0.92, 0.97),
    vec3(0.94, 0.91, 0.75),
    vec3(0.97, 0.79, 0.37),
    vec3(1.0,  0.67, 0.0),
    vec3(0.8,  0.5,  0.0),
    vec3(0.6,  0.34, 0.0),
    vec3(0.41, 0.2,  0.01)
};
// clang-format on

vec3 getColor(double col)
{
    double r, g, b;

    int clr1 = int(col);
    double t2 = col - clr1;
    double t1 = 1 - t2;
    clr1 = clr1 % 17;
    int clr2 = (clr1 + 1) % 17;

    r = (color_map[clr1].x * t1 + color_map[clr2].x * t2);
    g = (color_map[clr1].y * t1 + color_map[clr2].y * t2);
    b = (color_map[clr1].z * t1 + color_map[clr2].z * t2);

    return vec3(r, g, b);
}

void main()
{
    vec4 pixel;
    
    // Compute global x, y coordinates utilizing local group ID
    const ivec2 tile_xy      = ivec2(gl_WorkGroupID);
    const ivec2 thread_xy    = ivec2(gl_LocalInvocationID);
    const ivec2 pixel_coords = tile_xy * TILE_SIZE + thread_xy;
    // ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    int maxIt = int(c_i);

    double x = pixel_coords.x / 1024.0lf * c_z + c_x;
    double y = pixel_coords.y / 1024.0lf * c_z + c_y;

    int it = 0;
    while (it < maxIt && zreal * zreal + zimaginary * zimaginary < 4.0) {
        double xtemp = zreal * zreal - zimaginary * zimaginary + x;
        zimaginary   = 2.0lf * zreal * zimaginary + y;
        zreal        = xtemp;
        it           = it + 1;
    }

    double color = it + 1 - (log(log(float(zreal) * float(zreal) + float(zimaginary) * float(zimaginary))) / log(2));

    vec3 col = getColor(color / maxIt * 17);

    imageStore(img_output, pixel_coords, vec4(col, 1.0));
}
