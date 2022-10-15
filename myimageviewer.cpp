#include "myimageviewer.h"
#include <QApplication>
myImageViewer::myImageViewer(QWidget *parent) :
    QLabel(parent)
{
    mode = 0;

    curScale = 1.0;
}

void myImageViewer::wheelEvent( QWheelEvent * event )
{
    emit wheel(event);
}

void myImageViewer::mousePressEvent( QMouseEvent * event )
{
        emit mousePress(event);
}

void myImageViewer::mouseReleaseEvent( QMouseEvent * event )
{
        emit mouseRelease(event);
}

void myImageViewer::mouseMoveEvent( QMouseEvent * event )
{
        emit mouseMove(event);
}

void myImageViewer::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
}

QPixmap myImageViewer::GetPixmap()
{
    QPixmap pix = *this->pixmap();
    return pix;
}

myFrame::myFrame(int width, int height, QWidget *parent) :
    QFrame(parent)
{
    // Создаем новую метку для вывода изображения
    imageLabel  = new myImageViewer(this);
    imageLabel->setScaledContents(true);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setMouseTracking(1);

    ImageW = width;
    ImageH = height;

    topMoved = 0;
    bottomMoved = 0;
    previewMoved = 0;

    ROImoved = 0;
    ROIresized = 0;

    // Создаем слой для размещения расширяемой области в данном фрейме
    layout = new QGridLayout;
    layout->setMargin(0);
    this->setLayout(layout);
    globalScale = 1;

    // Создаем расширяемую область
    scrollArea = new QScrollArea;
    scrollArea->setStyleSheet("border:solid grey;"); // цвет рамки области отображения
    scrollArea->setAlignment(Qt::AlignCenter);
    // Добавляем область на новый слой
    layout->addWidget(scrollArea);

    // Добавляем метку для вывода изображения в расширяемую область
    scrollArea->setWidget(imageLabel);

    imageLabel->setVisible(false);

    // Подготовка переменной изображения, выводимого на экран
    QImage image(ImageH, ImageW, QImage::Format_Indexed8);
    rxImage = image;
    QTransform rotating;
    rotating.rotate(90);
    rxImage = rxImage.transformed(rotating);

//    inverted = 1;
//    scurveCorr = 0;
//    gammaCoeff = 1;

//    sharp = 0;
//    blur = 0;
//    filter1 = 0;
//    filter2 = 0;
//    advSharp = 0;


    currentLeft = 1;
    currentRight = 65534;

    // Обработка сигналов для событий мыши на изображении
    connect(imageLabel, SIGNAL(wheel(QWheelEvent *)), this, SLOT(labelWheel(QWheelEvent *)));
    connect(imageLabel, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(labelMousePress(QMouseEvent*)));
    connect(imageLabel, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(labelMouseRelease(QMouseEvent*)));
    connect(imageLabel, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(labelMouseMove(QMouseEvent*)));
    connect(scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(VerticalBarChange(int)));
    connect(scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(HorizontalBarChange(int)));

    // Обработка сигнала изменения размеров окна
    connect(this, SIGNAL(resize(QResizeEvent *)), this, SLOT(scrollAreaResize(QResizeEvent*)));
    imageData = new ushort[ImageH*ImageW];
    tmpData = new ushort[ImageW*ImageH];
    reservData = new ushort[ImageW*ImageH];


    this->setVisible(false);
    this->setFrameShape(QFrame::NoFrame);
    this->setFrameShadow(QFrame::Plain);

    scrollArea->verticalScrollBar()->setVisible(0);
    scrollArea->horizontalScrollBar()->setVisible(0);
    scrollArea->verticalScrollBar()->setEnabled(0);
    scrollArea->horizontalScrollBar()->setEnabled(0);

}

void myFrame::resizeEvent( QResizeEvent * event )
{
    emit resize(event);
}

void myFrame::labelWheel(QWheelEvent * event)
{
    // Указатели на полосы прокрутки
    QScrollBar * hBar, * vBar;
    hBar = scrollArea->horizontalScrollBar();
    vBar = scrollArea->verticalScrollBar();

    // Величина и направление прокрутки колесом мыши
    int delta = event->delta();

    // Координаты события прокрутки
    imageX = event->x();
    imageY = event->y();

    // Значение полос прокрутки
    int barX = hBar->value();
    int barY = vBar->value();

    float scale;
    int maxFactor;

    if (delta > 0)
    {
        // Увеличение масштаба
        scale = (float)1.2;
        maxFactor = 9;

        if (globalScale > maxFactor)
            scale = 1;
    }
    else
    {
        // Уменьшение масштаба
        scale = (float)(1.0 / 1.2);
        if (globalScale*scale < 1.0)
             scale = 1;
    }

    globalScale*=scale;
    imageLabel->curScale = globalScale*baseScale;

    // Изменение размеров метки вывода изображения
    imageLabel->resize(imageLabel->pixmap()->size()*globalScale*baseScale);

    //qDebug() << hBar->maximum() << hBar->minimum() << hBar->value() << hBar->pageStep();

    hBar->setValue(imageX*(globalScale/oldScale) - (imageX - barX));
    vBar->setValue(imageY*(globalScale/oldScale) - (imageY - barY));

    oldScale = globalScale;

    return;
}


