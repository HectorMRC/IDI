#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

// Valors per als components que necessitem dels focus de llum
uniform vec3 posFocus; //Posició del focus
uniform vec3 colFocus; //Color del focus
uniform vec3 llumAmbient;

uniform int findex;

// Saltem les dades al FragmentShader
out vec3 VertexSCO;
out vec3 NormalSCO;

out vec3 f_posFocus; //Posició del focus segons l'index
out vec3 f_matamb;
out vec3 f_matdiff;
out vec3 f_matspec;
out float f_matshin;

// out vec3 fcolor;

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
  // Afegit:
  VertexSCO = (view * TG * vec4(vertex, 1.)).xyz;
  mat3 NormalMatrix = inverse (transpose (mat3 (view * TG)));
  NormalSCO = normalize(NormalMatrix * normal);

  f_matamb = matamb;
  f_matdiff = matdiff;
  f_matspec = matspec;
  f_matshin = matshin;

  switch(findex%3){
    case 1: // Patricio
      f_posFocus = vec3(4.,3.5,4.);
      break;

    case 2: // Arbre
      f_posFocus = vec3(1.,3.5,1.);
      break;

    default: // Observer
      f_posFocus = posFocus;
      break;
  }

  // Proporcionat a l'exercici:
  // fcolor = matdiff;
  gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
