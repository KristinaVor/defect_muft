#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>


#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;


#include "service_functions.h"
#include "myimageviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    myFrame* p_frame = nullptr;

    myTIFF *p_tiff = nullptr;

private slots:

    void on_btn_loadTiff_clicked();


    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;



};

#endif // MAINWINDOW_H
