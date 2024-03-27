#pragma once
#include <QtWidgets/QMainWindow>

#include <QtCharts/QtCharts>
QT_CHARTS_USE_NAMESPACE

#include "ui_Matlab_control_show.h"

//#include <vtkSmartPointer.h>
//#include <vtkSTLReader.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>


#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRendererCollection.h>
#include <vtkPropPicker.h>
#include <QObject>
#include <vtkAxesActor.h>
//表格
#include <QWidget>
#include <QTimer>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QImage>
#include <QLabel>
#include <QColor>
//读文件
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <QPointF>
#include <QtCharts/QLineSeries>
#include <cmath>
#include <algorithm>
#include <QColor>
#include <QPainter>
class Matlab_control_show : public QMainWindow
{
	Q_OBJECT

public:
	explicit Matlab_control_show(QWidget *parent = nullptr);
	~Matlab_control_show();



private slots:
	//处理select点击
	void on_pushButtonSelect_clicked();
	//处理select_done点击
	void on_pushButtonSelDone_clicked();
	//处理鼠标点击
	void onPointPicked(double* pos);


private:
	Ui::Matlab_control_showClass ui;
	void initVTK();

	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

	vtkSmartPointer<vtkAxesActor> axes;

//绘图部分
	 /* 用于模拟生成实时数据的定时器 */
	QTimer* m_timer;

	/* 图表对象 */
	QChart* m_chart;

	/* 横纵坐标轴对象 */
	QValueAxis *m_axisX, *m_axisY;

	/* 曲线图对象 */
	QLineSeries* m_lineSeries;

	/* 横纵坐标最大显示范围 */
	const int AXIS_MAX_X = 10, AXIS_MAX_Y = 10;

	/* 用来记录数据文件 */
	int num = 1;



private slots:
	void slotBtnClear();
	void slotBtnStartAndStop();
	void slotTimeout();
};


//当一个点被选中时发出该信号
class PointPickedSignal : public QObject
{
	Q_OBJECT
public:
	PointPickedSignal(QObject* parent = nullptr) : QObject(parent) {}
signals:
	void pointPicked(double* pos);
};


//继承自 vtkCommand 类，其功能是监听鼠标左键的点击事件。当用户点击鼠标左键时，会触发 Execute 方法
class MouseInteractorCommand : public vtkCommand
{
public:
	vtkTypeMacro(MouseInteractorCommand, vtkCommand);

	static MouseInteractorCommand* New()
	{
		return new MouseInteractorCommand;
	}

	virtual void Execute(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(callData))
	{
		vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
		int* clickPos = interactor->GetEventPosition();

		vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
		picker->SetTolerance(0.0005);

		if (picker->Pick(clickPos[0], clickPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()))
		{
			double* pos = picker->GetPickPosition();
			memcpy(pickedPoint, pos, sizeof(double) * 3);
			emit signal->pointPicked(pickedPoint);
		}
	}

	double pickedPoint[3];
	PointPickedSignal* signal; // this will emit the pointPicked signal when a point is picked
};
