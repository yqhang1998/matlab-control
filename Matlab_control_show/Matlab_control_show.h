#pragma once
#include <QtWidgets/QMainWindow>

#include <QtCharts/QtCharts>
QT_CHARTS_USE_NAMESPACE

#include "ui_Matlab_control_show.h"


#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


//加载cad图
#include "dl_dxf.h"
#include "dl_creationadapter.h"
#include "interactiveview.h"
#include "dxfreader.h"
#include "jlineitem.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QSpacerItem>
#include <QLineEdit>
//#include <vtkSmartPointer.h>
//#include <vtkSTLReader.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
#include <vtkLODActor.h>
#include <vtkTransformFilter.h>
#include <vtkCamera.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkCenterOfMass.h>
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
#include <QLibrary>

#include <QDateTime>


#define _USE_MATH_DEFINES 

class drawThread;

class Matlab_control_show : public QMainWindow
{
	Q_OBJECT

public:
	explicit Matlab_control_show(QWidget *parent = nullptr);
	~Matlab_control_show();

	//void MainWindow::paintEvent(QPaintEvent *event);
	void upPaint();//画b_scan图
	bool eventFilter(QObject *watched, QEvent *event);//b
	void clearLabel(); // 公共方法清除label

	void upPaint1();//画c_scan图
	double differenceMinMax(QVector<double> vec);//返回最大差值

private slots:
	//处理select点击
	void pushButtonSelect_clicked();
	//处理select_done点击
	void pushButtonSelDone_clicked();
	//处理鼠标点击
	void onPointPicked(double* pos);
	//dll加载
	void dllload();


public:
	Ui::Matlab_control_showClass ui;
	void initVTK(const QString &modelname);

	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkActor> actor1;
	vtkSmartPointer<vtkActor> pointsActor;
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
	/*标记曲线 A-scan标记范围*/
	QLineSeries* m_b_start;
	QLineSeries* m_b_end;
	QLineSeries* m_c_start;
	QLineSeries* m_c_end;

	/* 横纵坐标最大显示范围 */
	const int AXIS_MAX_X = 10, AXIS_MAX_Y = 10;

	/* 用来记录数据文件 */
	int num = 1;


//
	bool drawImage;  // 条件变量，用于控制是否绘制图像
	QImage image;
	// 数组，用于存储读取的数据,用于A-SCAN
	QVector<double> data1;
	QString line;

	//b_scan
	QVector<QVector<double>> b_scan;
	int rows;
	int cols;// = 406 - 144 + 1;
	int b_scan_start;
	int b_scan_end;
	void datacalculate(QVector<QVector<double>>& b_scan);
	QImage scaledImage;//大小调整，让image充满整个label

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

	QTcpSocket *socket;
	QTcpServer *server;
	//定义客户端指针链表容器
	QList<QTcpSocket *> clientList;

//
	//生成点的函数
	bool isEven(int num);
	std::vector<std::vector<double>> Fcn_plane_xyzrpy(int count1, int count2, double x_step, double y_step, double x_start, double y_start, double z);
	int x_num, y_num;
	double x_start, y_start, x_step, y_step, z_start;
	std::vector<std::vector<double>> points;
	//画点
	void drawPoint(double x, double y, double z, vtkSmartPointer<vtkRenderer> renderer);
	//点连成线函数
	void point2line(double x1, double y1, double z1, double x2, double y2, double z2, vtkSmartPointer<vtkRenderer> renderer);

	//喇叭口点生成函数
	std::vector<std::vector<double>> Fcn_trumpet_xyzrpy(double cx, double cy, double cz, double step, double tool_len);
	double cx, cy, cz, tool_len,step;
	int samples1, samples2;


private slots:
	void slotBtnClear();
	void slotBtnStartAndStop();
	void slotTimeout();
	//tcp
	void pushButton_clicked();
	void pushButton_2_clicked();
	//config
	void pushButtonConfig_clicked();
	//tcp连接
	void newConnection_slot();
	void readyRead_slot();  //自定义处理readyRead信号的槽函数
	//平面工件路径规划划线
	void drawline();
	//喇叭口工件路径规划画线
	void drawcurve();


//CAd加载部分
public:
	QGraphicsScene* Scene;
	InteractiveView* View;
	qreal XOffset;
	qreal YOffset;
	qreal ZoomDelta;
	// 中心点
	qreal XCenter;
	qreal YCenter;
	QVector<QLineF> dxfLines;
	QList<DL_TextData> dxfText;
private slots:
	void BtnReadDXF_clicked();
	void XOffset_editingFinished();
	void YOffset_editingFinished();
	void Zoom_editingFinished();
	void EditXCenter_editingFinished();
	void EditYCenter_editingFinished();
	void DWF_pushButton_clicked();
	void DWF_pushButton_2_clicked();

//时间显示
private:
	QTimer *timer_datetime;
	QDateTime dt;
	QLabel *currentTimeLabel;
private slots:
	void do_timer_timeout();


//串口
private:
	QSerialPort *comPort;
private slots:
	void do_serialport_close_clicked();//关闭串口
	void do_serialport_clicked();//打开配置串口
	void rece_slots();//接收槽函数
	void do_send_clicked();//发送
	void do_start();//打开喷水
	void do_close();//关闭喷水
};

//vtk部分
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


