#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QString>
#include <QFileDialog>
#include <QDirModel>
#include <QFileInfo>
#include <QDateTime>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public://定义公共变量
    QString name;

    QDateTime file_buildTime;//文件创建时间
    QString file_year;//文件创建年份
    QString file_month;//文件创建月份
    QString file_day;//文件创建日
    QString file_hour;//文件创建小时
    QString file_minute;//文件创建分钟
    QString file_second;//文件创建秒

    int set_minute;//函数被执行的分钟数
    //int temp_second;//函数被执行的秒数，若精确到秒则使用

    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

    QTcpServer *messageServer;//定义服务器
    QTcpServer *fileServer;
    QTcpSocket *receivedMessageSocket;
    QTcpSocket *receivedFileSocket;
    QFile *newFile;//新文件指针
    QFile *processFile;//正在处理的文件指针

    QByteArray inBlock;
    QString fileName;//文件名
    QString file_suffix;
    QFileInfo fileinfo;//文件信息
    QString file_full;
    QString suffix;
    QString currentStr;//当前文件地址
    qint64 totalSize;  //总共需要发送的文件大小（文件内容&文件名信息）
    qint64 byteReceived;  //已经发送的大小

    QTcpSocket *tcpMessageClient;//文字套接字
    QTcpSocket *tcpFileClient;//文件套接字

    QFile *localFile;//本地文件指针

    QByteArray outBlock;  //分次传
    qint64 loadSize;  //每次发送数据的大小
    qint64 byteToWrite;  //剩余数据大小

    QString ip;
    int messagePort;
    int filePort;
    int sendTimes;  //用来标记是否为第一次发送，第一次以后连接信号触发，后面的则手动调用

    //QString dir = "D:/123/";

private slots:
    void acceptConnection();//建立链接
    void readClient();//读取客户端内容
    void time_delete();//定时执行函数
    void on_createButton_clicked();//按键执行函数
    void send();//传送文件头信息
    void goOnSend(qint64);//传送文件内容
    void on_sendFileButton_clicked();//send按钮信号操作
    void readSlot();

    void on_connectButton_clicked();
    void on_sendMessageButton_clicked();
};

#endif // WIDGET_H
