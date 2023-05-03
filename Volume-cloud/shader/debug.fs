#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D dubug;

void main(){
    float color = texture(dubug,TexCoords).r;
    FragColor = vec4(color,0.0,0.0,1.0);
    // FragColor = texture(dubug,TexCoords);

}