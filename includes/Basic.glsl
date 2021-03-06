#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords   = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#shader fragment
#version 330 core

in vec2 TexCoords;
out vec4 color;
uniform sampler2D mandelbrotTexture;

void main()
{
    vec3 col     = texture(mandelbrotTexture, TexCoords).rgb;
    color = vec4(col, 1.0);
}