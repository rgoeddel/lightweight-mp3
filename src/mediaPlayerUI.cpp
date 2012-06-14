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
    songIdx = 0;
    state = WAITING;
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
    song = new Phonon::MediaObject(this);
    audioOut = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::Path path = Phonon::createPath(song, audioOut);

    // Hook up song signals
    connect(song, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(songStateChanged(Phonon::State, Phonon::State)));
    connect(song, SIGNAL(aboutToFinish()),
            this, SLOT(queueNextSong()));


    // Meta data updating!
    connect(song, SIGNAL(metaDataChanged()), this, SLOT(populateMetaData()));
    metaArtist = new QLabel(this);
    metaTitle = new QLabel(this);
    metaGenre = new QLabel(this);

    // Display library data here
    QStringList tableLabels = getTableLabels();
    tableWidget = new QTableWidget(0, tableLabels.size(), this);
    tableWidget->setHorizontalHeaderLabels(tableLabels);
    tableWidget->hideColumn(tableLabels.size()-1);

    connect(tableWidget, SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(loadSong(int, int)));

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
    // Track # -- Song -- Album -- Artist -- Genre -- Bitrate -- (Hidden file location)
    QStringList headers;
    headers.append(tr("Track #"));
    headers.append(tr("Song"));
    headers.append(tr("Album"));
    headers.append(tr("Artist"));
    headers.append(tr("Genre"));
    headers.append(tr("Bitrate"));
    headers.append(tr("File path"));

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
    metaLoader->setCurrentSource(Phonon::MediaSource(*metaQueue->begin()));
    metaQueue->removeFirst();
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

void MediaPlayerUI::metaQueueUpdate(Phonon::State nstate, Phonon::State /*ostate*/)
{
    if (nstate == Phonon::ErrorState) {
        cerr << "ERR: Could not open file" << endl;
        // Load up next file!
        while (!metaQueue->isEmpty() &&
               !(metaQueue->takeLast() == metaLoader->currentSource().fileName()))
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

    // Resolve meta data
    // Track # -- Song -- Album -- Artist -- Genre -- Bitrate --
    QStringList artist = metaLoader->metaData("ARTIST");
    QStringList album = metaLoader->metaData("ALBUM");
    QStringList title = metaLoader->metaData("TITLE");
    QStringList genre = metaLoader->metaData("GENRE");
    QStringList trackno = metaLoader->metaData("TRACK-NUMBER");
    QStringList bitrate = metaLoader->metaData("BITRATE");

    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, new QTableWidgetItem(*(trackno.begin())));
    tableWidget->setItem(row, 1, new QTableWidgetItem(*(title.begin())));
    tableWidget->setItem(row, 2, new QTableWidgetItem(*(album.begin())));
    tableWidget->setItem(row, 3, new QTableWidgetItem(*(artist.begin())));
    tableWidget->setItem(row, 4, new QTableWidgetItem(*(genre.begin())));
    tableWidget->setItem(row, 5, new QTableWidgetItem(*(bitrate.begin())));
    tableWidget->setItem(row, 6, new QTableWidgetItem(metaLoader->currentSource().fileName()));

    for (int i = 0; i < tableWidget->columnCount(); i++) {
        tableWidget->item(row, i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }

    // Load up next item
    if (!metaQueue->isEmpty()) {
        metaLoader->setCurrentSource(Phonon::MediaSource(*(metaQueue->begin())));
        metaQueue->removeFirst();
    }

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
            playAction->setIcon(QPixmap("../resources/icons/pauseButton.png"));
            song->play();
            state = PLAYING;
            break;
        case Phonon::StoppedState:
            playAction->setIcon(QPixmap("../resources/icons/pauseButton.png"));
            song->play();
            state = PLAYING;
            break;
        case Phonon::PlayingState:
            playAction->setIcon(QPixmap("../resources/icons/playButton.png"));
            song->pause();
            state = PAUSED;
            break;
        default:
            cerr << "ERR: MediaObject state not handled." << endl;
            cerr << song->state() << endl;
            break;
    }
}

void MediaPlayerUI::loadSong(int r, int /*c*/)
{
    state = LOADING;
    QTableWidgetItem *item = tableWidget->item(r, tableWidget->columnCount()-1);
    song->setCurrentSource(Phonon::MediaSource(item->text()));
    songIdx = r;
}

void MediaPlayerUI::songStateChanged(Phonon::State nstate, Phonon::State)
{
    // Wait for the song to load, then play it
    if (state == LOADING) {
        switch (nstate) {
            case Phonon::LoadingState:
            case Phonon::BufferingState:
            case Phonon::ErrorState:
                break;
            default:
                state = PLAYING;
                play();
                break;
        }
    }
}

void MediaPlayerUI::queueNextSong()
{
    // If in shuffle mode (XXX Not yet implemented),
    // choose a random song to play next. Otherwise,
    // go down the list
    songIdx = (songIdx+1) % tableWidget->rowCount();
    QTableWidgetItem *item = tableWidget->item(songIdx,
                                               tableWidget->columnCount()-1);
    song->enqueue(Phonon::MediaSource(item->text()));
}

void MediaPlayerUI::seekLeft()
{
}

void MediaPlayerUI::seekRight()
{
}
