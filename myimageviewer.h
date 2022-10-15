#ifndef MYIMAGEVIEWER_H
#define MYIMAGEVIEWER_H

#include <QLabel>
#include <QCheckBox>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QFrame>
#include <QResizeEvent>
#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QDebug>
#include <QTime>
#include <QPainter>
#include <QPaintEngine>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMenu>

#define BITS 65535

#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;


class myImageViewer : public QLabel
{
    Q_OBJECT
public:
    // methods
    explicit myImageViewer(QWidget *parent = 0);
    void wheelEvent( QWheelEvent * event );
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void mouseMoveEvent( QMouseEvent * event );
    void paintEvent(QPaintEvent* event);

    // ROI
    bool enableROI;
    int mode;
    int recX, recY, recW, recH;
    bool reconBinning = 0;

    // scale
    float curScale;

    // text for image
    QString text;

    int currentSlice = 0;

    bool isStick = 0;
    int stickState = 0;
    QPoint firstStickPoint, secondStickPoint;
    float magnificationFactor = 1.0;

    ushort * currentMask = NULL;
    bool isMask = 0;
    int currentWidth;
    int currentHeight;
    int trLeft;
    int trTop;
    int trRight;
    int trBottom;

    int newWidth;
    int newHeight;

    ushort * tmpMask = NULL;

    bool isPixel = false;
    ushort isPixelValue = 0;
    int currentMousePosX, currentMousePosY;



    int debugType = 0;

signals:
    void wheel(QWheelEvent *);
    void mousePress(QMouseEvent *);
    void mouseRelease(QMouseEvent *);
    void mouseMove(QMouseEvent *);


public slots:
    QPixmap GetPixmap();

};

class myFrame : public QFrame
{
    Q_OBJECT
public:
    explicit myFrame(int width, int height, QWidget *parent = 0);

    // function
    void resizeEvent( QResizeEvent * event );

    void RAWto8bit();

    QImage Mat2QImage(Mat const& src);
    Mat QImage2Mat(QImage const& src);
    Mat img,imgray,imgBlur,imageCanny,imgDil;
    void getContours(Mat imgDil, Mat img);


    void showDefect(bool state);
    bool isNeedToShowDefect = false;




    void repaint();
    void saveImage(QString fileName);
    void setMode(int mode);
    int getMode();
    void updateState();

    void setMagnificationFactor(float factor);



    int debugType = 0;


    // UI variables
    QGridLayout * layout;
    QScrollArea * scrollArea;
    myImageViewer * imageLabel;

    // graphic lines status (for mouse event)
    bool topMoved;
    bool bottomMoved;
    bool previewMoved;
    bool ROImoved;
    bool ROIresized;
    bool reconBinning = 0;
    int imageX, imageY;
    int deltaY, deltaX;
    int oldX = 0, oldY = 0;

    // image variables
    QImage rxImage;
    QImage correctedImage;



    ushort * imageData;
    ushort * reservData;
    ushort * tmpData;
    int ImageW, ImageH;


    // image scale variables
    float baseScale, globalScale;
    float oldScale;

    // histogram variables
    ushort currentLeft, currentRight;
    int ImageFieldW, ImageFieldH;
    int currentState;

    //
    bool releaseAlt = false;

    bool isVideo = false;


    // for helix projections view
    bool isHelixView = false;
    float sizeOfVerticalStepOnReceiver = 0;
    int numberOfHelixStep = 0;
    int maxHeight = 0;
    int currentHeight = 0;




signals:
    void resize(QResizeEvent *);
    void histCalculated(ushort *, ushort, ushort);
    void finished();
    void HorizontalBar(float, float);
    void VerticalBar(float, float);
    void changeSliders(int, int);
    void listItems(int);

    void changeHistWidget(int, int);
    void changeReconBorder(int start, int end, int preview);

    void getRAWImageValue(int * pixel, int i, int j);

public slots:

    void labelWheel(QWheelEvent * event);
    void labelMousePress(QMouseEvent * event);
    void labelMouseRelease(QMouseEvent * event);
    void labelMouseMove(QMouseEvent * event);
    void scrollAreaResize(QResizeEvent * event);
    void setRAWImage(ushort * RAWData, int w, int h, QString text);

    QImage returnImage(int w, int h);



private slots:
    void VerticalBarChange(int val);
    void HorizontalBarChange(int val);
};

#endif // MYIMAGEVIEWER_H
