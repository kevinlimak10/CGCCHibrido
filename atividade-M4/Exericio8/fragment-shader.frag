#version 410

// Variáveis uniformes do shader. 
uniform vec3 lightColor;
uniform vec3 lightPosition;

// Coeficiente de reflexão
uniform vec3 ka;
// Coeficiente de reflexão difusa
uniform vec3 kd;
// Coeficiente de reflexão especular
uniform vec3 ks;
// Expoente de reflexão especular
uniform float q;

// Variáveis de entrada do shader
in vec3 finalColor;
in vec3 scaledNormal;
in vec2 textureCoord;
in vec3 fragmentPosition;

// Variáveis auxiliares
uniform vec3 cameraPos;
uniform sampler2D tex_buffer;

// Variável de saída do shader
out vec4 color;

void main()
{
	
	// Calcular iluminação difusa
	vec3 Light = normalize(lightPosition - fragmentPosition);
	vec3 Normal = normalize(scaledNormal);
	float diff = max(dot(Normal,Light),0.0);
	vec3 diffuse = kd * diff * lightColor;

	vec3 Refl = normalize(reflect(-Light,Normal));
	vec3 Vert = normalize(cameraPos - fragmentPosition);
	float sp = max(dot(Refl,Vert),0.0);
	vec3 specular = ks * pow(sp, q) * lightColor;

	// Calcular iluminação ambiente
	vec3 ambient = ka * lightColor;

	vec3 texColor = texture(tex_buffer, textureCoord).xyz;
	vec3 total = (ambient + diffuse) * texColor + specular;

	color = vec4(total, 1.0f);
}