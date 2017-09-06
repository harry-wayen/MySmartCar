#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QTcpSocket>
#include<QThread>
#include<QTimer>
#include<QBuffer>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_Btn_connect_clicked();
    void on_Btn_right_pressed();
    void on_Btn_up_pressed();
    void on_Btn_left_pressed();
    void on_Btn_down_pressed();
    void connectOk();
    void on_Btn_stop_pressed();

    void on_Btn_connectVession_clicked();
    void connectImageOk();
    void timmerEvent();
    void on_pushButton_leftDown_clicked();

    void on_pushButton_rightDown_clicked();
    void ShowDistance();
    void initLcd();

    void on_Btn_record_clicked();

private:
    void initCtrls();
    void saveImage();

private:
    Ui::Widget *ui;
    QTcpSocket * m_pTcpSocket;
    QTcpSocket * m_imageTcpSocket;
    QTimer     * m_timmer;
    int m_ImageLen;
    bool m_bReadFlag;
    QTimer  *m_DisTimmer;
    double m_Distance;
    bool m_bRecoder;

};

#endif // WIDGET_H
