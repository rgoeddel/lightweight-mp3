#include <QtGui>
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
    playing = false;
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

    // Slider for current song
    seekSlider = new QSlider(Qt::Horizontal, this);
    seekSlider->setMinimum(0);
    seekSlider->setMaximum(100);
    seekSlider->setTickInterval(1);
    // XXX: Set the tick interval...
    // XXX: Connect actions to song playback

    // Buttons
    // XXX: Toggle from play to pause
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
    if (playing) {
        playAction->setIcon(QPixmap("../resources/icons/playButton.png"));
        playing = false;
    } else {
        playAction->setIcon(QPixmap("../resources/icons/pauseButton.png"));
        playing = true;
    }
}

void MediaPlayerUI::seekLeft()
{
    int cval = seekSlider->value();
    seekSlider->setValue(max(seekSlider->minimum(), cval-1));
}

void MediaPlayerUI::seekRight()
{
    int cval = seekSlider->value();
    seekSlider->setValue(min(seekSlider->maximum(), cval+1));
}
