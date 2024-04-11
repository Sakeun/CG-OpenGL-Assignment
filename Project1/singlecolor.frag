#version 430 core

uniform vec3 FragCol;

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;

out vec4 fragColor;

void main()
{
    fragColor = vec4(FragCol, 1.0);
}