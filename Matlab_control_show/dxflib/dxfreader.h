#ifndef DXFREADER_H
#define DXFREADER_H

#include <QObject>
#include "dxflib/dl_dxf.h"
#include "dxflib/dl_creationadapter.h"

class DxfReader : public QObject, public DL_CreationAdapter
{
    Q_OBJECT
public:
    struct DxfText {
        QString Text;
    };
    explicit DxfReader(const QString &fileName, QObject *parent = nullptr);

    virtual void addLine(const DL_LineData &data) override;
    virtual void addText(const DL_TextData &data) override;
    virtual void addArc(const DL_ArcData &data) override;


    QList<DL_LineData> dxfLines;
    QList<DL_TextData> dxfText;
    QList<DL_ArcData> dxfArc;
private:
    QString fileName;
};

#endif // DXFREADER_H
