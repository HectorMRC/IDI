#include "MyGLWidget.h"

#include <iostream>
using namespace std;

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat

  //Guardem el valor inicial per poder-los consultar:
  scale = 0.5;
  TX = TY = RZ = 0;
  rotation = 0;

  ctrl = false;
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

  glViewport (0, 0, width()/2, height()/2);
  modelTransform(ctrl? 1. : 0.);

  // Activem l'Array a pintar 
  glBindVertexArray(VAO1);

  // Pintem l'escena
  int nTriangles = 1;
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);

  /////////////////////////////////////////////
  // Segon objecte: apartat 6
  glViewport (width()/2, height()/2, width()/2, height()/2);
  modelTransform(ctrl? -1. : 0.);

  // Activem l'Array a pintar 
  glBindVertexArray(VAO2);

  // Pintem l'escena
  nTriangles = 1;
  glDrawArrays(GL_TRIANGLES, 0, nTriangles*3);
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  // Aquí anirà el codi que cal fer quan es redimensiona la finestra
}

void MyGLWidget::modelTransform(float factor){ //L'atribut del metode es necessari per a la meva solució de l'apartat 6.
  glm::mat4 TG (1.0); //Matriu de transformació

  TG = glm::scale(TG, glm::vec3(scale, scale, 0.));
  TG = glm::rotate(TG, rotation, glm::vec3(0., 0., 1.)); //Si M_PI > 0 --> la rotació és en sentit antihorari!
  TG = glm::translate(TG, glm::vec3(TX, TY, 0.));

  if(ctrl){
    TG = glm::rotate(TG, factor*RZ, glm::vec3(0., 0., 1.));
    if(factor < 0) ctrl = false;
  }

  /* OBSERVACIONS:
   * ROTATE: Si fem la rotació després de la translació, com demana l'apartat 2: el triangue es desplaça pel eixos corresponents rotant sobre si mateix;
             no obstant, si fem la rotació abans, com es demana a l'apartat 3: el triangle es desplaça rotant respecte l'origen de coordenades
            (com fent una espiral d'endins cap enfora).

   * SCALE: Si l'escale es du a terme a l'inici, l'objecte es escalat respecte l'origen de coordenades;
            altrament, si s'escala al final, l'objecte s'escala respecte el seu centre.
            Extranyament, fer l'escalat enmig te efectes sobre la rotació (en el cas de l'apartat 3): tanca o enxampla l'espiral
            que dibuixa el triangle en traslladar-lo.
  */

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

// Aquest metode permet consultar un event de teclat.
void MyGLWidget::keyPressEvent(QKeyEvent *e){
  //cout << "FuckPineapplePizza" << endl;
  makeCurrent();
  switch(e->key()){
    case Qt::Key_S:
      scale += 0.1; //augmentem la vista
      break;

    case Qt::Key_D:
      scale -= 0.1; //decrementem la vista
      break;

    case Qt::Key_Left:
      TX += -0.1;
      rotation += (float)M_PI/4;
      break;

    case Qt::Key_Right:
      TX += 0.1;
      rotation += -(float)M_PI/4;
      break;

    case Qt::Key_Up:
      TY += 0.1;
      rotation += -(float)M_PI/4;
      break;

    case Qt::Key_Down:
      TY += -0.1;
      rotation += (float)M_PI/4;
      break;

    case Qt::Key_P:
      RZ += 30*((float)M_PI/180);
      ctrl = true;
      break;

    default: e->ignore();
  }

  // S'ha de efectius els canvis, es a dir, carregar-los als uniforms corresponents.
  //glUniform1f(scaleLoc, scale);
  //modelTransform();

  // Evidentment, si un event de teclat ha de generar algun tipus de canvi per pantalla,
  // és necessari actualitzar-ne el contingut. Això significa tornar a imprimir TOTA la vista.
  update();
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

  ////////////////////////////////////////////////////////////////////////////////////////
  // Segon objecte: demanat a l'apartat 6 (Aprofitarè el model ja creat) /////////////////
  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);

  GLuint VBO2;
  glGenBuffers(1, &VBO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);
  
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
  // Obtenim identificador per a l'atribut "color" del vertex shader:
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Obtenim l'identificador per a l'atribut "scale" del vertex shader:
  //scaleLoc = glGetUniformLocation(program->programId(), "scale");
  //glUniform1f(scaleLoc, scale); // A diferencia dels atributs anterior, al uniform cal assignar-li un valor de d'aquesta manera (més endavant, si cal, es pot modificar).
  // Obtenim l'identificador per a l'atribut "translate" del vertex shader:
  transLoc = glGetUniformLocation(program->programId(), "transform");
  modelTransform(0.);
}
