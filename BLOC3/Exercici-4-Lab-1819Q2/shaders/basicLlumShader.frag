#version 330 core

//in vec3 fcolor;

in vec3 VertexSCO;
in vec3 NormalSCO;
in vec3 f_matamb;
in vec3 f_matdiff;
in vec3 f_matspec;
in float f_matshin;

in vec3 f_posFocus; //Posició del focus segons l'index
uniform vec3 colFocus; //Color del focus
uniform vec3 llumAmbient;

out vec4 FragColor;

vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * f_matamb;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * f_matdiff * dot (L, NormSCO);
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

    if ((dot(R, V) < 0) || (f_matshin == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), f_matshin);
    return (colRes + f_matspec * colFocus * shine); 
}

void main()
{	
	vec3 llumSCO = normalize(f_posFocus - VertexSCO); // Assumim que f_posFocus ve donat amb el format SCO
  vec3 fcolor = Phong (NormalSCO, llumSCO, vec4(VertexSCO, 1.));
	FragColor = vec4(fcolor,1);	

	/*	La iluminació oferta pel mètode Phong atorga l'efecte de textura a l'escena (per exemple:
		als ulls del patricio s'hi aprecia una certa reflexio); mentre que en el cas del mètode
		Lamber el color es pla, no hi ha cap mena d'acabat en els colors iluminats.
	*/
}
