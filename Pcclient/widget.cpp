#include "widget.h"
#include "ui_widget.h"
#include<QDebug>
#include<QHostAddress>
#include<QBuffer>
#include<QPixmap>
#include<QPalette>
#include<QFileDialog>
#include<QDateTime>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_pTcpSocket = NULL;
    m_imageTcpSocket = NULL;
    m_timmer = NULL;
    m_ImageLen = 0;
    m_bReadFlag = 1;
    m_bRecoder = 0;
    initCtrls();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initCtrls()//初始化控件
{
    this->setWindowTitle("Super Car");
    this->setWindowFlags(Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint);
    this->setWindowIcon(QIcon(":/che1.jpg"));

    m_pTcpSocket = new QTcpSocket(this);
    m_imageTcpSocket = new QTcpSocket(this);
    m_timmer = new QTimer(this);
    m_DisTimmer = new QTimer(this);
    m_Distance = 0;
    ui->Btn_down->setEnabled(false);
    ui->Btn_left->setEnabled(false);
    ui->Btn_up->setEnabled(false);
    ui->Btn_right->setEnabled(false);
    ui->Btn_stop->setEnabled(false);
    ui->pushButton_leftDown->setEnabled(false);
    ui->pushButton_rightDown->setEnabled(false);
    ui->Btn_record->setEnabled(false);

}
//控制部分网络连接
void Widget::on_Btn_connect_clicked()
{
    if(m_pTcpSocket == NULL)
    {
        return ;
    }
    m_pTcpSocket->abort();
    if("connect" == ui->Btn_connect->text())
    {
        m_pTcpSocket->connectToHost(QHostAddress("192.168.1.1"),2001);
        connect(m_pTcpSocket,SIGNAL(connected()),this,SLOT(connectOk()));
    }
    else
    {
        on_Btn_stop_pressed();
        ui->Btn_connect->setText("connect");
        ui->Btn_down->setEnabled(false);
        ui->Btn_left->setEnabled(false);
        ui->Btn_up->setEnabled(false);
        ui->Btn_right->setEnabled(false);
        ui->Btn_stop->setEnabled(false);
        ui->pushButton_leftDown->setEnabled(false);
        ui->pushButton_rightDown->setEnabled(false);
        ui->Btn_record->setEnabled(false);
        m_DisTimmer->stop();
        disconnect(m_pTcpSocket,SIGNAL(connected()),this,SLOT(connectOk()));
    }
}

//右键按下

void Widget::on_Btn_right_pressed()
{
    if(!m_DisTimmer->isActive())
    {
        m_DisTimmer->start(1000);
    }
     QByteArray sndData;
     unsigned char cmdStr[5] = {0xff,0x00,0x04,0x00,0xff};
     int i = 0;
     for(i = 0;i<5;++i)
     {
         sndData.append(cmdStr[i]);
     }
     m_pTcpSocket->write(sndData);
}

//上键按下
void Widget::on_Btn_up_pressed()
{
    if(!m_DisTimmer->isActive())
    {
        m_DisTimmer->start(1000);
    }
    QByteArray sndData;
     unsigned char cmdStr[5] = {0xff,0x00,0x01,0x00,0xff};
     int i = 0;
     for(i = 0;i<5;++i)
     {
         sndData.append(cmdStr[i]);
     }
     m_pTcpSocket->write(sndData);
}
//左键按下
void Widget::on_Btn_left_pressed()
{
    if(!m_DisTimmer->isActive())
    {
        m_DisTimmer->start(1000);
    }
    QByteArray sndData;
     unsigned char cmdStr[5] = {0xff,0x00,0x03,0x00,0xff};
     int i = 0;
     for(i = 0;i<5;++i)
     {
         sndData.append(cmdStr[i]);
     }
     m_pTcpSocket->write(sndData);
}
//下键按下
void Widget::on_Btn_down_pressed()
{
    if(!m_DisTimmer->isActive())
    {
        m_DisTimmer->start(1000);
    }
    QByteArray sndData;
     unsigned char cmdStr[5] = {0xff,0x00,0x02,0x00,0xff};
     int i = 0;
     for(i = 0;i<5;++i)
     {
         sndData.append(cmdStr[i]);
     }
     m_pTcpSocket->write(sndData);
}
//控制部分连接服务器成功
void Widget::connectOk()
{
    qDebug()<<"connect Ok";
    m_Distance = 0;
    initLcd();
    connect(m_DisTimmer,SIGNAL(timeout()),this,SLOT(ShowDistance()));
    ui->Btn_connect->setText("disconnect");
    ui->Btn_down->setEnabled(true);
    ui->Btn_left->setEnabled(true);
    ui->Btn_up->setEnabled(true);
    ui->Btn_right->setEnabled(true);
    ui->Btn_stop->setEnabled(true);
    ui->Btn_record->setEnabled(true);
    ui->pushButton_leftDown->setEnabled(true);
    ui->pushButton_rightDown->setEnabled(true);   
}

