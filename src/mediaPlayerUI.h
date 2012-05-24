#ifndef MEDIAPLAYERUI_H
#define MEDIAPLAYERUI_H

#include <QtGui>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <Phonon/VolumeSlider>


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
    void populateMetaData();

private:
    // Local funcs
    void initState();
    void initActions();
    void initMenu();
    void initGUI();

    // Local vars
    QMenu *fileMenu;
    QLabel *metaArtist;
    QLabel *metaTitle;
    QLabel *metaGenre;

    // Sounds
    Phonon::MediaObject *song;
    Phonon::AudioOutput *audioOut;
    Phonon::SeekSlider *seekSlider;
    Phonon::VolumeSlider *volumeSlider;
    QMultiMap<QString, QString> metaData;

    // Actions
    QAction *openAction;
    QAction *quitAction;
    QAction *playAction;
    QAction *seekLeftAction;
    QAction *seekRightAction;
};

#endif
