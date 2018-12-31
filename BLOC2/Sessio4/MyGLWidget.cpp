#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  scale = 1.0f;
  rotation = 0.f;
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

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  createBuffers();
}

void MyGLWidget::paintGL () 
{
  glViewport (0, 0, width(), height()); // Aquesta crida no caldria perquè Qt la fa de forma automàtica amb aquests paràmetres

  
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT);

  // Carreguem la transformació de model
  modelTransform (0);
  // Inicialització de la càmera:
  ini_camera();
  // Carreguem la projecció corresponent a la vista que es vol pintar:
  //projectTransform();
  // Carreguem el punt de vista corresponent a la vista que es vol pintar:
  //viewTransform();

  // Activem el VAO per a pintar la caseta 
  //glBindVertexArray (VAO_Casa);
  glBindVertexArray(VAO_Homer);

  // pintem
  int nTriangles = model.faces().size();
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);

  nTriangles = 2;
  modelTransform(1);
  glBindVertexArray(VAO_Terra);
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);

  glBindVertexArray (0);
}

// Inicialitzadora de la càmera:
void MyGLWidget::ini_camera(){
  // Atributs necessaris per a la projectionTransform:
  FOV = M_PI/2.f;
  RA = 1.f;
  ZNear = 0.4f;
  ZFar = 3.f;

  // Atributs necessaris per a la viewTransform:
  OBS = glm::vec3(0.,0.,1.);
  VRP = glm::vec3(0.,0.,0.);
  UP = glm::vec3(0.,1.,0.);

  // Configuració:
  projectTransform();
  viewTransform();
}

void MyGLWidget::modelTransform (int iteration) 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  if(!iteration) transform = glm::rotate(transform, rotation, glm::vec3(0.,1.,0.));
  transform = glm::scale(transform, glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

// Metoda encarregat de configurar la projecció (Optica):
void MyGLWidget::projectTransform(){
  glm::mat4 transform = glm::perspective(FOV, RA, ZNear, ZFar);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &transform[0][0]);

  /* Paràmetres: glm::perspective(FOV, RA, ZNear, ZFar)
      FOV:    Angle d'obertura de la càmera (En radiants sempre!).
      RA:     Ratio del ViewPort on s'aplicarà la vista; és a dir: RA = amplada(W)/altura(H).
      ZNear:  Distancia mínima a la qual es pot arribar sense tallar l'objecte.
      ZFar:   Distancia màxima des de la qual és observable l'objecte.
  */
}

// Metoda encarregat de configurar la vista (Posició):
void MyGLWidget::viewTransform(){
  glm::mat4 transform = glm::lookAt(OBS, VRP, UP);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &transform[0][0]);

  /* Paràmetres: glm::lookAt(OBS, VRP, UP)
      OBS:  Posició de la càmera (Coordenades)
      VRP:  Orientació de visió de la càmera (Punt al qual apunta el centre de la càmera)
      UP:   Sentit del vector "a munt" de la càmera (Indica cap a quin sentit i direcció és el up de la vista captada per la càmera)
  */
}

void MyGLWidget::resizeGL (int w, int h) 
{
  // Aquí anirà el codi que cal fer quan es redimensiona la finestra
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_S: // escalar a més gran
      scale += 0.05;
      break;

    case Qt::Key_D: // escalar a més petit
      scale -= 0.05;
      break;

    case Qt::Key_R:
      rotation += (float)M_PI/4;
      break;

    default:
      event->ignore();
      break;
  }

  update();
}

