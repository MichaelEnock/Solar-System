#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
// texture samplers
uniform sampler2D texture1;
uniform vec3 viewPos;

vec3 lightPos = vec3(0.0f);
vec3 lightColor = vec3(1.0f);

void main()
{

	//ambient
	float ambientStrength = 0.25f;
    vec3 ambient = ambientStrength * lightColor;


	// diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	// specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
	
	// result
    vec3 objectColor = texture(texture1, TexCoord).xyz;
    vec3 result = (ambient + diffuse) * objectColor;
	FragColor = vec4(result, 1.0f);
}