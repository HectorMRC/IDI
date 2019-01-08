#version 330 core

in vec3 fcolor;

// Per a l'exercici 2 de la sessió 9:
in vec3 VertexSCO;
in vec3 NormalSCO;
in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform vec3 posFocus; //Posició del focus
uniform vec3 colFocus; //Color del focus
uniform vec3 llumAmbient;

out vec4 FragColor;

vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matamb;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * matdiff * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO) 
{
    // Els vectors estan normalitzats

    // Inicialitzem color a Lambert
    vec3 colRes = Lambert (NormSCO, L);

    // Calculem R i V
    if (dot(NormSCO,L) < 0)
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshin == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshin);
    return (colRes + matspec * colFocus * shine); 
}

void main()
{	
	vec3 llumSCO = normalize(posFocus - VertexSCO); // Assumim que posFocus ve donat amb el format SCO
	FragColor = Lambert(NormalSCO, llumSCO);	
}
