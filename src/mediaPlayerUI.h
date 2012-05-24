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
    void importAll();
    void metaQueueUpdate(Phonon::State, Phonon::State);
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

    QStringList getTableLabels();

    // Visuals
    QMenu *fileMenu;
    QTableWidget *tableWidget;
    QLabel *metaArtist;
    QLabel *metaTitle;
    QLabel *metaGenre;

    // Data Handling
    Phonon::MediaObject *metaLoader;
    QStringList *metaQueue;

    // Sounds
    Phonon::MediaObject *song;
    Phonon::AudioOutput *audioOut;
    Phonon::SeekSlider *seekSlider;
    Phonon::VolumeSlider *volumeSlider;
    QMultiMap<QString, QString> metaData;

    // Actions
    QAction *openAction;
    QAction *importBulkAction;
    QAction *quitAction;
    QAction *playAction;
    QAction *seekLeftAction;
    QAction *seekRightAction;
};

#endif
