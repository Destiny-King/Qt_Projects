#include "iconhelper.h"
#include <QPainter>
#include <QFontDatabase>
#include <QFile>
#include <QDebug>
#include <QMouseEvent>
#include <QIcon>

IconHelper::IconHelper(const QString &fontFile, const QString &fontName, QObject *parent)
    : QObject(parent)
{
    //判断图形字体是否存在，不存在则加入
    QFontDatabase fontDb;
    if(!fontDb.families().contains(fontName) && QFile(fontFile).exists()){
        int fontId = fontDb.addApplicationFont(fontFile);
        QStringList listName = fontDb.applicationFontFamilies(fontId);
        if(listName.count() == 0){
            qDebug() << QString("load %1 error").arg(fontName);
        }
    }
    //再次判断是否包含字体名称防止加载失败
    if(fontDb.families().contains(fontName)){
        iconFont = QFont(fontName);
        iconFont.setHintingPreference(QFont::PreferNoHinting);
    }
}

IconHelper *IconHelper::iconFontAliBaBa = 0;
IconHelper *IconHelper::iconFontAwesome = 0;
IconHelper *IconHelper::iconFontAwesome6 = 0;
IconHelper *IconHelper::iconFontWeather = 0;
int IconHelper::iconFontIndex = -1;

void IconHelper::initFont()
{
    static bool isInit = false;
    if(!isInit){
        isInit = true;
        if(iconFontAliBaBa == 0){
            iconFontAliBaBa = new IconHelper(":/font/iconfont.ttf","iconfont");
        }
        if (iconFontAwesome == 0) {
            iconFontAwesome = new IconHelper(":/font/fontawesome-webfont.ttf", "FontAwesome");
        }
        if (iconFontAwesome6 == 0) {
            iconFontAwesome6 = new IconHelper(":/font/fa-regular-400.ttf", "Font Awesome 6 Pro Regular");
        }
        if (iconFontWeather == 0) {
            iconFontWeather = new IconHelper(":/font/pe-icon-set-weather.ttf", "pe-icon-set-weather");
        }
    }
}

void IconHelper::setIconFontIndex(int index)
{
    iconFontIndex = index;
}

QFont IconHelper::getIconFontAliBaBa()
{
    initFont();
    return iconFontAliBaBa->getIconFont();
}

QFont IconHelper::getIconFontAwesome()
{
    initFont();
    return iconFontAwesome->getIconFont();
}

QFont IconHelper::getIconFontAwesome6()
{
    initFont();
    return iconFontAwesome6->getIconFont();
}

QFont IconHelper::getIconFontWeather()
{
    initFont();
    return iconFontWeather->getIconFont();
}

IconHelper *IconHelper::getIconHelper(int icon)
{
    initFont();

    IconHelper *iconHelper = iconFontAwesome;
    if(iconFontIndex < 0){
        if((icon > 0xe501 && icon < 0xe793) || (icon > 0xe8d5 && icon < 0xea5d)){
            iconHelper = iconFontAliBaBa;
        }
    }else if(iconFontIndex == 0){
        iconHelper = iconFontAliBaBa;
    }else if(iconFontIndex == 1){
        iconHelper = iconFontAwesome;
    }else if (iconFontIndex == 2) {
        iconHelper = iconFontAwesome6;
    } else if (iconFontIndex == 3) {
        iconHelper = iconFontWeather;
    }
    return iconHelper;
}

void IconHelper::setIcon(QLabel *lab, int icon, quint32 size)
{
    getIconHelper(icon)->setIcon1(lab,icon,size);
}

void IconHelper::setIcon(QAbstractButton *btn, int icon, quint32 size)
{
    getIconHelper(icon)->setIcon1(btn,icon,size);
}

//设置图形字体到图标
void IconHelper::setPixmap(QAbstractButton *btn, const QColor &color, int icon, quint32 size, quint32 width, quint32 height, int flags)
{
    getIconHelper(icon)->setPixmap1(btn,color,icon,size,width,height,flags);
}

QPixmap IconHelper::getPixmap(const QColor &color, int icon, quint32 size, quint32 width, quint32 height, int flags)
{
    return getIconHelper(icon)->getPixmap1(color,icon,size,width,height,flags);
}

void IconHelper::setStyle(QWidget *widget, QList<QPushButton *> btns, QList<int> icons, const IconHelper::StyleColor &styleColor)
{
    int icon = icons.first();
    getIconHelper(icon)->setStyle1(widget,btns,icons,styleColor);
}

