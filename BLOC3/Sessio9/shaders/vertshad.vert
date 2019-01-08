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

//uniform vec3 posFocus; //Posició del focus
//uniform vec3 colFocus; //Color del focus
//uniform vec3 llumAmbient;

//Valors per als components que necessitem dels focus de llum
//vec3 colFocus = vec3(0.8, 0.8, 0.8);
//vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
//vec3 posFocus = vec3(1, 0, 1);  // en SCA

out vec3 fcolor;

// Per a l'exercici 2 de la sessió 9:
out vec3 VertexSCO;
out vec3 NormalSCO;
out vec3 matamb;
out vec3 matdiff;
out vec3 matspec;
out float matshin;

void main()
{	
    VertexSCO = (view * TG * vec4(vertex, 1.)).xyz;

    mat3 NormalMatrix = inverse (transpose (mat3 (view * TG)));
    NormalSCO = normalize(NormalMatrix * normal);

    fcolor = matdiff;
    gl_Position = proj * view * TG * vec4 (vertex, 1.0);

    /*Resposta a l'exercici 2 de la sessió 8:
        La iluminació oferta pel mètode Phong atorga l'efecte de textura a l'escena (per exemple:
        als ulls del patricio s'hi aprecia una certa reflexio); mentre que en el cas del mètode
        Lamber el color es pla, no hi ha cap mena d'acabat en els colors iluminats.*/
}
