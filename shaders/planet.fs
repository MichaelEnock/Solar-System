
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec3 viewPos;

// Directional light
uniform vec3 dirLightDir;
uniform vec3 dirLightColor;

// Point light
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //ambient
	float ambientStrength = 0.19f;
    vec3 ambient = ambientStrength * (dirLightColor + pointLightColor);

    // ----- Directional Light (Yellow) -----
    vec3 dirLight = normalize(-dirLightDir);
    float dirDiff = max(dot(norm, dirLight), 0.0);
    vec3 dirDiffuse = dirDiff * dirLightColor;

    vec3 dirReflect = reflect(-dirLight, norm);
    float dirSpec = pow(max(dot(viewDir, dirReflect), 0.0), 32);
    vec3 dirSpecular = 0.5 * dirSpec * dirLightColor;

    // ----- Point Light (Blue) -----
    vec3 pointLightDir = normalize(pointLightPos - FragPos);
    float distance = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);

    vec3 pointAmbient = 0.05 * pointLightColor;
    float pointDiff = max(dot(norm, pointLightDir), 0.0);
    vec3 pointDiffuse = pointDiff * pointLightColor;

    vec3 pointReflect = reflect(-pointLightDir, norm);
    float pointSpec = pow(max(dot(viewDir, pointReflect), 0.0), 32);
    vec3 pointSpecular = 0.5 * pointSpec * pointLightColor;

    pointAmbient *= attenuation;
    pointDiffuse *= attenuation;
    pointSpecular *= attenuation;

    vec3 objectColor = texture(texture1, TexCoord).rgb;
    vec3 lighting = (ambient + dirDiffuse + dirSpecular + pointAmbient + pointDiffuse + pointSpecular);
    vec3 result = lighting * objectColor;
    
   FragColor = vec4(result, 1.0f);

  // FragColor = vec4(pointDiffuse, 1.0);
   // FragColor = vec4(dirDiffuse, 1.0);
}























/*#version 330 core
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
}*/