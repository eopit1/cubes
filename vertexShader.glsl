#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;   

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat3 inverseTransposeMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
    vec4 worldPos   = matModel * vec4(position, 1.0);
    fragPos         = vec3(worldPos);
    fragNormal      = normalize(inverseTransposeMatrix * normal);
    fragTexCoord    = texCoord;

    gl_Position = matProjection * matView * worldPos;
}