//停止按键
void Widget::on_Btn_stop_pressed()
{
    if(m_DisTimmer->isActive())
    {
       m_DisTimmer->stop();
    }
    QByteArray sndData;
    unsigned char cmdStr[5] = {0xff,0x00,0x00,0x00,0xff};
    int i = 0;
    for(i = 0;i<5;++i)
    {
        sndData.append(cmdStr[i]);
    }
    m_pTcpSocket->write(sndData);

}
//左倒车
void Widget::on_pushButton_leftDown_clicked()
{
    if(!m_DisTimmer->isActive())
    {
        m_DisTimmer->start(1000);
    }
    QByteArray sndData;
    unsigned char cmdStr[5] = {0xff,0x00,0x06,0x00,0xff};
    int i = 0;
    for(i = 0;i<5;++i)
    {
        sndData.append(cmdStr[i]);
    }
    m_pTcpSocket->write(sndData);

}
//右倒车
void Widget::on_pushButton_rightDown_clicked()
{
    if(!m_DisTimmer->isActive())
    {
        m_DisTimmer->start(1000);
    }
    QByteArray sndData;
    unsigned char cmdStr[5] = {0xff,0x00,0x08,0x00,0xff};
    int i = 0;
    for(i = 0;i<5;++i)
    {
        sndData.append(cmdStr[i]);
    }
    m_pTcpSocket->write(sndData);

}
//显示距离
void Widget::ShowDistance()
{
    if(m_Distance == 999)
    {
        m_Distance = 0;
    }
    m_Distance += 0.61;
    ui->lcd_distance->display(QString::number(m_Distance));
}
//初始化LCD
void Widget::initLcd()
{
    ui->lcd_distance->setSegmentStyle(QLCDNumber::Flat);
    ui->lcd_distance->setDigitCount(6);
    ui->lcd_distance->setMode(QLCDNumber::Dec);
    QPalette lcdpat ;
    lcdpat.setColor(QPalette::Normal,QPalette::WindowText,Qt::blue);
    ui->lcd_distance->setPalette(lcdpat);
    //ui->lcd_distance->setStyleSheet("background-color:red");
}
/********************************************************/
//视屏连接
void Widget::on_Btn_connectVession_clicked()
{
    if(m_imageTcpSocket == NULL)
    {
        return ;
    }
    m_imageTcpSocket->abort();
    if("connect" == ui->Btn_connectVession->text())
    {
        m_imageTcpSocket->connectToHost(QHostAddress("192.168.1.1"),8888);
        connect(m_imageTcpSocket,SIGNAL(connected()),this,SLOT(connectImageOk()));
    }
    else
    {
        ui->Btn_connectVession->setText("connect");
        m_imageTcpSocket->abort();
        m_timmer->stop();
        disconnect(m_imageTcpSocket,SIGNAL(connected()),this,SLOT(connectImageOk()));
    }
}
//连接视屏成功
void Widget::connectImageOk()
{
    ui->Btn_connectVession->setText("disconnect");
    connect(m_timmer,SIGNAL(timeout()),this,SLOT(timmerEvent()));
    m_timmer->start(100);
}

//视屏接收
void Widget::timmerEvent()
{

    if(m_bReadFlag)  //判断是否能够开始接受图片数据，若上一帧图片数据还未接受完则不能继续读取
    {
        QByteArray SndData;
        char CheckStr[3] = {'p','i','c'};
        SndData.append(CheckStr[0]);
        SndData.append(CheckStr[1]);
        SndData.append(CheckStr[2]);
        m_imageTcpSocket->write(SndData);


    char response[24] = {0};
    if(m_imageTcpSocket->bytesAvailable() >= 24)    //读取图片信息区24字节
    {
        m_bReadFlag = 0;
        if(!m_imageTcpSocket->read(response,24))
        {
            return;
        }
    }
    else
    {
        return;
    }
  //  qDebug()<<response<<"response";

   int idataLen = 0;
   int i = 0;
   while(response[i] >='0' && response[i] <= '9')  //循环解析接受的24字节字符串获取图片字节长度
   {
       idataLen = idataLen*10 + (int)(response[i]-'0');
       i++;
   }
   if(idataLen <= 0)
   {
       qDebug()<<"error";
       return;
   }
  // qDebug()<<idataLen;
   m_ImageLen = idataLen;
  }
   QByteArray ImageData;
   if(m_imageTcpSocket->bytesAvailable() >= m_ImageLen)      //判断可读字节是否达到上面求出的图片长度
   {
       ImageData.append((QByteArray)m_imageTcpSocket->read(m_ImageLen));
       qDebug()<<ImageData.size();
       m_bReadFlag = 1;
       m_ImageLen = 0;
   }
   else
   {
       m_bReadFlag = 0;
       return;
   }
   //显示图片
   if(m_ImageLen != 0)
   {
       return;
   }
   QImage img =  QImage::fromData(ImageData);
   ui->labelPicture->setScaledContents(true);
   ui->labelPicture->setPixmap(QPixmap::fromImage(img));
   if(m_bRecoder)//保存行车记录
   {
       qDebug()<<"recording";
       m_bRecoder = 0;
       //QString Filename = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
       img.save("record.jpg","JPG");
   }
}
//********************************************************************************/
//是否记录行车记录
void Widget::on_Btn_record_clicked()
{
    m_bRecoder = 1;
}
