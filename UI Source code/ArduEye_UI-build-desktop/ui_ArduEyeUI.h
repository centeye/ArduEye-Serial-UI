/********************************************************************************
** Form generated from reading UI file 'ArduEyeUI.ui'
**
** Created: Thu Sep 15 07:56:52 2011
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
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *CmdEdit;
    QPushButton *enterButton;
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
        ArduEyeUI->resize(1119, 437);
        QPalette palette;
        QBrush brush(QColor(145, 151, 232, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(124, 124, 128, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush1);
        QBrush brush2(QColor(252, 252, 252, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        QBrush brush3(QColor(40, 40, 40, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        ArduEyeUI->setPalette(palette);
        ArduEyeUI->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FF6C52,  stop: 1 #FF120B);\n"
"border-radius: 3px;\n"
"min-height: 23px;\n"
"border: none;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 7px;\n"
"padding-right: 7px;\n"
"margin: 5px;\n"
"}\n"
"QLineEdit{\n"
"background-color: rgb(200, 200, 200);\n"
"border-color: rgb(200, 200, 200);\n"
"border-style: solid;\n"
"}\n"
"QTextEdit{\n"
"background-color: rgb(200, 200, 200);\n"
"border-color: rgb(200, 200, 200);\n"
"border-style: solid;\n"
"}\n"
""));
        gridLayout_2 = new QGridLayout(ArduEyeUI);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        CmdEdit = new QLineEdit(ArduEyeUI);
        CmdEdit->setObjectName(QString::fromUtf8("CmdEdit"));
        CmdEdit->setMinimumSize(QSize(0, 21));

        horizontalLayout_2->addWidget(CmdEdit);

        enterButton = new QPushButton(ArduEyeUI);
        enterButton->setObjectName(QString::fromUtf8("enterButton"));
        enterButton->setMinimumSize(QSize(71, 39));

        horizontalLayout_2->addWidget(enterButton);


        verticalLayout->addLayout(horizontalLayout_2);

        textEdit = new QTextEdit(ArduEyeUI);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy);
        textEdit->setMinimumSize(QSize(381, 192));
        textEdit->setMaximumSize(QSize(1200, 1200));
        textEdit->setFrameShape(QFrame::StyledPanel);

        verticalLayout->addWidget(textEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        PortSelect = new QComboBox(ArduEyeUI);
        PortSelect->setObjectName(QString::fromUtf8("PortSelect"));
        PortSelect->setMinimumSize(QSize(261, 26));

        horizontalLayout->addWidget(PortSelect);

        ConnectButton = new QPushButton(ArduEyeUI);
        ConnectButton->setObjectName(QString::fromUtf8("ConnectButton"));
        ConnectButton->setMinimumSize(QSize(101, 39));
        ConnectButton->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout->addWidget(ConnectButton);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        CmdListEdit = new QTextEdit(ArduEyeUI);
        CmdListEdit->setObjectName(QString::fromUtf8("CmdListEdit"));
        sizePolicy.setHeightForWidth(CmdListEdit->sizePolicy().hasHeightForWidth());
        CmdListEdit->setSizePolicy(sizePolicy);
        CmdListEdit->setMinimumSize(QSize(161, 241));
        CmdListEdit->setMaximumSize(QSize(160, 16777215));
        QPalette palette1;
        QBrush brush4(QColor(200, 200, 200, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush4);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush4);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush4);
        CmdListEdit->setPalette(palette1);

        verticalLayout_2->addWidget(CmdListEdit);

        FileRecordButton = new QPushButton(ArduEyeUI);
        FileRecordButton->setObjectName(QString::fromUtf8("FileRecordButton"));
        FileRecordButton->setMinimumSize(QSize(161, 39));
        FileRecordButton->setMaximumSize(QSize(16777215, 16777215));

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
        imagelabel->setMinimumSize(QSize(391, 331));
        imagelabel->setMaximumSize(QSize(1200, 1200));
        imagelabel->setFrameShape(QFrame::Box);
        imagelabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(imagelabel, 0, 0, 1, 1);

        OpticY_Label = new QLabel(ArduEyeUI);
        OpticY_Label->setObjectName(QString::fromUtf8("OpticY_Label"));
        OpticY_Label->setMinimumSize(QSize(65, 331));
        OpticY_Label->setMaximumSize(QSize(65, 1200));
        OpticY_Label->setFrameShape(QFrame::Box);

        gridLayout->addWidget(OpticY_Label, 0, 1, 1, 1);

        OpticX_Label = new QLabel(ArduEyeUI);
        OpticX_Label->setObjectName(QString::fromUtf8("OpticX_Label"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(OpticX_Label->sizePolicy().hasHeightForWidth());
        OpticX_Label->setSizePolicy(sizePolicy2);
        OpticX_Label->setMinimumSize(QSize(391, 65));
        OpticX_Label->setMaximumSize(QSize(1200, 65));
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
        enterButton->setText(QApplication::translate("ArduEyeUI", "Enter", 0, QApplication::UnicodeUTF8));
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
