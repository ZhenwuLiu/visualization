#ifndef VISUWIDGET_H
#define VISUWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include "visupropertyloader.h"
#include "visupropertymeta.h"

class VisuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisuWidget(
            QWidget *parent,
            QMap<QString, QString> properties,
            QMap<QString, VisuPropertyMeta> metaProperties) :   QWidget(parent),
                                                                mProperties(properties),
                                                                mPropertiesMeta(metaProperties)
    {
        setObjectName(VisuWidget::OBJECT_NAME);
        mActive = false;
    }

    virtual bool updateProperties(const QString &key, const QString &value);
    void loadProperties();
    static const QString TAG_NAME;
    static const QString KEY_ID;
    static const QString KEY_WIDTH;
    static const QString KEY_HEIGHT;
    static const QString KEY_X;
    static const QString KEY_Y;
    static const QString KEY_NAME;
    static const QString KEY_TYPE;

    QMap<QString, QString>& getProperties();
    void setPropertiesMeta(QMap<QString, VisuPropertyMeta> meta);
    QMap<QString, VisuPropertyMeta> getPropertiesMeta();
    QString getName();
    void setName(QString name);
    void setPosition(QPoint position);
    const QSize sizeHint();
    QString getType();
    void setActive(bool active);
    quint16 getId() const;
    void setId(quint16 id);

    // Drag&drop related
    QPoint mDragStartPosition;
    QPoint mDragStartRelativePosition;
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
    QPoint getRelativeOffset();
    void drawActiveBox(QPainter* painter);
    virtual bool refresh(const QString& key);

    static const QString OBJECT_NAME;

signals:
    void widgetActivated(VisuWidget*);
    void widgetContextMenu(VisuWidget*);

protected:

    // properties map
    QMap<QString, QString> mProperties;
    QMap<QString, VisuPropertyMeta> mPropertiesMeta;

    quint16 cId;
    QString cName;
    quint16 cX;                  // x position in pixels
    quint16 cY;                  // y position in pixels
    quint16 cWidth;              // width in pixels
    quint16 cHeight;             // height in pixels

    QSize mSize;
    QString mTagName;
    bool    mActive;        // true when widget is active in wysiwyg editor

    void setup();
};

#endif // VISUWIDGET_H
