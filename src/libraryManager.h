#ifndef LIBRARY_MANAGER_H
#define LIBRARY_MANAGER_H

#include <QtGui>

class LibraryManager
{
    Q_OBJECT

public:
    LibraryManager();

    // Load library file
    void loadLibraryFile(char* filename);

private:

};

#endif