void IconHelper::setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> icons, const IconHelper::StyleColor &styleColor)
{
    int icon = icons.first();
    getIconHelper(icon)->setStyle1(widget,btns,icons,styleColor);
}

void IconHelper::setStyle(QWidget *widget, QList<QAbstractButton *> btns, QList<int> icons, const IconHelper::StyleColor &styleColor)
{
    int icon = icons.first();
    getIconHelper(icon)->setStyle1(widget,btns,icons,styleColor);
}

//watched:监视的对象 event:过滤的事件
bool IconHelper::eventFilter(QObject *watched, QEvent *event)
{
    if(watched->inherits("QAbstractButton")){
        QAbstractButton *btn = (QAbstractButton *)watched;
        //设置索引位置
        int index = btns.indexOf(btn);
        if(index >= 0){
            //不同的事件设置不同的图标，同时区分选中的和没有选中的
            if(btn->isChecked()){
                if(event->type() == QEvent::MouseButtonPress){
                    QMouseEvent *mouseEvent = (QMouseEvent *)event;
                    if(mouseEvent->button() == Qt::LeftButton){
                        btn->setIcon(QIcon(pixChecked.at(index)));
                    }
                }else if(event->type() == QEvent::Enter){
                    btn->setIcon(QIcon(pixChecked.at(index)));
                }else if(event->type() == QEvent::Leave){
                    btn->setIcon(QIcon(pixChecked.at(index)));
                }
            }else{
                if(event->type() == QEvent::MouseButtonPress){
                    QMouseEvent *mouseEvent = (QMouseEvent *)event;
                    if(mouseEvent->button() == Qt::LeftButton){
                        btn->setIcon(QIcon(pixPressed.at(index)));
                    }
                }else if(event->type() == QEvent::Enter){
                    btn->setIcon(QIcon(pixHover.at(index)));
                }else if(event->type() == QEvent::Leave){
                    btn->setIcon(QIcon(pixNormal.at(index)));
                }
            }
        }
    }
    return QObject::eventFilter(watched,event);
}

//按钮选中状态切换处理
void IconHelper::toggled(bool checked)
{
    //选中和不选中设置不同的图标
    QAbstractButton *btn = (QAbstractButton *)sender();
    int index = btns.indexOf(btn);
    if(checked){
        btn->setIcon(QIcon(pixChecked.at(index)));
    }else{
        btn->setIcon(QIcon(pixNormal.at(index)));
    }
}

QFont IconHelper::getIconFont()
{
    return this->iconFont;
}

void IconHelper::setIcon1(QLabel *lab, int icon, quint32 size)
{
    iconFont.setPixelSize(size);
    lab->setFont(iconFont);
    lab->setText((QChar)icon);
}

void IconHelper::setIcon1(QAbstractButton *btn, int icon, quint32 size)
{
    iconFont.setPixelSize(size);
    btn->setFont(iconFont);
    btn->setText((QChar)icon);
}

void IconHelper::setPixmap1(QAbstractButton *btn, const QColor &color, int icon, quint32 size, quint32 width, quint32 height, int flags)
{
    btn->setIcon(getPixmap1(color,icon,size,width,height,flags));
}

QPixmap IconHelper::getPixmap1(const QColor &color, int icon, quint32 size, quint32 width, quint32 height, int flags)
{
    //主动绘制图形字体到图片
    QPixmap pix(width,height);
    //填充透明的黑色值
    pix.fill(Qt::transparent);

    QPainter painter;
    //开始绘制设备
    painter.begin(&pix);
    //设置渲染提示
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(color);

    //设置字体像素
    iconFont.setPixelSize(size);
    //设置字体
    painter.setFont(iconFont);
    //绘制文本
    painter.drawText(pix.rect(),flags,(QChar)icon);
    //结束绘制
    painter.end();
    return pix;
}

void IconHelper::setStyle1(QWidget *widget, QList<QPushButton *> btns, QList<int> icons, const IconHelper::StyleColor &styleColor)
{
    QList<QAbstractButton *> list;
    foreach (QPushButton *btn, btns) {
        list << btn;
    }
    setStyle(widget,list,icons,styleColor);
}

void IconHelper::setStyle1(QWidget *widget, QList<QToolButton *> btns, QList<int> icons, const IconHelper::StyleColor &styleColor)
{
    QList<QAbstractButton *> list;
    foreach (QToolButton *btn, btns) {
        list << btn;
    }
    setStyle(widget,list,icons,styleColor);
}

