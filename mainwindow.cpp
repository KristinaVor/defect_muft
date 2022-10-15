#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    p_tiff = new myTIFF("MainWindow");

    p_frame = new myFrame(ui->frm_singleView->width(), ui->frm_singleView->height());
    ui->frm_singleView->setLayout(new QHBoxLayout);
    ui->frm_singleView->layout()->addWidget(p_frame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_loadTiff_clicked()
{
    uint imageWidth;
    uint imageHeight;
    ushort * imageData, *tmp;
    QByteArray ba;
    QString fp = QFileDialog::getOpenFileName();
    const char *c_str;

qDebug() << "MainWindow::1";
    ba = fp.toLocal8Bit();
    qDebug() << "MainWindow::2";
    c_str = ba.data();
    qDebug() << "MainWindow::3";
    tmp = p_tiff->openFile(c_str, &imageWidth, &imageHeight);
    imageData = new ushort[imageWidth * imageHeight];
    memcpy(imageData, tmp, imageWidth * imageHeight * 2);
    p_frame->setRAWImage(imageData,imageWidth,imageHeight,"");

}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
 if (!arg1)
 {
 p_frame->showDefect(false);
  }
 else
 {
     p_frame->showDefect(true);
 }
}
