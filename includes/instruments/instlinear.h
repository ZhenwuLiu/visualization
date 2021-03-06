#ifndef INSTLINEAR_H
#define INSTLINEAR_H

#include "visuinstrument.h"

class InstLinear : public VisuInstrument
{
    Q_OBJECT
    public:
        InstLinear(
                QWidget *parent,
                QMap<QString, QString> properties,
                QMap<QString, VisuPropertyMeta> metaProperties) : VisuInstrument(parent, properties, metaProperties)
        {
            loadProperties();
        }
        static const QString TAG_NAME;

        virtual bool updateProperties(const QString &key, const QString &value);
        void loadProperties();

    protected:
        virtual void renderStatic(QPainter *painter);   // Renders to pixmap_static
        virtual void renderDynamic(QPainter *painter);  // Renders to pixmap
        virtual bool refresh(const QString &key);

    private:
        // configuration properties
        quint8 cLineThickness;  // Thickness of geometric primitives drawn
        quint8 cMajorLen;       // Length, in pixels, of major division marks
        quint8 cMinorLen;       // Length, in pixels, of minor division marks
        quint8 cMajorCnt;       // Number of major count divisions
        quint8 cMinorCnt;       // Number of minor count divisions
        quint16 cBarThickness;
        bool cHorizontal;

        // additional properties not related to configuration
        quint16 mBarLength;
        quint16 mMargin;

        void renderDivisions(QPainter* painter);
        void renderLabel(QPainter* painter, int sigCur, quint16 xOfs);
        void setupMargin(QPainter *painter);

        static const QString KEY_HORIZONTAL;
        static const int SPACING = 5;
};

#endif // INSTLINEAR_H
