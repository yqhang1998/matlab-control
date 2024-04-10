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
#include <vtkLine.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
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
#include <vtkAxesActor.h>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <QPointF>
#include <QtCharts/QLineSeries>
#include <cmath>
#include <algorithm>
#include <QPainter>
#include <QMainWindow>
#include <QPixmap>
#include <QPaintEvent>
#include <QRgb>
#include <QLinearGradient>

#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QAbstractSocket>
#include <QTcpServer>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES 


class Matlab_control_show : public QMainWindow
{
	Q_OBJECT

public:
	explicit Matlab_control_show(QWidget *parent = nullptr);
	~Matlab_control_show();

	//void MainWindow::paintEvent(QPaintEvent *event);
	void upPaint();//��b_scanͼ
	bool eventFilter(QObject *watched, QEvent *event);//b
	void clearLabel(); // �����������label

	void upPaint1();//��c_scanͼ
	double differenceMinMax(QVector<double> vec);//��������ֵ


private slots:
	//����select���
	void pushButtonSelect_clicked();
	//����select_done���
	void pushButtonSelDone_clicked();
	//���������
	void onPointPicked(double* pos);



private:
	Ui::Matlab_control_showClass ui;
	void initVTK();

	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkActor> actor1;
	vtkSmartPointer<vtkActor> pointsActor;
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
	/*������� A-scan��Ƿ�Χ*/
	QLineSeries* m_b_start;
	QLineSeries* m_b_end;
	QLineSeries* m_c_start;
	QLineSeries* m_c_end;

	/* �������������ʾ��Χ */
	const int AXIS_MAX_X = 10, AXIS_MAX_Y = 10;

	/* ������¼�����ļ� */
	int num = 1;


//
	bool drawImage;  // �������������ڿ����Ƿ����ͼ��
	QImage image;
	// ���飬���ڴ洢��ȡ������,����A-SCAN
	QVector<double> data1;
	QString line;

	//b_scan
	QVector<QVector<double>> b_scan;
	int rows;
	int cols;// = 406 - 144 + 1;
	int b_scan_start;
	int b_scan_end;
	void datacalculate(QVector<QVector<double>>& b_scan);
	QImage scaledImage;//��С��������image��������label

	//c_scan
	int a;
	int b;
	QVector<double> c_scan;
	int rows_c = 30;
	int cols_c = 30;
	QVector<double> slice_vector;
	QVector<double> normalize(QVector<double>& vec);
	QImage image1;
	QColor valueToColor(float value);
	int c_scan_start;
	int c_scan_end;

	//ur_tcp
	//QTcpSocket *server_;
	//QString IP_;
	//QString port_;
	QTcpSocket *socket;
	QTcpServer *server;
	//����ͻ���ָ����������
	QList<QTcpSocket *> clientList;

//
	//���ɵ�ĺ���
	bool isEven(int num);
	std::vector<std::vector<double>> Fcn_plane_xyzrpy(int count1, int count2, double x_step, double y_step, double x_start, double y_start, double z);
	int x_num, y_num;
	double x_start, y_start, x_step, y_step, z_start;
	std::vector<std::vector<double>> points;
	//����
	void drawPoint(double x, double y, double z, vtkSmartPointer<vtkRenderer> renderer);
	//�������ߺ���
	void point2line(double x1, double y1, double z1, double x2, double y2, double z2, vtkSmartPointer<vtkRenderer> renderer);

private slots:
	void slotBtnClear();
	void slotBtnStartAndStop();
	void slotTimeout();
	//tcp
	void pushButton_clicked();
	void pushButton_2_clicked();
	//config
	void pushButtonConfig_clicked();
	//tcp
	void newConnection_slot();
	void readyRead_slot();  //�Զ��崦��readyRead�źŵĲۺ���
	//·���滮����
	void drawline();

};

//vtk����
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
