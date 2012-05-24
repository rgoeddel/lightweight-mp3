#include <QtGui>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <Phonon/VolumeSlider>
#include <iostream>
#include <cmath>

#include "mediaPlayerUI.h"

using namespace std;

MediaPlayerUI::MediaPlayerUI()
{
    setWindowTitle(tr("Lightweight MP3"));

    metaLoader = new Phonon::MediaObject(this);
    metaQueue = new QStringList;

    connect(metaLoader, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(metaQueueUpdate(Phonon::State, Phonon::State)));

    initState();
    initActions();
    initMenu();
    initGUI();

    resize(640, 480);
}

// Initialize internal state
void MediaPlayerUI::initState()
{
    song = 0;
}

// Create actions
void MediaPlayerUI::initActions()
{
    // Create actions
    openAction = new QAction(tr("&Open"), this);
    importBulkAction = new QAction(tr("&Import all"), this);
    quitAction = new QAction(tr("&Quit"), this);
    playAction = new QAction(QPixmap("../resources/icons/playButton.png"), 0, this);
    seekLeftAction = new QAction(QPixmap("../resources/icons/seekLeft.png"), 0, this);
    seekRightAction = new QAction(QPixmap("../resources/icons/seekRight.png"), 0, this);

    // Connections
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(importBulkAction, SIGNAL(triggered()), this, SLOT(importAll()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(playAction, SIGNAL(triggered()), this, SLOT(play()));
    connect(seekLeftAction, SIGNAL(triggered()), this, SLOT(seekLeft()));
    connect(seekRightAction, SIGNAL(triggered()), this, SLOT(seekRight()));
}

// Create the menu
void MediaPlayerUI::initMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(importBulkAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
}

// Create the visual components on the GUI
void MediaPlayerUI::initGUI()
{
    // Song info
    // XXX: Song choice later
    song = new Phonon::MediaObject(this);
    song->setCurrentSource(Phonon::MediaSource("../resources/music/pressure_cooker.mp3"));
    audioOut = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::Path path = Phonon::createPath(song, audioOut);

    // Meta data updating!
    connect(song, SIGNAL(metaDataChanged()), this, SLOT(populateMetaData()));
    metaArtist = new QLabel(this);
    metaTitle = new QLabel(this);
    metaGenre = new QLabel(this);

    // Display library data here
    QStringList tableLabels = getTableLabels();
    tableWidget = new QTableWidget(1, tableLabels.size(), this);
    tableWidget->setHorizontalHeaderLabels(tableLabels);

    // Sliders
    seekSlider = new Phonon::SeekSlider(song, this);
    volumeSlider = new Phonon::VolumeSlider(audioOut, this);

    // Buttons
    QToolBar *bar = new QToolBar;
    bar->addAction(seekLeftAction);
    bar->addAction(playAction);
    bar->addAction(seekRightAction);

    // Layout
    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->addWidget(metaArtist);
    textLayout->addWidget(metaTitle);
    textLayout->addWidget(metaGenre);

    QHBoxLayout *tableLayout = new QHBoxLayout;
    tableLayout->addWidget(tableWidget);

    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(seekSlider);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(bar);
    buttonLayout->addWidget(volumeSlider);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(textLayout);
    mainLayout->addLayout(tableLayout);
    mainLayout->addLayout(sliderLayout);
    mainLayout->addLayout(buttonLayout);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
}

// Get the headers for our table
QStringList MediaPlayerUI::getTableLabels()
{
    // Track # -- Song -- Album -- Artist -- Genre -- Bitrate --
    QStringList headers;
    headers.append(tr("Track #"));
    headers.append(tr("Song"));
    headers.append(tr("Album"));
    headers.append(tr("Artist"));
    headers.append(tr("Genre"));
    headers.append(tr("Bitrate"));

    return headers;
}

// === SLOTS =============
void MediaPlayerUI::populateMetaData()
{
    if (song == 0) {
        cerr << "ERR: No song to populate meta data with." << endl;
        return;
    }

    QStringList artists = song->metaData("ARTIST");
    if (artists.begin() != artists.end()) {
        metaArtist->setText("Artist: " + (*artists.begin()));
    }
    QStringList titles = song->metaData("TITLE");
    if (titles.begin() != titles.end()) {
        metaTitle->setText("Title: " + (*titles.begin()));
    }
    QStringList genres = song->metaData("GENRE");
    if (genres.begin() != genres.end()) {
        metaGenre->setText("Genre: " + (*genres.begin()));
    }
}

// Open a new file, adding it to our table of playable songs.
// This process builds a library file for later reloading
void MediaPlayerUI::open()
{
    // Supported filetypes for now:
    // -- MP3 -- MP4 -- OGG -- WAV -- FLAC?
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Music (*.mp3 *.mp4 *.ogg *.wav)"));
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
    }

    cout << "Found songs:" << endl;
    for (QStringList::iterator it = fileNames.begin();
         it != fileNames.end();
         it++)
    {
        cout << "\t" << it->toStdString() << endl;
    }
    metaQueue->append(fileNames);
}

void MediaPlayerUI::importAll()
{
    // Supported filetypes for now:
    // -- MP3 -- MP4 -- OGG -- WAV -- FLAC?
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setNameFilter(tr("Music (*.mp3 *.mp4 *.ogg *.wav)"));
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
    }

    cout << "Found songs:" << endl;
    for (QStringList::iterator it = fileNames.begin();
         it != fileNames.end();
         it++)
    {
        cout << "\t" << it->toStdString() << endl;
    }
}

void MediaPlayerUI::metaQueueUpdate(Phonon::State nstate, Phonon::State ostate)
{
    if (nstate == Phonon::ErrorState) {
        cerr << "ERR: Could not open file" << endl;
        // Load up next file!
        while (!metaQueue->isEmpty() &&
               !metaQueue->takeLast() == metaLoader->currentSource().fileName())
        { /* Loop until loaded or no song to choose*/ }
        return;
    }

    if (nstate != Phonon::StoppedState && nstate != Phonon::PausedState) {
        return;
    }

    if (metaLoader->currentSource().type() == Phonon::MediaSource::Invalid) {
        cerr << "ERR: Invalid MediaSource" << endl;
        return;
    }

    // XXX Resolve meta data
}

void MediaPlayerUI::quit()
{
    cout << "quit" << endl;
}

void MediaPlayerUI::play()
{
    if (song == 0) {
        cerr << "ERR: Play error. No song loaded." << endl;
        return;
    }
    switch (song->state()) {
        case Phonon::PausedState:
            playAction->setIcon(QPixmap("../resources/icons/playButton.png"));
            song->play();
            break;
        case Phonon::StoppedState:
            playAction->setIcon(QPixmap("../resources/icons/playButton.png"));
            song->play();
            break;
        case Phonon::PlayingState:
            playAction->setIcon(QPixmap("../resources/icons/pauseButton.png"));
            song->pause();
            break;
        default:
            cerr << "ERR: MediaObject state not handled." << endl;
            break;
    }
}

void MediaPlayerUI::seekLeft()
{
}

void MediaPlayerUI::seekRight()
{
}
