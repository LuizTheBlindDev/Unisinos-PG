//C�digo fonte do Fragment Shader (em GLSL): ainda hardcoded
#version 450

in vec3 finalColor;
in vec3 fragPos;
in vec3 scaledNormal;
in vec2 tex;

out vec4 color;

//Propriedades da superficie
uniform float ka;
uniform float kd;
uniform float ks;
uniform float q;
uniform sampler2D sp;

//Propriedades da fonte de luz
uniform vec3 lightPos;
uniform vec3 lightColor;

//Posi��o da Camera
uniform vec3 cameraPos;

void main()
{
	//C�lculo da parcela de ilumina��o ambiente
	vec3 ambient = ka * lightColor;

	//C�lculo da parcela de ilumina��o difusa
	vec3 N = normalize(scaledNormal);
	vec3 L = normalize(lightPos - fragPos);
	float diff = max(dot(N,L),0.0);
	vec3 diffuse = kd * diff * lightColor;

	//C�lculo da parcela de ilumina��o especular
	vec3 V = normalize(cameraPos - fragPos);
	vec3 R = normalize(reflect(-L,N));
	float spec = max(dot(R,V),0.0);
	spec = pow(spec,q);
	vec3 specular = ks * spec * lightColor;

	vec4 objColor = texture(sp, tex);
	vec3 result = (ambient + diffuse) * vec3( objColor )+ specular;

	color = vec4(result,1.0);
}