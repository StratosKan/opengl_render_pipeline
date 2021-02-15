#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    float intensity;
	
    float constant;
	float linear;
	float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;

#define NR_LIGHTS 2
uniform Light lights[NR_LIGHTS];

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result;
	
    for(int i = 0; i < NR_LIGHTS; i++)
	{	 
	// ambient
    vec3 ambient = lights[i].ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(lights[i].position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lights[i].diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular 
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);            //shininess change for rough effect 1= shiny 0 = rough
    vec3 specular = lights[i].specular * spec * texture(material.specular, TexCoords).rgb;  
	
	// attenuation (a.k.a. fall-off)
    float distance    = length(lights[i].position - FragPos);
	float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + 
  			     lights[i].quadratic * (distance * distance));
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
        
    result += ambient + diffuse + specular;
	result *= lights[i].intensity;
    }
    FragColor = vec4(result, 1.0);
}