void myFrame::labelMousePress(QMouseEvent * event)
{
    // Проверяем нажатую кнопку мыши
    if (event->button() & Qt::MiddleButton)
    {
        // Запоминаем координаты нажатия мыши
        imageX = event->globalX();
        imageY = event->globalY();
        // Изменияем иконку курсора
        setCursor(Qt::OpenHandCursor);
    }
}

void myFrame::labelMouseRelease(QMouseEvent * event)
{
    // Проверяем нажатую кнопку мыши
    if (event->button() & Qt::MiddleButton)
    {
        // Изменияем иконку курсора
        setCursor(Qt::CustomCursor);
    }
}

void myFrame::labelMouseMove(QMouseEvent * event)
{
    float scale = globalScale*baseScale;

    // Проверяем нажатую кнопку мыши
    if (event->buttons().testFlag(Qt::MiddleButton))
    {
        // Указатели на полосы прокрутки
        QScrollBar * hBar, * vBar;
        hBar = scrollArea->horizontalScrollBar();
        vBar = scrollArea->verticalScrollBar();

        // Изменение положения полос прокрутки на изменение координаты мыши
        hBar->setValue(hBar->value()-(event->globalX()-imageX));
        vBar->setValue(vBar->value()-(event->globalY()-imageY));
        // Запоминем новое положение мыши
        imageX = event->globalX();
        imageY = event->globalY();
    }
//    oldX = newX;
//    oldY = newY;
}

void myFrame::scrollAreaResize(QResizeEvent * event)
{
    float scaleW = ((float)scrollArea->size().width())/((float)rxImage.size().width());
    float scaleH = ((float)scrollArea->size().height())/((float)rxImage.size().height());

    if (scaleW > scaleH)
        baseScale = scaleH;
    else
        baseScale = scaleW;

    imageLabel->resize(rxImage.size()*baseScale*globalScale);

    imageLabel->curScale = baseScale*globalScale;
}


void myFrame::setRAWImage(ushort * RAWData, int w, int h, QString text)
{
    imageLabel->text = text;

    ImageW = w;
    ImageH = h;

    maxHeight = ImageH;
    currentHeight = 0;

    QImage image(ImageW, maxHeight, QImage::Format_Indexed8);
    rxImage = image;

    delete[] imageData;
    delete[] reservData;
    delete[] tmpData;

    imageData = new ushort[ImageH*ImageW];
    reservData = new ushort[ImageW*ImageH];
    tmpData = new ushort[ImageW*ImageH];

    memcpy(imageData, RAWData, ImageH*ImageW*2);
    memcpy(reservData, RAWData, ImageH*ImageW*2);

    float scaleH = ((float)scrollArea->size().width())/((float)rxImage.size().width()+10);
    float scaleW = ((float)scrollArea->size().height())/((float)rxImage.size().height()+10);

    if (scaleW > scaleH)
        baseScale = scaleH;
    else
        baseScale = scaleW;

    RAWto8bit();
    imageLabel->curScale = globalScale*baseScale; // сброс маркеров при переходе на другое изображение
}

void myFrame::RAWto8bit()
{
    this->setVisible(true);

    QImage image(ImageW, maxHeight, QImage::Format_Indexed8);
    rxImage = image;

    quint8 * line = rxImage.scanLine(currentHeight);
    int stride =  rxImage.bytesPerLine();

    qint32 pixel = 0;
    for ( int y = currentHeight; y < currentHeight+ImageH; ++y, line += stride)
    {
        quint8 * pix = line;
        for ( int x = 0; x < ImageW; ++x, pix += 1)
        {
            pixel  = imageData[((y-currentHeight)*ImageW)+x]/255;

            if (pixel > 255)
            {
                pixel = 255;
            }
            if (pixel < 0)
            {
                pixel = 0;
            }

            pix[0] = pixel;
        }
    }

    correctedImage = rxImage;
    img = QImage2Mat(correctedImage);
    if (img.empty())
    {
        qDebug() << "Image Loading error";
        return;
    }
    qDebug() << img.rows << img.cols;

    QPixmap pixmap;



//     /////////////// Canny детектор границ  /////////////////////////////////
//                       Canny(img,imageCanny,14,10);
//        ///////////     Расширение границ, которые нашел Canny   /////////////////////////////////
//                        Mat kernel = getStructuringElement(MORPH_RECT,Size(5,5));
//                        dilate(imageCanny, imgDil,kernel);
//                        getContours(imgDil, img);


                        if (isNeedToShowDefect) pixmap.convertFromImage(Mat2QImage(img));
                        else pixmap.convertFromImage(rxImage);
                      //  pixmap.convertFromImage(rxImage);
                        imageLabel->setVisible(true);
                        imageLabel->curScale = globalScale*baseScale;
                        imageLabel->resize(rxImage.size()*baseScale*globalScale); // TODO: Придумать альтернативу resize(), т.к. медленно работает на больших увеличениях
        imageLabel->setPixmap(pixmap);
}

