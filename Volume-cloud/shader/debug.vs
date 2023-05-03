#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoords;

out vec2 TexCoords;

void main(){
    TexCoords = aTexcoords;
    gl_Position = vec4(aPos,1.0);
}