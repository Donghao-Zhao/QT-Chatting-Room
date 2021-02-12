#include "widget.h"
#include "ui_widget.h"
#include <QTextCodec>
#include <QDir>
#include <QFileDialog>
#include <QTimer>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //ui->progressLabel->hide();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));//设置字符编码

    tcpMessageClient = new QTcpSocket(this);//创建新套接字
    tcpFileClient = new QTcpSocket(this);//创建新套接字
    sendTimes = 0;

    connect(tcpFileClient, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));
    ui->sendMessageButton->setShortcut(Qt::Key_Return);//将字母区回车键与登录按钮绑定在一起
}

void Widget::acceptConnection()
{
    ui->receivedStatusLabel->setText(tr("已连接!"));//显示当前状态

    receivedMessageSocket = messageServer->nextPendingConnection();
    receivedFileSocket = fileServer->nextPendingConnection();
    QObject::connect(receivedMessageSocket, SIGNAL(readyRead()),this,  SLOT(readSlot()));
    QObject::connect(receivedFileSocket, SIGNAL(readyRead()), this, SLOT(readClient()));//转到接收函数
}



void Widget::readSlot()
{
    //client继承自qiodevice
    QTextStream in(receivedMessageSocket);
    QString str = in.readAll();

    //ui->receiveTextEdit->setText(str);
    ui->personList->addItem("Other:");
    ui->messageList->addItem(str);
}

void Widget::on_sendMessageButton_clicked()
{
    QString str = ui->sendLineEdit->text();
    ui->sendLineEdit->clear();
    ui->personList->addItem("Me:");
    ui->messageList->addItem(str);

    //toLocal8Bit方法是将qstring转为qbytearray
    tcpMessageClient->write(str.toLocal8Bit());
}



void Widget::readClient()
{
    ui->receivedStatusLabel->setText(tr("正在接收文件..."));

    if(byteReceived == 0)  //才刚开始接收数据，此数据为文件信息
    {
        QDataStream in(receivedFileSocket);
        in>>totalSize>>byteReceived>>fileName;
        name = fileName;
        fileName = "Files/" + fileName;//获取文件名

        QDir p;//定义文件路径
        QDir t;//定义文件路径

        p.mkpath("D:/图片");//创建图片存储路径
        t.mkpath("D:/文本");//创建文本存储路径

        //文件后缀
        fileinfo = QFileInfo(name);//获取文件信息
        file_suffix = fileinfo.suffix();
        if(file_suffix=="jpg"){//创建新图片文件，类型为只写，保存文件地址
            newFile = new QFile("D:/图片/"+name);
            newFile->open(QFile::WriteOnly);
            currentStr = "D:/图片/"+name;
        }
        else if(file_suffix=="txt"){//创建新文本文件，类型为只写，保存文件地址
            newFile = new QFile("D:/文本/"+name);
            newFile->open(QFile::WriteOnly);
            currentStr = "D:/文本/"+name;
        }
    }
    else  //正式读取文件内容
    {
        inBlock = receivedFileSocket->readAll();//写文件内容

        byteReceived += inBlock.size();//写文件大小
        newFile->write(inBlock);//写文件内容
        newFile->flush();//刷新文件
    }

    if(byteReceived == totalSize)//若接收文件大小与总大小相同
    {
        ui->receivedStatusLabel->setText(tr("%1接收完成").arg(fileName));//显示接收完成
        ui->deleteLabel->setText(tr("下一分钟删除"));//显示文件删除信息

        inBlock.clear();//清理接收区
        byteReceived = 0;
        totalSize = 0;

        QFileInfo processFile(currentStr);//打开已接收的文件

        file_buildTime = processFile.created();//获取文件创建时间
        file_year = file_buildTime.toString(Qt::ISODate).mid(0,4);//获取文件创建年份
        file_month = file_buildTime.toString(Qt::ISODate).mid(5,2);//获取文件创建月份
        file_day = file_buildTime.toString(Qt::ISODate).mid(8,2);//获取文件创建日
        file_hour = file_buildTime.toString(Qt::ISODate).mid(11,2);//获取文件创建时
        file_minute = file_buildTime.toString(Qt::ISODate).mid(14,2);//获取文件创建分钟
        file_second = file_buildTime.toString(Qt::ISODate).mid(17,2);//获取文件创建秒

        //int temp_second = file_second.toInt();//将文件创建时间的秒数由QString类型转换为int类型
        set_minute = file_minute.toInt();//将文件创建时间的分钟数由QString类型转换为int类型
        set_minute += 1;//1单位时间后执行处理函数

        QTimer *timer = new QTimer(this);//设置定时器
        connect(timer, SIGNAL(timeout()), this, SLOT(time_delete()));//设置定时执行函数
        timer->start(1000);//每1秒执行一次
    }
}

