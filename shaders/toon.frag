#version 330 core

out vec4 fragColor;

in vec2 texCoords;
in vec3 Normal;
in vec3 Vert_World_Pos;
uniform vec3 View_Pos;

uniform sampler2D texture_diffuse1;
struct DirectionalLight {
    vec3 Dir;
    vec3 Color;
};

uniform DirectionalLight dirLight;

struct AmbientLight {
    float Intensity;
    vec3 Color;
};

uniform AmbientLight ambient_light;

vec4 ambient()
{
    return vec4(ambient_light.Intensity * ambient_light.Color, 1.f);
}

vec4 diffuseLight()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.Dir);
    float cosValue = max(dot(normal, lightDir),0.0f);
    float intensity = smoothstep(0, 0.01, cosValue);
    return vec4(intensity * dirLight.Color,1.0f);
}

vec4 specular_light()
{
    float specularStrength = 1f;
    float shininess = 64.0f;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.Dir);
    vec3 view_dir = normalize(View_Pos - Vert_World_Pos);
    
    vec3 halfVector = normalize(lightDir + view_dir);
    float NdotH = max(dot(normal, halfVector),0.0f);
    
    float spec = pow(NdotH, shininess * shininess);
    
    float smooth_factor = 0.005;
    float intensity = smoothstep(0.5, 0.5 + smooth_factor, spec);
    
    return vec4(specularStrength * intensity * dirLight.Color, 1.f);
}

void main()
{
    vec4 baseColor = texture(texture_diffuse1, texCoords);
    vec4 ambient_comp = ambient();
    vec4 diffuse_comp = diffuseLight();
    vec4 specular_comp = specular_light(); 
    fragColor = baseColor * (diffuse_comp + ambient_comp + specular_comp); 
}