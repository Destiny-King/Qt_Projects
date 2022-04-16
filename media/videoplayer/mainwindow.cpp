#include "mainwindow.h"
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //视频播放器布局初始化
    videoLayout();

    //媒体初始化
    mediaPlayerInit();

    //扫描本地视频
    scanVideoFiles();

    //设置按钮属性
    pushButton[0]->setCheckable(true);
    pushButton[4]->setCheckable(true);

    //按钮连接信号槽
    connect(pushButton[0],SIGNAL(clicked()),this,SLOT(btn_play_clicked()));
    connect(pushButton[1],SIGNAL(clicked()),this,SLOT(btn_next_clicked()));
    connect(pushButton[2],SIGNAL(clicked()),this,SLOT(btn_volumedown_clicked()));
    connect(pushButton[3],SIGNAL(clicked()),this,SLOT(btn_volumeup_clicked()));
    connect(pushButton[4],SIGNAL(clicked()),this,SLOT(btn_fullscreen_clicked()));

    //列表连接信号槽
    connect(listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(listWidgetClicked(QListWidgetItem *)));

    //媒体连接信号槽
    connect(videoPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(mediaPlayerStateChanged(QMediaPlayer::State)));
    connect(mediaPlayerList,SIGNAL(currentIndexChanged(int)),this,SLOT(mediaPlaylistCurrentIndexChanged(int)));
    connect(videoPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(mediaPlayDurationChanged(qint64)));
    connect(videoPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(mediaPlayPositionChanged(qint64)));

    //slider信号槽连接
    connect(durationSlider,SIGNAL(sliderReleased()),this,SLOT(durationSliderReleased()));
    connect(volumeSlider,SIGNAL(sliderReleased()),this,SLOT(volumeSliderReleased()));

}

MainWindow::~MainWindow()
{
}

