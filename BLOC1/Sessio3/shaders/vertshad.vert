#version 330 core

//uniform float scale;
uniform mat4 transform;

in vec4 color;
in vec3 vertex;

out vec4 vertexColor; //La variable de sortida s'ha d'anomenar igual que la variable d'entrada del fragment shader.

void main()  {
    gl_Position = transform * vec4 (vertex/*scale*/, 1.0);
    vertexColor = color; //Cal assignar-li un valor, del contrari no sabrà que colocar-li i la variable quedarà indefinida.
    
    // Processat el vertex shader s'accedeix al fragment shader automaticament.
    // Les dades del fragment corresponent venen directament donades.
}
