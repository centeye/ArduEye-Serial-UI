
#include <QtGui>

#include "paintwidget.h"

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget(parent)
{

    rows = 0; cols = 0;
    data = 0; PaintType = 0;
}

PaintWidget::~PaintWidget()
{

}

void PaintWidget::LoadData(char *Data, int Rows, int Cols, int Type)
{
    data = Data;
    rows = Rows;
    cols = Cols;
    PaintType = Type;
}

void PaintWidget::paintEvent(QPaintEvent *)
{

    switch(PaintType){
    case PAINT_HORZ:
        PaintBoxHorizontal();
        break;
    case PAINT_VERT:
        PaintBoxVertical();
        break;
    default:
        break;

    }
}

void PaintWidget::PaintBoxHorizontal()
{
    int i, j, iIdx, X1;
    int * BoxCols = new int[cols];
    QPainter painter(this);

    if(data != 0)
    {

        // if more than one column, sum data to be 1D
        for(i = 0; i < cols; i++)
            BoxCols[i] = 0;
        for(i = 0; i < rows; i++)
        {
            iIdx = i*cols;
            for(j = 0; j < cols; j++)
            {
                BoxCols[j] += data[iIdx + j];
            }
        }
        for(j = 0; j < cols; j++)
        {
            BoxCols[j] /= 4;
            BoxCols[j] *= this->height() / 128;
        }

        // paint boxes
        int BoxWidth = this->width() / cols;


        X1 = 0;
        for(j = 0; j < cols; j++)
        {
            if(BoxCols[j] >=0)
                painter.setPen("blue");
            else
                painter.setPen("red");
            painter.drawRect(X1, 0, BoxWidth, BoxCols[j]);
            // change brush
            painter.setPen("black");
            painter.drawRect(X1, BoxCols[j], BoxWidth, this->height() - BoxCols[j]);
            X1 += BoxWidth;
        }
    }
    else
    {
        painter.setPen("black");
        painter.drawRect(0, 0, this->width(), this->height());
    }
    delete BoxCols;
}

void PaintWidget::PaintBoxVertical()
{
    int i, j, Y1, iIdx;
    int * BoxRows = new int[rows];
    QPainter painter(this);

    if(data != 0)
    {
        // if more than one column, sum data to be 1D
        for(i = 0; i < cols; i++)
            BoxRows[i] = 0;
        for(i = 0; i < rows; i++)
        {
            iIdx = i * cols;
            for(j = 0; i < cols; j++)
            {
                BoxRows[i] += data[iIdx + j];
            }
            BoxRows[i] /= 4;
            BoxRows[i] *= this->width() / 128;
        }

        // initiallize block size
        int BoxHeight = this->height() / rows;

        // paint boxes
        Y1 = 0;
        for(j = 0; j < rows; j++)
        {
            if(BoxRows[j] >=0)
                painter.setPen("blue");
            else
                painter.setPen("red");
            painter.drawRect(0, Y1, BoxRows[j], BoxHeight);
            // change brush
            painter.setPen("black");
            painter.drawRect(BoxRows[j], Y1, this->width() - BoxRows[j], BoxHeight);
            Y1 += BoxHeight;
        }
    }
    else
    {
        painter.setPen("black");
        painter.drawRect(0, 0, this->width(), this->height());
    }
    delete BoxRows;
}
