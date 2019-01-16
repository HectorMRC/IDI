#include <QApplication>
#include <QPushButton> // Per generar butons
#include <QHBoxLayout> // Per generar layouts horitzontals
#include <QVBoxLayout> // Per generar layouts verticals
#include <QGridLayout> // Per generar graelles

/* Signals i Slots:
 * connect(Object1, signal1, Object2, slot1);
 * El SIGNAL es un event de soritda de l'objecte Qt.    (Predefinit, no es pot canviar)
 * El SLOT s'executa quan es produeix un signal.        (Modificable, es pot implementar el codi a executar)
 * Un signal es enviat per un objecte que es rebut per
 * un Slot d'un altre objecte si cal.
 */

int main(int argc, char **argv){
    QApplication a(argc, argv);
    QPushButton hello("Hello QT!", 0);
    
    hello.resize(100, 30);
    hello.show();
    
    return a.exec();
}
