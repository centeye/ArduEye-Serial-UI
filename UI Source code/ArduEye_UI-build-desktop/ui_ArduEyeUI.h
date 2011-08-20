/********************************************************************************
** Form generated from reading UI file 'ArduEyeUI.ui'
**
** Created: Sat Aug 20 15:59:35 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARDUEYEUI_H
#define UI_ARDUEYEUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ArduEyeUI
{
public:
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *CmdEdit;
    QPushButton *findButton;
    QTextEdit *textEdit;
    QHBoxLayout *horizontalLayout;
    QComboBox *PortSelect;
    QPushButton *ConnectButton;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *CmdListEdit;
    QPushButton *FileRecordButton;
    QGridLayout *gridLayout;
    QLabel *imagelabel;
    QLabel *OpticY_Label;
    QLabel *OpticX_Label;
    QLabel *label_place_holder;

    void setupUi(QWidget *ArduEyeUI)
    {
        if (ArduEyeUI->objectName().isEmpty())
            ArduEyeUI->setObjectName(QString::fromUtf8("ArduEyeUI"));
        ArduEyeUI->resize(969, 364);
        gridLayout_2 = new QGridLayout(ArduEyeUI);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        CmdEdit = new QLineEdit(ArduEyeUI);
        CmdEdit->setObjectName(QString::fromUtf8("CmdEdit"));

        horizontalLayout_2->addWidget(CmdEdit);

        findButton = new QPushButton(ArduEyeUI);
        findButton->setObjectName(QString::fromUtf8("findButton"));

        horizontalLayout_2->addWidget(findButton);


        verticalLayout_4->addLayout(horizontalLayout_2);

        textEdit = new QTextEdit(ArduEyeUI);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setMinimumSize(QSize(256, 0));
        textEdit->setFrameShape(QFrame::StyledPanel);

        verticalLayout_4->addWidget(textEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        PortSelect = new QComboBox(ArduEyeUI);
        PortSelect->setObjectName(QString::fromUtf8("PortSelect"));
        PortSelect->setMinimumSize(QSize(201, 0));

        horizontalLayout->addWidget(PortSelect);

        ConnectButton = new QPushButton(ArduEyeUI);
        ConnectButton->setObjectName(QString::fromUtf8("ConnectButton"));

        horizontalLayout->addWidget(ConnectButton);


        verticalLayout_4->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        CmdListEdit = new QTextEdit(ArduEyeUI);
        CmdListEdit->setObjectName(QString::fromUtf8("CmdListEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CmdListEdit->sizePolicy().hasHeightForWidth());
        CmdListEdit->setSizePolicy(sizePolicy);
        CmdListEdit->setMinimumSize(QSize(161, 192));
        CmdListEdit->setMaximumSize(QSize(160, 16777215));
        QPalette palette;
        QBrush brush(QColor(227, 227, 227, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(212, 208, 200, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        CmdListEdit->setPalette(palette);

        verticalLayout_2->addWidget(CmdListEdit);

        FileRecordButton = new QPushButton(ArduEyeUI);
        FileRecordButton->setObjectName(QString::fromUtf8("FileRecordButton"));

        verticalLayout_2->addWidget(FileRecordButton);


        horizontalLayout_3->addLayout(verticalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        imagelabel = new QLabel(ArduEyeUI);
        imagelabel->setObjectName(QString::fromUtf8("imagelabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(imagelabel->sizePolicy().hasHeightForWidth());
        imagelabel->setSizePolicy(sizePolicy1);
        imagelabel->setMinimumSize(QSize(321, 271));
        imagelabel->setMaximumSize(QSize(1200, 1200));
        imagelabel->setFrameShape(QFrame::Box);
        imagelabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(imagelabel, 0, 0, 1, 1);

        OpticY_Label = new QLabel(ArduEyeUI);
        OpticY_Label->setObjectName(QString::fromUtf8("OpticY_Label"));
        OpticY_Label->setMinimumSize(QSize(51, 271));
        OpticY_Label->setMaximumSize(QSize(1200, 1200));
        OpticY_Label->setFrameShape(QFrame::Box);

        gridLayout->addWidget(OpticY_Label, 0, 1, 1, 1);

        OpticX_Label = new QLabel(ArduEyeUI);
        OpticX_Label->setObjectName(QString::fromUtf8("OpticX_Label"));
        OpticX_Label->setMinimumSize(QSize(321, 51));
        OpticX_Label->setMaximumSize(QSize(1200, 1200));
        OpticX_Label->setFrameShape(QFrame::Box);

        gridLayout->addWidget(OpticX_Label, 1, 0, 1, 1);

        label_place_holder = new QLabel(ArduEyeUI);
        label_place_holder->setObjectName(QString::fromUtf8("label_place_holder"));
        label_place_holder->setMinimumSize(QSize(51, 51));
        label_place_holder->setMaximumSize(QSize(100, 100));

        gridLayout->addWidget(label_place_holder, 1, 1, 1, 1);


        horizontalLayout_3->addLayout(gridLayout);


        gridLayout_2->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        retranslateUi(ArduEyeUI);

        QMetaObject::connectSlotsByName(ArduEyeUI);
    } // setupUi

    void retranslateUi(QWidget *ArduEyeUI)
    {
        ArduEyeUI->setWindowTitle(QApplication::translate("ArduEyeUI", "ArduEyeUI", 0, QApplication::UnicodeUTF8));
        findButton->setText(QApplication::translate("ArduEyeUI", "Enter", 0, QApplication::UnicodeUTF8));
        ConnectButton->setText(QApplication::translate("ArduEyeUI", "Connect", 0, QApplication::UnicodeUTF8));
        FileRecordButton->setText(QApplication::translate("ArduEyeUI", "Record To File", 0, QApplication::UnicodeUTF8));
        imagelabel->setText(QString());
        OpticY_Label->setText(QString());
        OpticX_Label->setText(QString());
        label_place_holder->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ArduEyeUI: public Ui_ArduEyeUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARDUEYEUI_H
