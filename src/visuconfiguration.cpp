#include "visuconfiguration.h"

#include <QVector>
#include <QPushButton>
#include <algorithm>

#include "visusignal.h"
#include "visuhelper.h"
#include "visuconfigloader.h"
#include "visumisc.h"
#include "exceptions/configloadexception.h"
#include "instruments/instanalog.h"
#include "instruments/instdigital.h"
#include "instruments/instlinear.h"
#include "instruments/insttimeplot.h"
#include "instruments/instxyplot.h"
#include "instruments/instled.h"
#include "controls/ctrlbutton.h"
#include "statics/staticimage.h"

const QString VisuConfiguration::TAG_INSTRUMENT = "instrument";
const QString VisuConfiguration::TAG_CONTROL = "control";
const QString VisuConfiguration::TAG_SIGNAL = "signal";
const QString VisuConfiguration::TAG_STATIC = "static";
const QString VisuConfiguration::TAG_CONFIGURATION = "configuration";
const QString VisuConfiguration::ATTR_TYPE = "type";

// These tags are not used, except for purpose of verification of XML structure.
// Perhaps some kind of load hooks can be later triggered when reading these tags.
const QString VisuConfiguration::TAG_VISU_CONFIG = "visu_config";
const QString VisuConfiguration::TAG_STATICS_PLACEHOLDER = "statics";
const QString VisuConfiguration::TAG_SIGNALS_PLACEHOLDER = "signals";
const QString VisuConfiguration::TAG_INSTRUMENTS_PLACEHOLDER = "instruments";
const QString VisuConfiguration::TAG_CONTROLS_PLACEHOLDER = "controls";

#include "wysiwyg/visuwidgetfactory.h"

VisuConfiguration::~VisuConfiguration()
{
    for (VisuWidget* widget : widgetsList)
    {
        delete widget;
    }
}

void VisuConfiguration::attachInstrumentToSignal(QPointer<VisuInstrument> instrument, int signalId)
{
    getSignal(signalId)->connectInstrument(instrument);
}

void VisuConfiguration::detachInstrumentFromSignal(QPointer<VisuInstrument> instrument, int signalId)
{
    getSignal(signalId)->disconnectInstrument(instrument);
}

void VisuConfiguration::attachInstrumentToSignal(QPointer<VisuInstrument> instrument)
{
    quint16 signalId = instrument->getSignalId();
    attachInstrumentToSignal(instrument, signalId);
}

void VisuConfiguration::detachInstrumentFromSignal(QPointer<VisuInstrument> instrument)
{
    quint16 signalId = instrument->getSignalId();
    detachInstrumentFromSignal(instrument, signalId);
}

void VisuConfiguration::createSignalFromToken(QXmlStreamReader& xmlReader)
{
    QMap<QString, QString> properties = VisuConfigLoader::parseToMap(xmlReader, TAG_SIGNAL);
    VisuSignal* signal = new VisuSignal(properties);
    signalsList.push_back(signal);
}

QPointer<VisuInstrument> VisuConfiguration::createInstrumentFromToken(QXmlStreamReader& xmlReader, QWidget *parent)
{
    QMap<QString, QString> properties = VisuConfigLoader::parseToMap(xmlReader, TAG_INSTRUMENT);

    VisuWidget* widget = VisuWidgetFactory::createInstrument(parent, properties[ATTR_TYPE], properties);
    VisuInstrument* instrument = static_cast<VisuInstrument*>(widget);

    attachInstrumentToSignal(instrument);
    addWidget(instrument);
    widget->show();

    return instrument;
}

CtrlButton* VisuConfiguration::createControlFromToken(QXmlStreamReader& xmlReader, QWidget *parent)
{
    QMap<QString, QString> properties = VisuConfigLoader::parseToMap(xmlReader, TAG_CONTROL);
    CtrlButton* control;

    if (properties[ATTR_TYPE] == CtrlButton::TAG_NAME) {
        control = new CtrlButton(parent, properties);
    }

    addWidget(control);

    return control;
}

