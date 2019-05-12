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

void SCO(bool inSCOFocus){
  if(inSCOFocus){
    VertexSCO = (view * TG * vec4(vertex, 1.)).xyz;
    mat3 NormalMatrix = inverse (transpose (mat3 (view * TG)));
    NormalSCO = normalize(NormalMatrix * normal);
  }
  else{
    VertexSCO = vertex;
    NormalSCO = normal;
  }
}

void main()
{	
  SCO(findex==0);
  f_matamb = matamb;
  f_matdiff = matdiff;
  f_matspec = matspec;
  f_matshin = matshin;

  switch(findex%3){
    case 1: // Patricio
      f_posFocus =  vec3(4.,3.5,4.);
      break;

    case 2: // Arbre
      f_posFocus =  vec3(1.,3.5,1.);
      break;

    default: // Observer
      f_posFocus = posFocus;
      break;
  }

  // Proporcionat a l'exercici:
  // fcolor = matdiff;
  gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
