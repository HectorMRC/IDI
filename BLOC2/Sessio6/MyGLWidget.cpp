#include "MyGLWidget.h"

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  scale = 1.0f;
  rotation = 0.f;

  // Atributs necessaris per a la projectionTransform:
  FOV = M_PI/2.f;
  RA = 1.f;
  ZNear = 0.4f;
  ZFar = 3.f;

  Ortho = false;

  // Atributs necessaris per a la viewTransform:
  OBS = glm::vec3(0.,0.,1.);
  VRP = glm::vec3(0.,0.,0.);
  UP = glm::vec3(0.,1.,0.);

  euler = glm::vec3(0.,0.,0.);
  current_x = current_y = 0;
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
  glBindVertexArray(VAOs[1]);

  // pintem
  int nTriangles = 2;
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);

  nTriangles = model.faces().size();
  modelTransform(1); // Primer patricio
  glBindVertexArray(VAOs[0]);
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);
  
  modelTransform(2); // Segon patricio
  glBindVertexArray(VAOs[0]);
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);
  
  modelTransform(3); // Tercer patricio
  glBindVertexArray(VAOs[0]);
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);

  glBindVertexArray (0);
}

// Inicialitzadora de la càmera:
void MyGLWidget::ini_camera(){
  // Configuració:
  projectTransform();
  viewTransform();
}

void MyGLWidget::modelTransform (int iteration) 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  if(iteration > 0){
    // Rotació en pitja R:
    transform = glm::rotate(transform, rotation, glm::vec3(0.,1.,0.));

    if(iteration == 1){
      transform = glm::translate(transform, glm::vec3(2., 0.5, 2.));
    }
    else if(iteration == 2){
      transform = glm::translate(transform, glm::vec3(0., 0.5, 0.));
      transform = glm::rotate(transform, (float)M_PI/2, glm::vec3(0., 1., 0.));
    }
    else if(iteration == 3){
      transform = glm::translate(transform, glm::vec3(-2., 0.5, -2.));
      transform = glm::rotate(transform, (float)M_PI, glm::vec3(0., 1., 0.));
    }

    transform = glm::scale(transform, glm::vec3(1./radi_model, 1./radi_model, 1./radi_model));
    transform = glm::translate(transform, -centre_model);

  }

  transform = glm::scale(transform, glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

// Metoda encarregat de configurar la projecció (Optica):
void MyGLWidget::projectTransform(){
  glm::mat4 transform(1.0f);
  if(!Ortho) transform = glm::perspective(FOV, RA, ZNear, ZFar);
  else transform = glm::ortho(Left, Right, Bottom, Top, ZNear, ZFar);
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
  // LookAt:
  //glm::mat4 transform = glm::lookAt(OBS, VRP, UP);
  // Angles d'euler:
  glm::mat4 transform(1.0f);
  transform = glm::translate(transform, glm::vec3(centre_esfera.x, centre_esfera.y, centre_esfera.z-distancia));
  transform = glm::rotate(transform, euler.x, glm::vec3(0., 0., 1.));
  transform = glm::rotate(transform, euler.y, glm::vec3(1., 0., 0.));
  transform = glm::rotate(transform, euler.z, glm::vec3(0., 1., 0.));
  // RECORDA: els radiants en positiu assignen rotació antihoraria!

  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &transform[0][0]);

  /* Paràmetres: glm::lookAt(OBS, VRP, UP)
      OBS:  Posició de la càmera (Coordenades)
      VRP:  Orientació de visió de la càmera (Punt al qual apunta el centre de la càmera)
      UP:   Sentit del vector "a munt" de la càmera (Indica cap a quin sentit i direcció és el up de la vista captada per la càmera)
  */
}

