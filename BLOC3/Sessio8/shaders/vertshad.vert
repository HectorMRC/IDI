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

uniform vec3 posFocus; //Posició del focus
uniform vec3 colFocus; //Color del focus
uniform vec3 llumAmbient;

// Valors per als components que necessitem dels focus de llum
//vec3 colFocus = vec3(0.8, 0.8, 0.8);
//vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
//vec3 posFocus = vec3(1, 0, 1);  // en SCA

out vec3 fcolor;

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

vec3 calcular_Lamber(){
    // Cal recordar que en matrius les multiplicacions s'han de fer en ordre invers!
    // Una matriu de 4x4 es pot multiplicar per un vector de 4 posicions, per tant cal assignar els vec3 com de 4, i despres fer la conversió inversa.
    vec3 VertexSCO = (view * TG * vec4(vertex, 1.)).xyz;
    
    mat3 NormalMatrix = inverse (transpose (mat3 (view * TG)));
    vec3 NormalSCO = normalize(NormalMatrix * normal);
    
    vec3 focusSCO = (view * vec4(posFocus, 1.0)).xyz;
    vec3 llumSCO = normalize(focusSCO - VertexSCO);

    return Lambert(NormalSCO, llumSCO);
}

vec3 calcular_Phong(){
    // Cal recordar que en matrius les multiplicacions s'han de fer en ordre invers!
    // Una matriu de 4x4 es pot multiplicar per un vector de 4 posicions, per tant cal assignar els vec3 com de 4, i despres fer la conversió inversa.
    vec4 VertexSCO = view * TG * vec4(vertex, 1.);
    
    mat3 NormalMatrix = inverse (transpose (mat3 (view * TG)));
    vec3 NormalSCO = normalize(NormalMatrix * normal);
    
    vec3 focusSCO = (view * vec4(posFocus, 1.0)).xyz;
    vec3 llumSCO = normalize(focusSCO - VertexSCO.xyz);

    return Phong(NormalSCO, llumSCO, VertexSCO);
}

void main()
{	
    fcolor = calcular_Phong(); //calcular_Lamber(); //matdiff;
    gl_Position = proj * view * TG * vec4 (vertex, 1.0);

    /*Resposta a l'exercici 2:
        La iluminació oferta pel mètode Phong atorga l'efecte de textura a l'escena (per exemple:
        als ulls del patricio s'hi aprecia una certa reflexio); mentre que en el cas del mètode
        Lamber el color es pla, no hi ha cap mena d'acabat en els colors iluminats.*/
}
