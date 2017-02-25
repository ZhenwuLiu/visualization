#include "visuhelper.h"
#include "exceptions/configloadexception.h"
#include "statics/staticimage.h"
#include <QByteArray>
#include "visumisc.h"

namespace VisuHelper
{
    QString transformKey(const QString &key)
    {
        return key.mid(1, 1).toLower() + key.mid(2);
    }

    void checkIfKeyExists(QString key, QMap<QString, QString> properties)
    {
        if (!properties.contains(key))
        {
            throw ConfigLoadException("Missing property: %1", key);
        }
    }

    void handleMissingKey(const QString &key,
                          QMap<QString, QString>& properties,
                          const QMap<QString, VisuPropertyMeta>& metaProperties)
    {
        if (!properties.contains(key))
        {
            qDebug("Missing property: %s", key.toStdString().c_str());
            properties[key] = metaProperties[key].defaultVal;
        }
    }

    template<>
    double get<double>(const QString& key,
                       QMap<QString, QString>& properties,
                       const QMap<QString, VisuPropertyMeta>& metaProperties)
    {
        handleMissingKey(key, properties, metaProperties);
        return properties[key].toDouble();
    }

    template<>
    QString get<QString>(const QString& key,
                         QMap<QString, QString>& properties,
                         const QMap<QString, VisuPropertyMeta>& metaProperties)
    {
        handleMissingKey(key, properties, metaProperties);
        return properties[key];
    }

    template<>
    QColor get<QColor>(const QString& key,
                       QMap<QString, QString>& properties,
                       const QMap<QString, VisuPropertyMeta>& metaProperties)
    {
        handleMissingKey(key, properties, metaProperties);

        QColor color = VisuMisc::strToColor(properties[key]);
        if (!color.isValid())
        {
            throw ConfigLoadException("Wrong color format (%1)", properties[key]);
        }
        return color;
    }

    template<>
    QImage get<QImage>(const QString& key,
                       QMap<QString, QString>& properties,
                       const QMap<QString, VisuPropertyMeta>& metaProperties)
    {
        handleMissingKey(key, properties, metaProperties);
        return VisuMisc::strToImage(properties[key], properties[StaticImage::KEY_FORMAT]);
    }

    template<>
    bool get<bool>(const QString& key,
                   QMap<QString, QString>& properties,
                   const QMap<QString, VisuPropertyMeta>& metaProperties)
    {
        handleMissingKey(key, properties, metaProperties);
        return (properties[key].toInt() != 0);
    }
}
