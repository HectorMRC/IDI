#version 330 core

uniform mat4 transform;

in vec3 vertex;
in vec4 color;

out vec4 fcolor;  //La variable de sortida s'ha d'anomenar igual que la variable d'entrada del fragment shader.

void main()  {
    gl_Position = transform * vec4 (vertex/*scale*/, 1.0);
    fcolor = color; //Cal assignar-li un valor, del contrari no sabrà que colocar-li i la variable quedarà indefinida.
}
