#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 tilt;
//uniform mat4 align;
//uniform mat4 scale;
//uniform mat4 translate;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
   TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
