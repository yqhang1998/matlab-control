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
//���
#include <QWidget>
#include <QTimer>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QImage>
#include <QLabel>
#include <QColor>
//���ļ�
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
	//����select���
	void on_pushButtonSelect_clicked();
	//����select_done���
	void on_pushButtonSelDone_clicked();
	//���������
	void onPointPicked(double* pos);


private:
	Ui::Matlab_control_showClass ui;
	void initVTK();

	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

	vtkSmartPointer<vtkAxesActor> axes;

//��ͼ����
	 /* ����ģ������ʵʱ���ݵĶ�ʱ�� */
	QTimer* m_timer;

	/* ͼ����� */
	QChart* m_chart;

	/* ������������� */
	QValueAxis *m_axisX, *m_axisY;

	/* ����ͼ���� */
	QLineSeries* m_lineSeries;

	/* �������������ʾ��Χ */
	const int AXIS_MAX_X = 10, AXIS_MAX_Y = 10;

	/* ������¼�����ļ� */
	int num = 1;



private slots:
	void slotBtnClear();
	void slotBtnStartAndStop();
	void slotTimeout();
};


//��һ���㱻ѡ��ʱ�������ź�
class PointPickedSignal : public QObject
{
	Q_OBJECT
public:
	PointPickedSignal(QObject* parent = nullptr) : QObject(parent) {}
signals:
	void pointPicked(double* pos);
};


//�̳��� vtkCommand �࣬�书���Ǽ����������ĵ���¼������û����������ʱ���ᴥ�� Execute ����
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
