#include "dialog.h"
#include "ui_dialog.h"

//定义全局变量PI
const static double PI = 3.1416;

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_countBtn_clicked()
{
    bool ok;
    QString tempStr;
    QString valueStr = ui->radiusLineEdit->text();
    int valueInt = valueStr.toInt(&ok);
    //计算圆面积
    double area = PI * valueInt * valueInt;
    ui->areaLabel_2->setText(tempStr.setNum(area));
}