void myFrame::showDefect(bool state)
{
  isNeedToShowDefect = state;
  repaint();
}

QImage myFrame::Mat2QImage(Mat const& src)
{
     Mat temp; // make the same cv::Mat
     if(src.channels() != 1) cvtColor(src, temp,COLOR_BGR2RGB,0); // cvtColor Makes a copt, that what i need
     else cvtColor(src, temp,COLOR_GRAY2RGB,0);
     QImage dest= QImage((const uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888).rgbSwapped();
     dest.bits();
     return dest;
}

Mat myFrame::QImage2Mat(QImage const& src)
{
    Mat mat (src.height(), src.width(),
    CV_8UC1,
    const_cast<uchar*>(src.bits()),
    static_cast<size_t>(src.bytesPerLine()));
    return mat;
}

void myFrame::getContours(Mat imgDil, Mat img)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

   findContours(imgDil, contours, hierarchy,RETR_LIST, CHAIN_APPROX_NONE);
 //  drawContours(img,contours, -1, Scalar(255,0,255),5); // -1 все контура, фиолетовый цвет, толщина
 //  imshow("None approximation", img);

///////////// Фильтр по контуру фигур (круг)////////////////

    for (int i=0; i < contours.size(); i++)
    {

        double area = contourArea(contours[i]);//возвращает площадь контура

        qDebug() << " Area: " <<  area  << endl;

         double perim = arcLength(contours[i],true);//возвращает периметр

        qDebug() <<  " Perim: " << perim << endl;

        //Фильтрация по размеру области
        if (area / (perim * perim) > 0.068 && area / (perim * perim)< 0.09 )
        {
            drawContours(img,contours, i, Scalar(255,0,255),7);

////////////////////////////////////
           char text[] ="" ;
            Rect brect = boundingRect(contours[i]);
            sprintf(text,"S = %d", brect.area());
                string str(text);
                putText(img,str, Point(brect.x+20, brect.y+20), 1, 1, CV_RGB(0,255,0));

        }
 }
}
void myFrame::repaint()
{
    memcpy(imageData, reservData, ImageH*ImageW*2);
    RAWto8bit();
}

void myFrame::saveImage(QString fileName)
{
    QString str = fileName.right(3);
    uchar state;
    if(str == "jpg") state = 0;
    if(str == "bmp") state = 1;

    QPixmap pix = imageLabel->grab();
    QImage im = pix.toImage();
    switch (state)
    {
        case 0:
         //   qDebug() << "myFrame::jpg" << fileName ;
            im = im.convertToFormat(QImage::Format_RGB32);
            im.save(fileName,"JPG");
            break;
        case 1:
         //   qDebug() << "myFrame::bmp" << fileName;
            im = im.convertToFormat(QImage::Format_RGB32);
            im.save(fileName,"BMP");
            break;
        default:
            if (debugType >= 2) qDebug() << "myFrame::saveError";
            break;
    }
}

QImage myFrame::returnImage(int w, int h)
{
    return rxImage.convertToFormat(QImage::Format_RGB32).scaled(w,h);
}

void myFrame::VerticalBarChange(int val)
{
    if(!scrollArea->horizontalScrollBar()->isVisible())  emit HorizontalBar(2.0, 190.0 - 4.0); // становятся невидимы оба
    emit VerticalBar((190.0*((float)(val+0.001)/(float)imageLabel->size().height()))+ 2.0, 190.0 * (float)scrollArea->height()  / (float)imageLabel->size().height() - 4.0);
}

void myFrame::HorizontalBarChange(int val)
{
    if(!scrollArea->verticalScrollBar()->isVisible())  emit VerticalBar(2.0, 190.0 - 2.0); // становятся невидимы оба
    emit HorizontalBar((190.0*(((float)val+0.001)/(float)imageLabel->size().width()))+ 2.0, 190.0 * (float)scrollArea->width() / ((float)imageLabel->size().width()) - 4.0); // 890 - размер области frame в mainwindow сделать setter
}

void myFrame::setMode(int mode)
{
    imageLabel->mode = mode;
}

int myFrame::getMode()
{
    return imageLabel->mode;
}

void myFrame::updateState()
{
    imageLabel->curScale = globalScale*baseScale;
    repaint();
}
