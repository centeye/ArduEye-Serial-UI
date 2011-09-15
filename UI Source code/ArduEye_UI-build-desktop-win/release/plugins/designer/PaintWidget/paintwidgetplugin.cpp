
#include "paintwidget.h"
#include "paintwidgetplugin.h"

#include <QtPlugin>

PaintWidgetPlugin::PaintWidgetPlugin(QObject *parent) :
    QObject(parent)
{
    initialized = false;
}

void PaintWidgetPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if(initialized)
        return;

    initialized = true;
}

bool PaintWidgetPlugin::isInitialized() const
{
    return initialized;
}

QWidget * PaintWidgetPlugin::createWidget(QWidget *parent)
{
    return new PaintWidget(parent);
}

QString PaintWidgetPlugin::name() const
{
    return "PaintWidget";
}

QString PaintWidgetPlugin::group() const
{
    return "Display Widgets";
}

QIcon PaintWidgetPlugin::icon() const
{
    return QIcon();
}

QString PaintWidgetPlugin::toolTip() const
{
    return "";
}

QString PaintWidgetPlugin::whatsThis() const
{
    return "widget handles custom paint functions";
}

bool PaintWidgetPlugin::isContainer() const
{
    return false;
}

QString PaintWidgetPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
               " <widget class=\"PaintWidget\" name=\"PaintWidget\">\n"
               "  <property name=\"toolTip\" >\n"
               "   <string>Paint Widget</string>\n"
               "  </property>\n"
               "  <property name=\"whatsThis\" >\n"
               "   <string>widget handles custom paint functions</string>\n"
               "  </property>\n"
               " </widget>\n"
               "</ui>\n";
}

QString PaintWidgetPlugin::includeFile() const
{
    return "paintwidget.h";
}

Q_EXPORT_PLUGIN2(PaintWidget, PaintWidgetPlugin)






