#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>

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

    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc;
    // Uniform necessari per a configurar la projecció:
    GLuint projLoc;
    // Uniform necessari per a configurar la view:
    GLuint viewLoc;

    // VAO names
    GLuint VAO_Casa;
    GLuint VAO_Homer;
    GLuint VAO_Terra;

    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale, rotation;
    glm::vec3 pos;

    // Variables de la càmera:
    float FOV, RA, ZNear, ZFar; //ProjectTransform
    glm::vec3 OBS, VRP, UP; //ViewTransform

    // Per a treballar amb models:
    Model model;
};