void MyGLWidget::resizeGL (int w, int h) 
{
  glViewport(0, 0, w, h);
  RA = float(w)/float(h);

  if(RA < 1.){
    if(!Ortho) FOV = 2 * atan(tan((float)M_PI/4)/RA);
    else{
      Left = -radi_esfera/2.;
      Right = radi_esfera/2.;
      Top = radi_esfera/2./RA;
      Bottom = -radi_esfera/2./RA;
    }
  }
  else if(Ortho and RA > 1.){
    Left = -radi_esfera/2. * RA;
    Right = radi_esfera/2. * RA;
    Bottom = - radi_esfera/2.;
    Top = radi_esfera/2.;
  }


  projectTransform();
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

    case Qt::Key_Z:
      FOV -= (float)M_PI/180; //Reduir l'angle del FOV implica fer Zoom-In
      break;

    case Qt::Key_X:
      FOV += (float)M_PI/180; //Augmentar l'angle del FOV implica fer Zoom-out
      break;

    case Qt::Key_O:
      Ortho = !Ortho;
      break;

    default:
      event->ignore();
      break;
  }

  update();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event){
  makeCurrent();

  if(event->x() > current_x) euler.z += 0.1;
  else if(event->x() < current_x) euler.z -= 0.1;
  if(event->y() > current_y) euler.y += 0.1;
  else if(event->y() < current_y) euler.y -= 0.1; 

  current_x = event->x();
  current_y = event->y();
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
  glGenVertexArrays(3, VAOs);
  glBindVertexArray(VAOs[2]);

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
    glm::vec3(-2.5, 0., -2.5),
    glm::vec3(-2.5, 0., 2.5),
    glm::vec3(2.5, 0., -2.5),
    glm::vec3(2.5, 0., -2.5),
    glm::vec3(-2.5, 0., 2.5),
    glm::vec3(2.5, 0., 2.5)
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
  glBindVertexArray(VAOs[1]);

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

  // Actualització de la caixa contenidora:
  calcular_extrems(terra, 6);
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
  model.load("/media/hector/TOSHIBA_EXT/FiB/ASSIGN/IDI/LAB/models/Patricio.obj"); //Càrrega del model en qüestió
  glBindVertexArray(VAOs[0]);

  GLuint VBO_Model[2]; //2 canals, un per al vertex i l'altre per als colors
  glGenBuffers(2, VBO_Model); //Amb l'obertura del canal aquest VBO queda lincat al VAO_Homer.

  // Cal calcular la mida del model per a poder imprimir els buffers als VBOs
  int model_size = sizeof(GL_FLOAT) * model.faces().size() * 3 * 3;

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[0]);
  glBufferData(GL_ARRAY_BUFFER, model_size, model.VBO_vertices(), GL_STATIC_DRAW); //Posició
  
  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[1]);
  glBufferData(GL_ARRAY_BUFFER, model_size, model.VBO_matdiff(), GL_STATIC_DRAW); //Color
  
  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  // Actualització de la caixa contenidora:
  calcular_extrems(model.VBO_vertices(), model.vertices().size());
}