//视频布局函数
void MainWindow::videoLayout()
{
    //设置位置与大小
    this->setGeometry(0,0,800,480);

    QPalette pal;
    pal.setColor(QPalette::WindowText,Qt::white);

    //水平
    for (int i =0;i<3;i++) {
        //水平容器
        hWidget[i] = new QWidget();
        hWidget[i]->setAutoFillBackground(true);

        //水平布局
        hBoxLayout[i] = new QHBoxLayout();
    }

    //垂直
    for (int i =0;i<2;i++) {
        //垂直容器
        vWidget[i] = new QWidget();
        vWidget[i]->setAutoFillBackground(true);

        //垂直布局
        vBoxLayout[i] = new QVBoxLayout();
    }

    for (int i = 0;i<2;i++) {
        label[i] = new QLabel();
    }

    for (int i = 0;i<5;i++) {
        pushButton[i] = new QPushButton();
        pushButton[i]->setMinimumSize(44,44);
        pushButton[i]->setMaximumSize(44,44);
    }

    //设置
    vWidget[0]->setObjectName("vWidget0");
    vWidget[1]->setObjectName("vWidget1");
    hWidget[1]->setObjectName("hWidget1");
    hWidget[2]->setObjectName("hWidget2");
    pushButton[0]->setObjectName("btn_play");
    pushButton[1]->setObjectName("btn_next");
    pushButton[2]->setObjectName("btn_volumedown");
    pushButton[3]->setObjectName("btn_volumeup");
    pushButton[4]->setObjectName("btn_screen");

    QFont font;
    font.setPixelSize(18);
    label[0]->setFont(font);
    label[1]->setFont(font);

    pal.setColor(QPalette::WindowText,Qt::white);
    label[0]->setPalette(pal);
    label[1]->setPalette(pal);

    label[0]->setText("00:00");
    label[1]->setText("/00:00");

    //播放进度条
    durationSlider = new QSlider(Qt::Horizontal);
    durationSlider->setMaximumHeight(15);
    durationSlider->setObjectName("durationSlider");

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0,100);
    volumeSlider->setMaximum(80);
    volumeSlider->setObjectName("volumeSlider");
    volumeSlider->setValue(50);

    //视频列表
    listWidget = new QListWidget();
    listWidget->setObjectName("listWidget");
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //视频显示窗口
    videoWidget = new QVideoWidget();
    videoWidget->setStyleSheet("border-image:none;"
                               "background:transparent;"
                               "border:none");

    //H0布局
    //垂直容器
    vWidget[0]->setMinimumSize(300,480);
    vWidget[0]->setMaximumWidth(300);
    videoWidget->setMinimumSize(500,480);

    //水平布局
    hBoxLayout[0]->addWidget(videoWidget);
    hBoxLayout[0]->addWidget(vWidget[0]);
    hWidget[0]->setLayout(hBoxLayout[0]);
    hBoxLayout[0]->setContentsMargins(0,0,0,0);

    setCentralWidget(hWidget[0]);

    //V0布局
    QSpacerItem *vSpacer0 = new QSpacerItem(0,80,QSizePolicy::Minimum,QSizePolicy::Maximum);
    vBoxLayout[0]->addWidget(listWidget);
    vBoxLayout[0]->addSpacerItem(vSpacer0);
    vBoxLayout[0]->setContentsMargins(0,0,0,0);
    vWidget[0]->setLayout(vBoxLayout[0]);

    //V1布局
    //底板部件布局
    hWidget[1]->setMaximumHeight(15);
    hWidget[2]->setMinimumHeight(65);
    vBoxLayout[1]->addWidget(hWidget[1]);
    vBoxLayout[1]->addWidget(hWidget[2]);
    vBoxLayout[1]->setAlignment(Qt::AlignCenter);

    vWidget[1]->setLayout(vBoxLayout[1]);
    vWidget[1]->setParent(this);
    vWidget[1]->setGeometry(0,this->height() - 80,this->width(),80);
    vBoxLayout[1]->setContentsMargins(0,0,0,0);

    //位于最上层
    vWidget[1]->raise();

    //H1布局
    hBoxLayout[1]->addWidget(durationSlider);
    hBoxLayout[1]->setContentsMargins(0,0,0,0);
    hWidget[1]->setLayout(hBoxLayout[1]);

    //H2布局
    QSpacerItem *hSpacer0 = new QSpacerItem(300,80,QSizePolicy::Expanding,QSizePolicy::Maximum);

    hBoxLayout[2]->addSpacing(20);
    hBoxLayout[2]->addWidget(pushButton[0]);
    hBoxLayout[2]->addSpacing(10);
    hBoxLayout[2]->addWidget(pushButton[1]);
    hBoxLayout[2]->addSpacing(10);
    hBoxLayout[2]->addWidget(pushButton[2]);
    hBoxLayout[2]->addWidget(volumeSlider);
    hBoxLayout[2]->addWidget(pushButton[3]);
    hBoxLayout[2]->addWidget(label[0]);
    hBoxLayout[2]->addWidget(label[1]);
    hBoxLayout[2]->addSpacerItem(hSpacer0);
    hBoxLayout[2]->addWidget(pushButton[4]);
    hBoxLayout[2]->addSpacing(20);
    hBoxLayout[2]->setContentsMargins(0,0,0,0);
    hBoxLayout[2]->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    hWidget[2]->setLayout(hBoxLayout[2]);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    vWidget[1]->setGeometry(0,this->height() - 80,this->width(),80);
}

void MainWindow::scanVideoFiles()
{
    QDir dir(QCoreApplication::applicationDirPath() + "/video");
    QDir dirabsolutePath(dir.absolutePath());
    //如果目录存在
    if(dirabsolutePath.exists()){
        //定义过滤器
        QStringList filter;
        //包含所有xx后缀的文件
        filter << "*.mp4" << "*.mkv" << "*.wmv" <<"*.avi" <<"*.flv" <<"*.ts";
        //获取该目录下的所有文件
        QFileInfoList files = dirabsolutePath.entryInfoList(filter,QDir::Files);
        //遍历
        for (int i = 0;i<files.count();i++) {
            MediaObjectInfo info;
            //使用utf-8编码
            info.fileName = QString::fromUtf8(files.at(i).fileName().toUtf8().data());
            info.filePath = QString::fromUtf8(files.at(i).filePath().toUtf8().data());
            //媒体列表添加视频
            if(mediaPlayerList->addMedia(QUrl::fromLocalFile(info.filePath))){
                //添加到容器数组中储存
                mediaObjectInfo.append(info);
                //添加视频名字到列表
                listWidget->addItem(info.fileName);
            }else{
                qDebug() << mediaPlayerList->errorString().toUtf8().data() << endl;
                qDebug() << "Error number:" <<mediaPlayerList->error() << endl;
            }
        }
    }
}

