#include <QtGui>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <iostream>
#include <cmath>

#include "mediaPlayerUI.h"

using namespace std;

MediaPlayerUI::MediaPlayerUI()
{
    setWindowTitle(tr("Lightweight MP3"));

    initState();
    initActions();
    initMenu();
    initGUI();
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
    quitAction = new QAction(tr("&Quit"), this);
    playAction = new QAction(QPixmap("../resources/icons/playButton.png"), 0, this);
    seekLeftAction = new QAction(QPixmap("../resources/icons/seekLeft.png"), 0, this);
    seekRightAction = new QAction(QPixmap("../resources/icons/seekRight.png"), 0, this);

    // Connections
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
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

    // Slider for current song
    seekSlider = new Phonon::SeekSlider(song, this);

    // Buttons
    QToolBar *bar = new QToolBar;
    bar->addAction(seekLeftAction);
    bar->addAction(playAction);
    bar->addAction(seekRightAction);

    // Layout
    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(seekSlider);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(bar);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(sliderLayout);
    mainLayout->addLayout(buttonLayout);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
}

// === SLOTS =============
void MediaPlayerUI::open()
{
    cout << "open" << endl;
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
