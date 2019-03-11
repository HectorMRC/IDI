//#include <GL/glew.h>
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
  creaBuffersArbre();
  anglegirArbre = 0.0;
}

void MyGLWidget::pintaArbre ()  
{
  // Activem l'Array a pintar 
  glBindVertexArray(VAO_Arbre);
  // Pintem
  int nTriangles = 4;
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);
}

void MyGLWidget::paintGL ()
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif

// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Pintem tots dos quadrats cadascú amb la seva transformació
  modelTransformArbre (anglegirArbre);
  pintaArbre ();
 
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::modelTransformArbre (float anglegir) {
  glm::mat4 TG (1.0); //Matriu de transformació

  TG = glm::translate(TG, glm::vec3(-0.5, 0, 0.));
  TG = glm::rotate(TG, anglegir, glm::vec3(0., 1., 0.)); //Si M_PI > 0 --> la rotació és en sentit antihorari!
  TG = glm::scale(TG, glm::vec3(2., 2., 0.));
  TG = glm::translate(TG, glm::vec3(0.5, 0, 0.));

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::resizeGL (int w, int h)
{
  ample = w;
  alt = h;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_R: { // increment angle rotació arbre respecte eix...
      anglegirArbre += 15*(M_PI/180);
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::creaBuffersArbre () {
  // Dades de les coordenades dels vèrtexs
  int nTriangles = 4;
  glm::vec3 Vertices[nTriangles*3];  
  Vertices[0] = glm::vec3(-0.55, -0.35, 0.0);  // tronc de l'arbre
  Vertices[1] = glm::vec3(-0.45, -0.35, 0.0);
  Vertices[2] = glm::vec3(-0.55, 0.05, 0.0);
  Vertices[3] = glm::vec3(-0.55, 0.05, 0.0);
  Vertices[4] = glm::vec3(-0.45, -0.35, 0.0);
  Vertices[5] = glm::vec3(-0.45, 0.05, 0.0);
  Vertices[6] = glm::vec3(-0.65, 0.05, 0.01);  // fulles de l'arbre
  Vertices[7] = glm::vec3(-0.35, 0.05, 0.01);
  Vertices[8] = glm::vec3(-0.5, 0.35, 0.01);
  Vertices[9] = glm::vec3(-0.35, 0.25, 0.01);
  Vertices[10] = glm::vec3(-0.65, 0.25, 0.01);
  Vertices[11] = glm::vec3(-0.5, -0.05, 0.01);

  // Dades dels colors dels vèrtexs
  glm::vec4 Colors[nTriangles*3];
  Colors[0] = glm::vec4(1.0, 0.6, 0.3, 1.0);  // tronc de l'arbre: color marró
  Colors[1] = glm::vec4(1.0, 0.6, 0.3, 1.0);
  Colors[2] = glm::vec4(1.0, 0.6, 0.3, 1.0);
  Colors[3] = glm::vec4(1.0, 0.6, 0.3, 1.0);
  Colors[4] = glm::vec4(1.0, 0.6, 0.3, 1.0);
  Colors[5] = glm::vec4(1.0, 0.6, 0.3, 1.0);
  Colors[6] = glm::vec4(0.0, 1.0, 0.0, 1.0);  // fulles de l'arbre: color verd
  Colors[7] = glm::vec4(0.0, 1.0, 0.0, 1.0);
  Colors[8] = glm::vec4(0.0, 1.0, 0.0, 1.0);
  Colors[9] = glm::vec4(0.0, 1.0, 0.0, 1.0);
  Colors[10] = glm::vec4(0.0, 1.0, 0.0, 1.0);
  Colors[11] = glm::vec4(0.0, 1.0, 0.0, 1.0);
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar:
  // tots el VBOs creats a continuació i fins al tancament d'aquest VAO
  // estan associats automaticament a aquest VAO.
  // [!] EL VAO HA DE SER GLOBAL!
  glGenVertexArrays(1, &VAO_Arbre);
  glBindVertexArray(VAO_Arbre);

  // Creació del Vertex Buffer Object (VBO) amb les dades dels vèrtexs:
  // [!] EL VBO HA DE SER LOCAL (és un medi, no l'objecte en si).
  GLuint VBOs[2];
  glGenBuffers(2, VBOs); //Tants canals com dades es vulguin enmmagatzemar (ex.: vertex, color, etc.)

  // Creació del buffer amb les coordenades dels vèrtexs
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); //Inicialització l'objecte (I EL CANAL CORRESPONENT)
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

  // Activem l'atribut que farem servir per vèrtex
  // Transferencia de la info al Vertex Shader per a que puguin ser pintats:  
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);
  
  // Un cop s'ha fet tot el procediment d'un canal, inclosa l'assignació amb la variable corresponent del vertex shader,
  // es pot procedir amb l'altre canal:
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
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
  fs.compileSourceFile("./shaders/basicShader.frag");
  vs.compileSourceFile("./shaders/basicShader.vert");
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
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Obtenim l'identificador per a l'atribut "translate" del vertex shader:
  transLoc = glGetUniformLocation(program->programId(), "transform");
  modelTransformArbre(0.);
}