//媒体初始化
void MainWindow::mediaPlayerInit()
{
    videoPlayer = new QMediaPlayer(this);
    mediaPlayerList = new QMediaPlaylist(this);
    //确保列表是空的
    mediaPlayerList->clear();
    //设置视频播放器的列表是mediaPlayerLiset
    videoPlayer->setPlaylist(mediaPlayerList);
    //设置视频输出窗口
    videoPlayer->setVideoOutput(videoWidget);
    //设置播放模式 Loop是列循环
    mediaPlayerList->setPlaybackMode(QMediaPlaylist::Loop);
    //设置默认软件音量为50%
    videoPlayer->setVolume(50);
}

void MainWindow::btn_play_clicked()
{
    int state = videoPlayer->state();
    switch (state) {
        case QMediaPlayer::StoppedState:
        //媒体播放
        videoPlayer->play();
        break;

        case QMediaPlayer::PlayingState:
        //媒体暂停
        videoPlayer->pause();
        break;

    case QMediaPlayer::PausedState:
        //媒体播放
        videoPlayer->play();
        break;
    }
}

void MainWindow::btn_next_clicked()
{
    videoPlayer->stop();
    int count = mediaPlayerList->mediaCount();
    if(count == 0) return;

    //列表下一个
    mediaPlayerList->next();
    videoPlayer->play();
}

void MainWindow::btn_volumeup_clicked()
{
    //点击每次音量+5
    volumeSlider->setValue(volumeSlider->value() + 5);
    videoPlayer->setVolume(volumeSlider->value());
}

void MainWindow::btn_volumedown_clicked()
{
    //点击每次音量-5
    volumeSlider->setValue(volumeSlider->value() - 5);
    videoPlayer->setVolume(volumeSlider->value());
}

void MainWindow::btn_fullscreen_clicked()
{
    //全屏/非全屏操作
    vWidget[0]->setVisible(!pushButton[4]->isChecked());
}

void MainWindow::mediaPlayerStateChanged(QMediaPlayer::State state)
{
    switch (state) {
        case QMediaPlayer::StoppedState:
        pushButton[0]->setChecked(false);
        break;

        case QMediaPlayer::PlayingState:
        pushButton[0]->setChecked(true);
        break;

        case QMediaPlayer::PausedState:
        pushButton[0]->setChecked(false);
        break;
    }
}

void MainWindow::listWidgetClicked(QListWidgetItem *item)
{
    videoPlayer->stop();
    mediaPlayerList->setCurrentIndex(listWidget->row(item));
    videoPlayer->play();
}

void MainWindow::mediaPlaylistCurrentIndexChanged(int index)
{
    if(index == -1)
        return;

    //设置列表正在播放的项
    listWidget->setCurrentRow(index);
}

void MainWindow::mediaPlayDurationChanged(qint64 duration)
{
    durationSlider->setRange(0,duration / 1000);
    int second = duration / 1000;
    int minute = second / 60;
    second %= 60;

    QString mediaDuration;
    mediaDuration.clear();

    if(minute >= 10)
        mediaDuration = QString::number(minute,10);
    else
        mediaDuration = "0" + QString::number(minute,10);

    if(second >= 10)
        mediaDuration = mediaDuration + ":" +QString::number(second,10);
    else
        mediaDuration = mediaDuration + ":0" +QString::number(second,10);

    //显示媒体总长度时间
    label[1]->setText("/" + mediaDuration);
}

void MainWindow::mediaPlayPositionChanged(qint64 position)
{
    if(!durationSlider->isSliderDown())
        durationSlider->setValue(position / 1000);

    int second = position / 1000;
    int minute = second / 60;
    second %= 60;

    QString mediaPosition;
    mediaPosition.clear();
    if(minute >= 10)
        mediaPosition = QString::number(minute,10);
    else
        mediaPosition = "0" + QString::number(minute,10);

    if(second >= 10)
        mediaPosition = mediaPosition + ":" +QString::number(second,10);
    else
        mediaPosition = mediaPosition + ":0" +QString::number(second,10);

    //显示现在播放的时间
    label[1]->setText("/" + mediaPosition);
}

void MainWindow::durationSliderReleased()
{
    //设置媒体播放位置
    videoPlayer->setPosition(durationSlider->value() * 1000);
}

void MainWindow::volumeSliderReleased()
{
    //设置音量
    videoPlayer->setVolume(volumeSlider->value());
}