void MyGLWidget::createBuffers () 
{
  // Com que tots els VAOs son ara enmagatzemats al mateix array, només cal inicialitzar-lo una sola vegada abans de contruir cap objecte.
  glGenVertexArrays(3, VAOs);

  // A l'hora de treballar amb cada model ja s'obrirà el canal del VAO que pertoqui.
  //create_caseta();
  load_homer();
  create_terra();

  glBindVertexArray (0);
  calcular_esfera(glm::vec3(0., -0.5, 0.), max(radi_terra, radi_model));
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

// Calculadora de la capsa de l'escena i els seus atributs
// Aquesta funció conté com a parametres els més comodes a l'hora "d'escanejar" un model.
void MyGLWidget::calcular_extrems(float current[], int size){
  /* Per a calcular l'esfera de la caixa contenidora hem de trobar primer els punt màxim
  i el punt minim delimitats pel conjunt d'objectes que es pintaran a l'escena.
  Per fer-ho, doncs, ens calen TOTS els vertex de TOTS els objectes a pintar, en aquest cas
  conservo tots els vertex a l'atribut global VBOs.*/

  glm::vec3 puntMinim_model;
  glm::vec3 puntMaxim_model;

  puntMinim_model.x = puntMaxim_model.x = current[0];
  puntMinim_model.y = puntMaxim_model.y = current[1];
  puntMinim_model.z = puntMaxim_model.z = current[2];

  for(int i = 0; i < size; i+= 3){
    // Per a poder usar els metodes min i max cal afegir la capsalera <algorithm> al .h!
    puntMaxim_model.x = max(puntMaxim_model.x, current[i]);
    puntMaxim_model.y = max(puntMaxim_model.y, current[i+1]);
    puntMaxim_model.z = max(puntMaxim_model.z, current[i+2]);

    puntMinim_model.x = min(puntMinim_model.x, current[i]);
    puntMinim_model.y = min(puntMinim_model.y, current[i+1]);
    puntMinim_model.z = min(puntMinim_model.z, current[i+2]);
  }

  centre_model.x = (puntMaxim_model.x + puntMinim_model.x)/2;
  centre_model.y = (puntMaxim_model.y + puntMinim_model.y)/2;
  centre_model.z = (puntMaxim_model.z + puntMinim_model.z)/2;

  // Per a poder usar el metode sqrt cal afegir la capsalera <math.h> al .h!
  radi_model = sqrt(pow((puntMaxim_model.x - puntMinim_model.x), 2) +
                    pow((puntMaxim_model.y - puntMinim_model.y), 2) +
                    pow((puntMaxim_model.z - puntMinim_model.z), 2));
}

// Calculadora de la capsa de l'escena i els seus atributs
// Aquesta funció conté com a parametres els més comodes a l'hora "d'escanejar" un array de vertex.
void MyGLWidget::calcular_extrems(glm::vec3 current[], int size){
  /* Per a calcular l'esfera de la caixa contenidora hem de trobar primer els punt màxim
  i el punt minim delimitats pel conjunt d'objectes que es pintaran a l'escena.
  Per fer-ho, doncs, ens calen TOTS els vertex de TOTS els objectes a pintar, en aquest cas
  conservo tots els vertex a l'atribut global VBOs.*/

  glm::vec3 puntMinim_terra;
  glm::vec3 puntMaxim_terra;

  puntMinim_terra.x = puntMaxim_terra.x = current[0].x;
  puntMinim_terra.y = puntMaxim_terra.y = current[0].y;
  puntMinim_terra.z = puntMaxim_terra.z = current[0].z;

  for(int i = 0; i < size; i++){
    // Per a poder usar els metodes min i max cal afegir la capsalera <algorithm> al .h!
    puntMaxim_terra.x = max(puntMaxim_terra.x, current[i].x);
    puntMaxim_terra.y = max(puntMaxim_terra.y, current[i].y);
    puntMaxim_terra.z = max(puntMaxim_terra.z, current[i].z);

    puntMinim_terra.x = min(puntMinim_terra.x, current[i].x);
    puntMinim_terra.y = min(puntMinim_terra.y, current[i].y);
    puntMinim_terra.z = min(puntMinim_terra.z, current[i].z);
  }

  centre_terra.x = (puntMaxim_terra.x + puntMinim_terra.x)/2;
  centre_terra.y = (puntMaxim_terra.y + puntMinim_terra.y)/2;
  centre_terra.z = (puntMaxim_terra.z + puntMinim_terra.z)/2;

  // Per a poder usar el metode sqrt cal afegir la capsalera <math.h> al .h!
  radi_terra = sqrt(pow((puntMaxim_terra.x - puntMinim_terra.x), 2) +
                    pow((puntMaxim_terra.y - puntMinim_terra.y), 2) +
                    pow((puntMaxim_terra.z - puntMinim_terra.z), 2));
}

void MyGLWidget::calcular_esfera(glm::vec3 centre, float radi){
  // Si volem que l'esfera estigui centrada al origen:
  centre_esfera = centre;
  radi_esfera = radi;

  // Configuració dels paràmetres de càmera:
  distancia = radi_esfera * 2;

  VRP = centre_esfera;
  OBS = VRP + glm::vec3(0.,0.,distancia);

  ZNear = distancia - radi_esfera;
  ZFar = distancia + radi_esfera;
  // Per a poder usar el metode asin cal afegir la capçalera cmath al .h
  // Multiplicar el FOV obtingut per un escalar enxampla l'angle de visio, el que fa que la vista sembli estar molt més lluny del que toca.
  FOV = asin(radi_esfera/distancia);

  Left = -radi_esfera/2.;
  Bottom = -radi_esfera/2.;
  Top = radi_esfera/2.;
  Right = radi_esfera/2.;
}