#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3  lightPosition;
uniform vec3  lightDirection;  
uniform vec3  lightColor;       
uniform vec3  cameraPosition;
uniform vec3  objectColor;      
uniform int   lightEnabled;
uniform int   isSun;            
uniform sampler2D sunTexture;

out vec4 fragColor;

void main()
{
    if (isSun == 1) {
        fragColor = texture(sunTexture, fragTexCoord);
        return;
    }

    vec3 ambient = 0.15 * objectColor;

    vec3 result = ambient;

    if (lightEnabled == 1) {
        vec3  norm     = normalize(fragNormal);
        vec3  lightDir = normalize(lightPosition - fragPos);  // point light
        float diff     = max(dot(norm, lightDir), 0.0);
        vec3  diffuse  = diff * lightColor * objectColor;
        vec3  viewDir   = normalize(cameraPosition - fragPos);
        vec3  reflectDir= reflect(-lightDir, norm);
        float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3  specular  = 0.4 * spec * lightColor;

        result = ambient + diffuse + specular;
    }

    fragColor = vec4(result, 1.0);
}
