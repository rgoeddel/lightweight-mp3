#ifndef MEDIAPLAYERUI_H
#define MEDIAPLAYERUI_H

#include <QtGui>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>


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
    Phonon::MediaObject *song;
    Phonon::AudioOutput *audioOut;
    Phonon::SeekSlider *seekSlider;

    // Actions
    QAction *openAction;
    QAction *quitAction;
    QAction *playAction;
    QAction *seekLeftAction;
    QAction *seekRightAction;
};

#endif
