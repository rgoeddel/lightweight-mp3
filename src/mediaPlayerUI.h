#ifndef MEDIAPLAYERUI_H
#define MEDIAPLAYERUI_H

#include <QtGui>


class MediaPlayerUI : public QMainWindow
{
    Q_OBJECT

public:
    MediaPlayerUI();

private slots:
    void open();
    void quit();
    void play();
    void seekLeft();
    void seekRight();

private:
    // Local funcs
    void initState();
    void initActions();
    void initMenu();
    void initGUI();

    // Local vars
    // Pieces
    QMenu *fileMenu;
    QSlider *seekSlider;

    // Actions
    QAction *openAction;
    QAction *quitAction;
    QAction *playAction;
    QAction *seekLeftAction;
    QAction *seekRightAction;

    // Internal state
    bool playing;
};

#endif
