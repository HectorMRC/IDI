#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  glViewport (0, 0, width(), height());

  // Activem l'Array a pintar 
  glBindVertexArray(VAO1);
  // Pintem l'escena
  int nTriangles = 1;
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  // Aquí anirà el codi que cal fer quan es redimensiona la finestra
}

void MyGLWidget::createBuffers ()
{
  // Array de vectors:
  // Tres vèrtexs amb X, Y i Z
  int nTriangles = 1;
  glm::vec3 Triangle[nTriangles*3];
  // Organitzats segons la regla de la ma dreta:
  Triangle[0] = glm::vec3(-1.0, -1.0, 0.0);
  Triangle[1] = glm::vec3(1.0, -1.0, 0.0);
  Triangle[2] = glm::vec3(0.0, 1.0, 0.0);

  glm::vec4 Colors[nTriangles*3];
  Colors[0] = glm::vec4(1., 0., 0., 1.);
  Colors[1] = glm::vec4(0., 1., 0., 1.);
  Colors[2] = glm::vec4(0., 0., 1., 1.);

  // Creació del Vertex Array Object (VAO) que usarem per pintar:
  // tots el VBOs creats a continuació i fins al tancament d'aquest VAO
  // estan associats automaticament a aquest VAO.
  // [!] EL VAO HA DE SER GLOBAL!
  glGenVertexArrays(1, &VAO1); //Tants canals com dades diferents a enmagatzemar.
  glBindVertexArray(VAO1); //Inicialitza l'objecte

  // Creació del Vertex Buffer Object (VBO) amb les dades dels vèrtexs:
  // [!] EL VBO HA DE SER LOCAL (és un medi, no l'objecte en si).
  GLuint VBO1[2];
  glGenBuffers(2, VBO1); //Tants canals com dades es vulguin enmmagatzemar (ex.: vertex, color, etc.)
  glBindBuffer(GL_ARRAY_BUFFER, VBO1[0]); //Inicialització l'objecte (I EL CANAL CORRESPONENT)
  //S'imprimeix el vector de vertexs creat al VBO.
  glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex
  // Transferencia de la info al Vertex Shader per a que puguin ser pintats:  
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0); //Activació de l'objecte vertexLoc associat al Shader corresponent.
  glEnableVertexAttribArray(vertexLoc); //Les dades enmagatzemades pel VBO passen a vertexLoc

  // Un cop s'ha fet tot el procediment d'un canal, inclosa l'assignació amb la variable corresponent del vertex shader,
  // es pot procedir amb l'altre canal:
  glBindBuffer(GL_ARRAY_BUFFER, VBO1[1]);
  // S'imprimeix el vector de vertexs creat al VBO.
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  // I es configura la info per a que passi per l'atribut del vertex shader corresponent:
  glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); //Activació de l'objecte colorLoc associat al Shader corresponent.
  glEnableVertexAttribArray(colorLoc); //Les dades enmagatzemades pel VBO passen a vertexLoc
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader:
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut color del vertex shader:
  colorLoc = glGetAttribLocation (program->programId(), "color");
}