void IconHelper::setStyle1(QWidget *widget, QList<QAbstractButton *> btns, QList<int> icons, const IconHelper::StyleColor &styleColor)
{
    int btnCount = btns.count();
    int iconCount = icons.count();
    if(btnCount <= 0 || iconCount <= 0 || btnCount != iconCount){
        return;
    }

    QString position = styleColor.position;
    quint32 iconSize = styleColor.iconSize;
    quint32 iconWidth = styleColor.iconWidth;
    quint32 iconHeight = styleColor.iconHeight;
    quint32 borderWidth = styleColor.borderWidth;

    //根据不同位置计算边框
    QString strBorder;
    if(position == "top"){
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    }else if(position == "right"){
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (position == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (position == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    //如果图标是左侧显示则需要让没有选中的按钮左侧也有加深的边框,颜色为背景颜色
    //如果图标在文字上面而设置的边框是 top bottom 也需要启用加深边框
    QStringList qss;
    if (styleColor.defaultBorder) {
        qss << QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:solid;border-radius:0px;%2border-color:%3;color:%4;background:%5;}")
            .arg(position).arg(strBorder).arg(styleColor.normalBgColor).arg(styleColor.normalTextColor).arg(styleColor.normalBgColor);
    } else {
        qss << QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
            .arg(position).arg(styleColor.normalTextColor).arg(styleColor.normalBgColor);
    }

    //悬停+按下+选中
    qss << QString("QWidget[flag=\"%1\"] QAbstractButton:hover{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
        .arg(position).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.hoverTextColor).arg(styleColor.hoverBgColor);
    qss << QString("QWidget[flag=\"%1\"] QAbstractButton:pressed{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
        .arg(position).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.pressedTextColor).arg(styleColor.pressedBgColor);
    qss << QString("QWidget[flag=\"%1\"] QAbstractButton:checked{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
        .arg(position).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.checkedTextColor).arg(styleColor.checkedBgColor);

    //窗体背景颜色+按钮背景颜色
    qss << QString("QWidget#%1{background:%2;}")
        .arg(widget->objectName()).arg(styleColor.normalBgColor);
    qss << QString("QWidget>QAbstractButton{border-width:0px;background-color:%1;color:%2;}")
        .arg(styleColor.normalBgColor).arg(styleColor.normalTextColor);
    qss << QString("QWidget>QAbstractButton:hover{background-color:%1;color:%2;}")
        .arg(styleColor.hoverBgColor).arg(styleColor.hoverTextColor);
    qss << QString("QWidget>QAbstractButton:pressed{background-color:%1;color:%2;}")
        .arg(styleColor.pressedBgColor).arg(styleColor.pressedTextColor);
    qss << QString("QWidget>QAbstractButton:checked{background-color:%1;color:%2;}")
        .arg(styleColor.checkedBgColor).arg(styleColor.checkedTextColor);

    //设置样式表
    widget->setStyleSheet(qss.join(""));

    //可能会重复调用设置所以先要移除上一次的
    for (int i = 0; i < btnCount; i++) {
        for (int j = 0; j < this->btns.count(); j++) {
            if (this->btns.at(j) == btns.at(i)) {
                disconnect(btns.at(i), SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
                this->btns.at(j)->removeEventFilter(this);
                this->btns.removeAt(j);
                this->pixNormal.removeAt(j);
                this->pixHover.removeAt(j);
                this->pixPressed.removeAt(j);
                this->pixChecked.removeAt(j);
                break;
            }
        }
    }

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    int checkedIndex = -1;
    for (int i = 0; i < btnCount; i++) {
        int icon = icons.at(i);
        QPixmap pixNormal = getPixmap1(styleColor.normalTextColor, icon, iconSize, iconWidth, iconHeight);
        QPixmap pixHover = getPixmap1(styleColor.hoverTextColor, icon, iconSize, iconWidth, iconHeight);
        QPixmap pixPressed = getPixmap1(styleColor.pressedTextColor, icon, iconSize, iconWidth, iconHeight);
        QPixmap pixChecked = getPixmap1(styleColor.checkedTextColor, icon, iconSize, iconWidth, iconHeight);

        //记住最后选中的按钮
        QAbstractButton *btn = btns.at(i);
        if (btn->isChecked()) {
            checkedIndex = i;
        }

        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);
        connect(btn, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));

        this->btns << btn;
        this->pixNormal << pixNormal;
        this->pixHover << pixHover;
        this->pixPressed << pixPressed;
        this->pixChecked << pixChecked;
    }

    //主动触发一下选中的按钮
    if (checkedIndex >= 0) {
        QMetaObject::invokeMethod(btns.at(checkedIndex), "toggled", Q_ARG(bool, true));
    }
}
