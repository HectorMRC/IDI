#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>

#include <algorithm>
#include <math.h>
#include <cmath>
#include <iostream>
using namespace std;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "/media/hector/TOSHIBA_EXT/FiB/ASSIGN/IDI/LAB/Model/model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouseMoveEvent - Es crida quan es genera un event de ratolí
    virtual void mouseMoveEvent(QMouseEvent *event);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform (int iteration);
    //Per comoditat he decidit separar en diferents metodes la carrega dels objectes:
    void create_caseta();
    void load_homer();
    void create_terra();

    // Metoda encarregat de configurar la projecció (Optica):
    void projectTransform();
    // Metoda encarregat de configurar la vista (Posició):
    void viewTransform();
    // Inicialitzadora de la càmera:
    void ini_camera();
    // Calculadora de la capsa de l'escena i els seus atributs:
    void calcular_extrems(float current[], int size); // Per a models importats
    void calcular_extrems(glm::vec3 current[], int size); // Per a objectes creats localment
    void calcular_esfera(glm::vec3 centre, float radi);

    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc;
    // Uniform necessari per a configurar la projecció:
    GLuint projLoc;
    // Uniform necessari per a configurar la view:
    GLuint viewLoc;

    // VAO & VBO names
    GLuint VAOs[3];

    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale, rotation;
    glm::vec3 pos;

    // Variables de la càmera:
    float FOV, RA, ZNear, ZFar; //ProjectTransform
    glm::vec3 OBS, VRP, UP; //ViewTransform

    bool Ortho;
    float Left, Right, Bottom, Top; 

    // Per a treballar amb models:
    Model model;

    // Dades respecte la capsa contenidora de l'escena
    // i els seus components importants:
    glm::vec3 centre_esfera;
    float radi_esfera, distancia;

    glm::vec3 euler;
    float current_x, current_y;

    // Dades dels objectes i/o models:
    glm::vec3 centre_model;
    float radi_model;

    glm::vec3 centre_terra;
    float radi_terra;
};