void Widget::time_delete()//删除文件函数
{
    QDateTime current_time =QDateTime::currentDateTime();//获取当前时间
    QString cur_year = current_time.toString(Qt::ISODate).mid(0,4);//获取当前时间的年份
    QString cur_month = current_time.toString(Qt::ISODate).mid(5,2);//获取当前时间的月份
    QString cur_day = current_time.toString(Qt::ISODate).mid(8,2);//获取当前时间的天数
    QString cur_hour = current_time.toString(Qt::ISODate).mid(11,2);//获取当前时间的小时
    QString cur_minute = current_time.toString(Qt::ISODate).mid(14,2);//获取当前时间的分钟
    QString cur_second = current_time.toString(Qt::ISODate).mid(17,2);//获取当前时间的秒数

    if(set_minute==60)//进制转换若当前时间正好为59分，则设置执行时间为00分
    {
        set_minute = 00;
    }

    //int temp_second = file_second.toInt();//将文件创建时间由QString类型转换为int类型
    int temp_minute = cur_minute.toInt();//将当前时间由QString类型转换为int类型

    //QString setTime_second=QString::number(temp_second);//将设置的时间转换为QString类型，用于输出，可注释
    QString setTime_minute=QString::number(set_minute);//将设置的时间转换为QString类型，用于输出，可注释

    /*
    if(file_year==cur_year&&file_month==cur_month&&file_day==cur_day&&file_hour==cur_hour&&file_minute==cur_minute&&file_second==setTime_second){
        ui->lineEdit_9->setText(cur_year);
    }
    */

    newFile->close();//关闭文件

    if(set_minute==temp_minute){//若当前时间与设置的时间相同
        //ui->lineEdit_9->setText("Delete!");//显示已删除
        ui->deleteLabel->setText(tr("文件已被删除"));//显示文件已被删除
        QFile::remove(currentStr);//删除目标文件
    }

}

void Widget::send()  //发送文件头信息
{
    byteToWrite = localFile->size();  //剩余数据的大小
    totalSize = localFile->size();

    loadSize = 4*1024;  //每次发送数据的大小

    QDataStream out(&outBlock, QIODevice::WriteOnly);
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);

    out<<qint64(0)<<qint64(0)<<currentFileName;

    totalSize += outBlock.size();  //总大小为文件大小加上文件名等信息大小
    byteToWrite += outBlock.size();

    out.device()->seek(0);  //回到字节流起点来写好前面连个qint64，分别为总大小和文件名等信息大小
    out<<totalSize<<qint64(outBlock.size());

    tcpFileClient->write(outBlock);  //将读到的文件发送到套接字
}

void Widget::goOnSend(qint64 numBytes)  //开始发送文件内容
{
    byteToWrite -= numBytes;  //剩余数据大小
    outBlock = localFile->read(qMin(byteToWrite, loadSize));
    tcpFileClient->write(outBlock);

    if(byteToWrite == 0)  //发送完毕
        ui->sendStatusLabel->setText(tr("文件发送完毕!"));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_createButton_clicked()//监听函数
{
    totalSize = 0;
    byteReceived = 0;


    messageServer = new QTcpServer(this);//创建新的套接字连接
    fileServer = new QTcpServer(this);//创建新的套接字连接

    ip = ui->serverIp->text();
    messagePort=ui->serverMessagePort->text().toInt();
    filePort=ui->serverFilePort->text().toInt();


    messageServer->listen(QHostAddress(ip), messagePort);//创建服务器主机地址和端口号
    fileServer->listen(QHostAddress(ip), filePort);//创建服务器主机地址和端口号

    connect(fileServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));//设置连接后执行的函数

    ui->receivedStatusLabel->setText(tr("开始监听..."));//显示当前状态
}

void Widget::on_connectButton_clicked()
{
    if(sendTimes == 0)  //只有第一次发送的时候，是发生在连接产生信号connect时
        {
            ip = ui->clientIp->text();
            messagePort=ui->clientMessagePort->text().toInt();
            filePort=ui->clientFilePort->text().toInt();

            tcpMessageClient->connectToHost(QHostAddress(ip),messagePort);//连接目标主机
            tcpFileClient->connectToHost(QHostAddress(ip),filePort);//连接目标主机
            sendTimes = 1;
        }
}

void Widget::on_sendFileButton_clicked()
{
    ui->sendStatusLabel->setText(tr("正在打开文件..."));

    loadSize = 0;//待传输文件信息初始化
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    fileName = QFileDialog::getOpenFileName(this);//获取打开的文件名
    localFile = new QFile(fileName);//创建新文件用于传输
    localFile->open(QFile::ReadOnly);//以只读方式打开文件

    ui->sendStatusLabel->setText(tr("已打开文件 %1").arg(fileName));//显示“已打开文件”
    send();  //第一次发送的时候是由connectToHost出发connect信号才能调用send，第二次之后就需要调用send了

    ui->sendStatusLabel->setText(tr("正在发送文件 %1").arg(fileName));//显示当前状态
}
