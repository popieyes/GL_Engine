#version 330 core
out vec4 fragColor;

in vec2 texCoords;
in vec3 Normal;
in vec3 Vert_World_Pos;
uniform vec3 View_Pos;

uniform vec4 material_color;
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
    float NdotL = max(dot(normal, lightDir),0.0f);
    float intensity = smoothstep(0, 0.01, NdotL);
    return vec4(intensity * dirLight.Color,1.0f);
}

vec4 specular_light()
{
    float specularStrength = 1.0f;
    float shininess = 16.0f;
    
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

vec4 rim_light()
{
    float rim_amount = 0.7f;
    float rim_threshold = 0.1;
    vec3 normal = normalize(Normal);
    vec3 light_dir = normalize(-dirLight.Dir);
    float NdotL = max(dot(normal, light_dir),0.0f);
    vec3 view_dir = normalize(View_Pos - Vert_World_Pos);
    float rim_dot = 1 - dot(view_dir, normal);
    float rim_intensity = rim_dot * pow(NdotL, rim_threshold);
    rim_intensity = smoothstep(rim_amount - 0.01, rim_amount + 0.01, rim_intensity);
    return vec4(rim_intensity * vec4(1.0f,1.0f,1.0f,1.0f));
}

void main()
{
    vec4 baseColor = material_color * texture(texture_diffuse1, texCoords);
    vec4 ambient_comp = ambient();
    vec4 diffuse_comp = diffuseLight();
    vec4 specular_comp = specular_light(); 
    vec4 rim_comp = rim_light();
    fragColor = baseColor * (diffuse_comp + ambient_comp + specular_comp + rim_comp); 
}