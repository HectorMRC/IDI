#version 330 core

in vec4 vertexColor;
out vec4 FragColor;

//Exercicis 2.1 i 2.2:
vec4 pintar_per_quadrants(){
	// [!] L'element gl_FragCoord proporciona les coordenades del fragment que s'està processant.
	if((int(gl_FragCoord.y)/10)%2 != 0) discard; //Es descarten certs fragments
	else if(gl_FragCoord.x < 357.){ //si es troba a la primera meitat del viewPort:
		if(gl_FragCoord.y < 357.) return vec4(1., 1., 0., 1);
		else return vec4(1., 0., 0., 1);
	}
	else{ //si es troba a la segona meitat del viewPort:
		if(gl_FragCoord.y < 357.) return vec4(0., 1., 0., 1.);
		else return vec4(0., 0., 1., 1.);
	}
}

void main() {
    FragColor = vertexColor;
    //FragColor = pintar_per_quadrants();
}

