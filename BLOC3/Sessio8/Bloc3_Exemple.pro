TEMPLATE    = app
QT         += opengl

INCLUDEPATH +=  /usr/include/glm
INCLUDEPATH += /media/hector/TOSHIBA_EXT/FiB/ASSIGN/IDI/LAB/Model

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h
HEADERS += /media/hector/TOSHIBA_EXT/FiB/ASSIGN/IDI/LAB/Model/model.h


SOURCES += /media/hector/TOSHIBA_EXT/FiB/ASSIGN/IDI/LAB/Model/model.cpp
SOURCES += main.cpp  MyForm.cpp \
        MyGLWidget.cpp
