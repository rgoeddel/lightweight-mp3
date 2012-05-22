#include <QtGui>

#include "mediaPlayerUI.h"

int main(int argv, char **argc)
{
    QApplication app(argv, argc);

    MediaPlayerUI mpui;
    mpui.show();

    return app.exec();
}
