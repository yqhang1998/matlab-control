#include "dxfreader.h"
#include <QDebug>

DxfReader::DxfReader(const QString &fileName, QObject *parent)
	: QObject(parent)
	, fileName(fileName)
{
	// ¶ÁÈ¡ dxf ÎÄ¼þ
	DL_Dxf *dxf = new DL_Dxf;
	if (!dxf->in(fileName.toStdString().c_str(), this)) { // if file open failed
		std::cerr << fileName.toStdString() << " could not be opened.\n";
		return;
	}
	delete dxf;
	dxf = nullptr;
}

void DxfReader::addLine(const DL_LineData &data)
{
	dxfLines << data;
}

void DxfReader::addText(const DL_TextData &data)
{
	dxfText << data;
	//    qDebug() << QString("%1").arg(data.text.data());
}

void DxfReader::addArc(const DL_ArcData &data)
{
	dxfArc << data;
}
