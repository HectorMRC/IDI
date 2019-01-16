#include <QApplication>
#include "MyForm.h"

int main(int argc, char **argv){
    QApplication(argc, argv);
    MyForm form;
    form.show();
    
    return app.exec();
}