StaticImage* VisuConfiguration::createStaticFromToken(QXmlStreamReader& xmlReader, QWidget *parent)
{
    QMap<QString, QString> properties = VisuConfigLoader::parseToMap(xmlReader, TAG_STATIC);
    StaticImage* image;

    if (properties[ATTR_TYPE] == StaticImage::TAG_NAME) {
        image = new StaticImage(parent, properties);
    }

    addWidget(image);

    return image;
}

void VisuConfiguration::initializeInstruments()
{
    for (int i=0; i<signalsList.size(); i++) {
        signalsList[i]->initializeInstruments();
    }
}

void VisuConfiguration::createConfigurationFromToken(QXmlStreamReader& xmlReader)
{
    setConfigValues(VisuConfigLoader::parseToMap(xmlReader, TAG_CONFIGURATION));
}

void VisuConfiguration::setConfigValues(const QMap<QString, QString>& properties)
{
    mProperties = properties;
    GET_PROPERTY(cPort, quint16, properties);
    GET_PROPERTY(cWidth, quint16, properties);
    GET_PROPERTY(cHeight, quint16, properties);
    GET_PROPERTY(cColorBackground, QColor, properties);
    GET_PROPERTY(cName, QString, properties);
}

void VisuConfiguration::fromXML(QWidget *parent, const QString& xmlString)
{
    QXmlStreamReader xmlReader(xmlString);

    while (xmlReader.tokenType() != QXmlStreamReader::EndDocument
           && xmlReader.tokenType() != QXmlStreamReader::Invalid) {

        if (xmlReader.tokenType() == QXmlStreamReader::StartElement) {

            ConfigLoadException::setContext("loading configuration");

            if (xmlReader.name() == TAG_SIGNAL) {
                createSignalFromToken(xmlReader);
            }
            else if (xmlReader.name() == TAG_INSTRUMENT) {
                createInstrumentFromToken(xmlReader, parent);
            }
            else if (xmlReader.name() == TAG_CONTROL) {
                createControlFromToken(xmlReader, parent);
            }
            else if (xmlReader.name() == TAG_STATIC) {
                createStaticFromToken(xmlReader, parent);
            }
            else if (xmlReader.name() == TAG_CONFIGURATION) {
                createConfigurationFromToken(xmlReader);
            }
            else if (xmlReader.name() == TAG_VISU_CONFIG) {
                // No actions needed.
            }
            else if (xmlReader.name() == TAG_INSTRUMENTS_PLACEHOLDER) {
                // No actions needed.
            }
            else if (xmlReader.name() == TAG_CONTROLS_PLACEHOLDER) {
                // No actions needed.
            }
            else if (xmlReader.name() == TAG_STATICS_PLACEHOLDER) {
                // No actions needed.
            }
            else if (xmlReader.name() == TAG_SIGNALS_PLACEHOLDER) {
                // No actions needed.
            }
            else
            {
                throw ConfigLoadException("Unknown XML node \"%1\"", xmlReader.name().toString());
            }
        }

        xmlReader.readNext();

    }

    initializeInstruments();
}

QPointer<VisuSignal> VisuConfiguration::getSignal(quint16 signalId)
{
    if(signalId >= signalsList.size())
    {
        // We shouldn't throw unhandled exception here, as that would
        // mean that signal source can crash application if wrong
        // signal id is sent.
        qDebug("Signal id %d too large!", signalId);
    }
    return signalsList[signalId];
}

QPointer<VisuInstrument> VisuConfiguration::getInstrument(quint16 instrument_id)
{
    return qobject_cast<VisuInstrument*>(widgetsList[instrument_id]);
}

QVector<QPointer<VisuSignal>>& VisuConfiguration::getSignals()
{
    return signalsList;
}

quint16 VisuConfiguration::getPort()
{
    return cPort;
}

