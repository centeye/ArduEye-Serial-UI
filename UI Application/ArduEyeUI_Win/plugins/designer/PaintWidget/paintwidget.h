#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>

#define PAINT_HORZ  1
#define PAINT_VERT  2

class QDESIGNER_WIDGET_EXPORT PaintWidget : public QWidget
{
    Q_OBJECT

public:
    PaintWidget(QWidget *parent = 0);
    ~PaintWidget();

    char * data;
    int rows, cols;
    int PaintType;

    void LoadData(char *Data, int Rows, int Cols, int Type);

protected:
    void paintEvent(QPaintEvent *);
    void PaintBoxHorizontal();
    void PaintBoxVertical();
};

#endif // PAINTWIDGET_H
