#ifndef SIGNAL_H
#define SIGNAL_H

#include <QtGlobal>
#include <QVector>
#include <QString>
#include <QObject>

#include "visuinstrument.h"
#include "visudatagram.h"
#include "visupropertyloader.h"
#include "visupropertymeta.h"
#include "visuconfigloader.h"

class VisuInstrument;   // forward declare Instrument class
class VisuSignal : public QObject
{
    Q_OBJECT

private:

    // configuration properties
    quint16 cId;                             // Signal ID
    QString cName;                           // Signal name
    QString cUnit;                           // Signal unit
    double  cFactor;                         // Signal factor used in real value calculation
    double  cOffset;                         // Signal offset used in real value calculation
    double  cMax;                            // Maximum signal value
    double  cMin;                            // Minimum signal value
    int     cSerialPlaceholder;              // Index of recevied serial string
    bool    cSerialTransform;                // Apply factor and offset to serial data

    quint64 mTimestamp;                      // Last update timestamp
    quint64 mRawValue;                       // Last value
    QMap<QString, QString> mProperties;
    QMap<QString, VisuPropertyMeta> mPropertiesMeta;

    // methods
    void notifyInstruments();

signals:
    void initialUpdate(const VisuSignal* const);
    void signalUpdated(const VisuSignal* const);

public:
    static const QString TAG_NAME;

    VisuSignal(const QMap<QString, QString>& properties);
    const QMap<QString, QString>& getProperties();
    const QMap<QString, VisuPropertyMeta>& getPropertiesMeta();
    void setPropertiesMeta(const QMap<QString, VisuPropertyMeta>& meta);
    void load();
    void updateProperty(QString key, QString value);
    void initializeInstruments();
    void datagramUpdate(const VisuDatagram& datagram);
    void set_raw_ralue(quint64 value);
    quint64 getRawValue() const;
    void set_timestamp(quint64 mTimestamp);
    quint64 getTimestamp() const;
    int getSerialPlaceholder() const;
    bool getSerialTransform() const;
    quint16 getId() const;
    void setId(quint16 id);
    double getFactor() const;
    double getOffset() const;
    double getRealValue() const;
    double getNormalizedValue() const;
    double getMin() const;
    double getMax() const;
    QString getName() const;
    QString getUnit() const;

    // observer interface
    void connectInstrument(VisuInstrument* instrument);
    void disconnectInstrument(VisuInstrument* instrument);
};

#endif // SIGNAL_H