//Per comoditat he decidit separar en diferents metodes la carrega dels objectes:
void MyGLWidget::create_caseta(){
  // Dades de la caseta
  // Dos VBOs, un amb posició i l'altre amb color
  glm::vec3 posicio[9] = {
  glm::vec3(-0.5, -1.0, -0.5),
  glm::vec3( 0.5, -1.0, -0.5),
  glm::vec3(-0.5,  0.0, -0.5),
  glm::vec3(-0.5,  0.0, -0.5),
  glm::vec3( 0.5, -1.0, -0.5),
  glm::vec3( 0.5,  0.0, -0.5),
  glm::vec3( 0.5,  0.0, -0.5),
  glm::vec3( 0.0,  0.6, -0.5),
  glm::vec3(-0.5,  0.0, -0.5)
  }; 
  glm::vec3 color[9] = {
  glm::vec3(1,0,0),
  glm::vec3(0,1,0),
  glm::vec3(0,0,1),
  glm::vec3(0,0,1),
  glm::vec3(0,1,0),
  glm::vec3(1,0,0),
  glm::vec3(1,0,0),
  glm::vec3(0,1,0),
  glm::vec3(0,0,1)
  };

  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Casa);
  glBindVertexArray(VAO_Casa);

  GLuint VBO_Casa[2];
  glGenBuffers(2, VBO_Casa);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Casa[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Casa[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
}

void MyGLWidget::create_terra(){
  glm::vec3 terra[6] = {
    glm::vec3(-1., -1., -1),
    glm::vec3(-1., -1., 1.),
    glm::vec3(1., -1., -1.),
    glm::vec3(1., -1., -1.),
    glm::vec3(-1., -1., 1.),
    glm::vec3(1., -1., 1.)
  };

  glm::vec3 color[6] = {
    glm::vec3(1.,0.,1.),
    glm::vec3(1.,0.,1.),
    glm::vec3(1.,0.,1.),
    glm::vec3(1.,0.,1.),
    glm::vec3(1.,0.,1.),
    glm::vec3(1.,0.,1.)
  };

  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(terra), terra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
}

void MyGLWidget::load_homer(){
  /* Per a poder carregar els models correctament cal afegir al fitxer .pro:
      INCLUDEPATH += <directori>/Model
      SOURCES += <directori>/Model/model.cpp
      HEADERS += <directori>/Model/model.hh

     També cal afegir a la capsalera del MyGLWidget.h l'include del model.h

     Al initializeGL () cal afegir la linea glEnable(GL_DEPTH_TEST);

     Procediment:
      - S'ha de creat un VAO global per al model homer. El model es lincarà a aquest VAO.
      - També calen els VBOs respectius, doncs s'han de lincar amb el VAO obert. (Segons la info que calgui més endavant, es poden declarar de manera local o global).
      - Pel que fa a la resta, el procediment és el mateix que amb la declaració manual d'un objecte.
  */

  //Model model; //Objecte on s'enmagatzemarà el model DECLARAT COM GLOBAL! (.hh)
  model.load("/media/hector/TOSHIBA_EXT/FiB/ASSIGN/IDI/LAB/models/HomerProves.obj"); //Càrrega del model en qüestió
  glGenVertexArrays(1, &VAO_Homer); //El model carregat quedarà linkat a aquest VAO
  glBindVertexArray(VAO_Homer);

  GLuint VBO_Homer[2]; //2 canals, un per al vertex i l'altre per als colors
  glGenBuffers(2, VBO_Homer); //Amb l'obertura del canal aquest VBO queda lincat al VAO_Homer.

  // Cal calcular la mida del model per a poder imprimir els buffers als VBOs
  int model_size = sizeof(GL_FLOAT) * model.faces().size() * 3 * 3;

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
  glBufferData(GL_ARRAY_BUFFER, model_size, model.VBO_vertices(), GL_STATIC_DRAW); //Posició
  
  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  glBufferData(GL_ARRAY_BUFFER, model_size, model.VBO_matdiff(), GL_STATIC_DRAW); //Color
  
  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
}

void MyGLWidget::createBuffers () 
{
  //create_caseta();
  load_homer();
  create_terra();

  glBindVertexArray (0);
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
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  // Obtenim l'identificador per a l'atribut "projection" del vertex shader
  projLoc = glGetUniformLocation(program->programId(), "proj");
  viewLoc = glGetUniformLocation(program->programId(), "view");
}

