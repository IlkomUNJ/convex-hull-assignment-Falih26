#include <QApplication>
#include "drawingwindow.h"

int main(int argc, char *argv[])
{
    // Membuat objek aplikasi Qt
    QApplication a(argc, argv);

    // Membuat dan menampilkan jendela utama
    DrawingWindow w;
    w.show();

    // Memulai loop event aplikasi
    return a.exec();
}