quint16 VisuConfiguration::getWidth()
{
    return cWidth;
}

quint16 VisuConfiguration::getHeight()
{
    return cHeight;
}

QColor VisuConfiguration::getBackgroundColor()
{
    return cColorBackground;
}

QString VisuConfiguration::getName()
{
    return cName;
}

QVector<QPointer<VisuWidget>> VisuConfiguration::getWidgets()
{
    return widgetsList;
}

void VisuConfiguration::addWidget(QPointer<VisuWidget> widget)
{
    int id = getFreeId((QVector<QPointer<QObject>>&)widgetsList);
    widget->setId(id);
    widgetsList[id] = widget;
}

void VisuConfiguration::deleteWidget(QPointer<VisuWidget> widget)
{
    VisuInstrument* instrument;
    if ( (instrument = qobject_cast<VisuInstrument*>(widget)) != nullptr)
    {
        detachInstrumentFromSignal(instrument);
    }

    delete(widgetsList[widget->getId()]);
}

void VisuConfiguration::addSignal(QPointer<VisuSignal> signal)
{
    int signalId = getFreeId((QVector<QPointer<QObject>>&)signalsList);
    signal->setId(signalId);
    signalsList[signalId] = signal;
}

int VisuConfiguration::getFreeId(QVector<QPointer<QObject>> &list)
{
    int id = -1;
    int size = list.size();
    for (int i=0 ; i<size; ++i)
    {
        if (list[i] == nullptr)
        {
            id = i;
        }
    }

    if (id == -1)
    {
        id = size;
        list.resize(size + 1);
    }

    return id;
}

void VisuConfiguration::deleteSignal(QPointer<VisuSignal> signal)
{
    int signalId = signal->getId();
    deleteSignal(signalId);
}

void VisuConfiguration::deleteSignal(int signalId)
{
    // pointer will be cleared automaticaly by QPointer
    delete (signalsList[signalId]);
}

QMap<QString, QString>& VisuConfiguration::getProperties()
{
    return mProperties;
}

QString VisuConfiguration::toXML()
{
    QString xml;

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml += "<visu_config>\n";
    xml += "\t<configuration>\n";
    xml += VisuMisc::mapToString(mProperties, 2);
    xml += "\t</configuration>\n";

    xml += "\t<signals>\n";
    for (VisuSignal* signal : signalsList)
    {
        if (signal != nullptr)
        {
            xml += "\t\t<signal>\n";
            xml += VisuMisc::mapToString(signal->getProperties(), 3);
            xml += "\t\t</signal>\n";
        }
    }
    xml += "\t</signals>\n";

    xml += "\t<instruments>\n";
    for (VisuWidget* instrument : widgetsList)
    {
        instrument = qobject_cast<VisuInstrument*>(instrument);
        if (instrument != nullptr)
        {
            xml += "\t\t<instrument>\n";
            xml += VisuMisc::mapToString(instrument->getProperties(), 3);
            xml += "\t\t</instrument>\n";
        }
    }
    xml += "\t</instruments>\n";

    xml += "\t<controls>\n";
    for (VisuWidget* control : widgetsList)
    {
        control = qobject_cast<VisuControl*>(control);
        if (control != nullptr)
        {
            xml += "\t\t<control>\n";
            xml += VisuMisc::mapToString(control->getProperties(), 3);
            xml += "\t\t</control>\n";
        }
    }
    xml += "\t</controls>\n";

    xml += "\t<statics>\n";
    for (VisuWidget* image : widgetsList)
    {
        image = qobject_cast<StaticImage*>(image);
        if (image != nullptr)
        {
            xml += "\t\t<static>\n";
            xml += VisuMisc::mapToString(image->getProperties(), 3);
            xml += "\t\t</static>\n";
        }
    }
    xml += "\t</statics>\n";

    xml += "<visu_config>\n";

    return xml;
}
