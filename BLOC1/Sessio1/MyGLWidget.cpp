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

  for(int i = 0; i < 2; i++){ //Pinta la mateixa escena en dos ViewPorts diferents:
    // La configuració del viewPort, tenint en compte que la coordenada {0,0} s'ubica al marge
    // inferior esquerra de la pantalla, funciona de la seguent manera:
    //  1r i 2n Params: posicions X i Y respectivament on comença el viewPort.
    //  3r i 4t Params: Extensió X i Y respectivament del viewport respecte les coordenades dels primers dos parametres.
    if(!i) glViewport (0, 0, width()/2, height()/2);
    else glViewport (width()/2, height()/2, width()/2, height()/2);

    // Activem l'Array a pintar 
    glBindVertexArray(VAO1);
   
    // Pintem l'escena
    int nTriangles = 3;
    glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);

    // Activem l'Array a pintar 
    glBindVertexArray(VAO2);
    nTriangles = 1;
    glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);
  }
  
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
  int nTriangles = 3;
  glm::vec3 Caseta[nTriangles*3];
  // Organitzats segons la regla de la ma dreta:
  Caseta[0] = glm::vec3(-1.0, 0.0, 0.0);
  Caseta[1] = glm::vec3(1.0, 0.0, 0.0);
  Caseta[2] = glm::vec3(0.0, 1.0, 0.0);

  Caseta[3] = glm::vec3(-0.6, 0.0, 0.0);
  Caseta[4] = glm::vec3(-0.6, -1.0, 0.0);
  Caseta[5] = glm::vec3(0.6, 0.0, 0.0);

  Caseta[6] = glm::vec3(-0.6, -1.0, 0.0);
  Caseta[7] = glm::vec3(0.6, -1.0, 0.0);
  Caseta[8] = glm::vec3(0.6, 0.0, 0.0);

  // Creació del Vertex Array Object (VAO) que usarem per pintar:
  // tots el VBOs creats a continuació i fins al tancament d'aquest VAO
  // estan associats automaticament a aquest VAO.
  // [!] EL VAO HA DE SER GLOBAL!
  glGenVertexArrays(1, &VAO1); //Tants canals com dades diferents a enmagatzemar.
  glBindVertexArray(VAO1); //Inicialitza l'objecte

  // Creació del Vertex Buffer Object (VBO) amb les dades dels vèrtexs:
  // [!] EL VBO HA DE SER LOCAL (és un medi, no l'objecte en si).
  GLuint VBO1;
  glGenBuffers(1, &VBO1); //Tants canals com dades es vulguin enmmagatzemar (ex.: vertex, color, etc.)
  glBindBuffer(GL_ARRAY_BUFFER, VBO1); //Inicialització l'objecte.
  //S'imprimeix el vector de vertexs creat al VBO.
  glBufferData(GL_ARRAY_BUFFER, sizeof(Caseta), Caseta, GL_STATIC_DRAW);

  // Activem l'atribut que farem servir per vèrtex
  // Transferencia de la info al Vertex Shader per a que puguin ser pintats:	
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0); //Activació de l'objecte vertexLoc associat al Shader corresponent.
  glEnableVertexAttribArray(vertexLoc); //Les dades enmagatzemades pel VBO passen a vertexLoc

  //////////////////////////////////////////////////////////////////////////
  // Un cop s'ha acabat de treballar amb un VAO és pot procedir amb l'altre:

  glm::vec3 Triangle[3];
  Triangle[0] = glm::vec3(1., 1., 0.);
  Triangle[1] = glm::vec3(0.6, 1., 0.);
  Triangle[2] = glm::vec3(1., 0.6, 0.);

  glGenVertexArrays(1, &VAO2); //Tants canals com dades diferents a enmagatzemar.
  glBindVertexArray(VAO2); //Inicialitza l'objecte

  GLuint VBO2;
  glGenBuffers(1, &VBO2); //Tants canals com dades es vulguin enmmagatzemar (ex.: vertex, color, etc.)
  glBindBuffer(GL_ARRAY_BUFFER, VBO2); //Inicialització l'objecte.
  //S'imprimeix el vector de vertexs creat al VBO.
  glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0); //Activació de l'objecte vertexLoc associat al Shader corresponent.
  glEnableVertexAttribArray(vertexLoc); //Les dades enmagatzemades pel VBO passen a vertexLoc

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

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
}
