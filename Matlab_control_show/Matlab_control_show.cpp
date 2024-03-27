#include "Matlab_control_show.h"
#pragma execution_character_set("utf-8")//解决中文乱码问题
Matlab_control_show::Matlab_control_show(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	// 配置 VTK 的初始设置，这个函数用来显示stl模型
	initVTK();


	//绘图曲线部分
	m_timer = new QTimer(this);
	m_timer->setSingleShot(false);
	m_timer->setInterval(50);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

	QObject::connect(ui.btnClear, SIGNAL(clicked(bool)), this, SLOT(slotBtnClear()));
	QObject::connect(ui.btnStartAndStop, SIGNAL(clicked(bool)), this, SLOT(slotBtnStartAndStop()));
	//
	// 创建横纵坐标轴并设置显示范围
	//
	m_axisX = new QValueAxis();
	m_axisY = new QValueAxis();
	m_axisX->setTitleText("x");
	m_axisY->setTitleText("y");
	m_axisX->setMin(0);
	m_axisY->setMin(-1.5);
	m_axisX->setMax(1000);
	m_axisY->setMax(2.5);


	m_lineSeries = new QLineSeries();                             // 创建曲线绘制对象
	m_lineSeries->setPointsVisible(true);                         // 设置数据点可见
	m_lineSeries->setName("curve");                            // 图例名称
	//下面是对曲线属性进行设置
	// 设置QPen，定义线条的属性
	QPen pen;
	pen.setWidth(0.7); // 设置线条宽度为2像素
	// 还可以设置其他的QPen属性，例如颜色
	pen.setColor(Qt::red); // 设置线条颜色为红色
	// 将QPen应用于QLineSeries对象
	m_lineSeries->setPen(pen);


	m_chart = new QChart();                                        // 创建图表对象
	m_chart->addAxis(m_axisY, Qt::AlignLeft);                      // 将X轴添加到图表上
	m_chart->addAxis(m_axisX, Qt::AlignBottom);                    // 将Y轴添加到图表上
	m_chart->addSeries(m_lineSeries);                              // 将曲线对象添加到图表上
	m_chart->setAnimationOptions(QChart::SeriesAnimations);        // 动画：能使曲线绘制显示的更平滑，过渡效果更好看

	m_lineSeries->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
	m_lineSeries->attachAxis(m_axisY);                             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

	ui.graphicsView->setChart(m_chart);                           // 将图表对象设置到graphicsView上进行显示
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);       // 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑



}

Matlab_control_show::~Matlab_control_show()
{}

void Matlab_control_show::initVTK()
{
	// 读取 STL 文件  创建 STL 读取器
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName("C:/Users/yqh/Desktop/222.stl");
	reader->Update();


	// 创建VTK渲染器和坐标轴对象
	//axes = vtkSmartPointer<vtkAxesActor>::New();

	// 创建映射器和演员 大家可以想像成一个舞台表演人员穿着表演服饰。Mapper(映射器)：把不同的数据类型，转成图形数据。Actor(演员)：执行渲染mapper的对象。
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(reader->GetOutput());

	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// 创建 VTK 渲染器 渲染器顾名思义是用来渲染图像的，将演员加入渲染器，相当于舞台表演人员站上了舞台。
	renderer = vtkSmartPointer<vtkRenderer>::New();
	ui.qvtkWidget->GetRenderWindow()->AddRenderer(renderer);//这一步是使用qt的不同之处从ui界面获取qvtkwidget控件的渲染窗口，将渲染器加进去。渲染窗口可以理解成一个剧院，里面有舞台、演员。就可以很好的展示图形了。
	// 添加演员到渲染器
	renderer->AddActor(actor);

	// 将坐标轴对象添加到渲染器中
	//renderer->AddActor(axes);
	// 设置渲染器背景色等属性
	renderer->SetBackground(0.1, 0.1, 0.1);
	// 渲染窗口初始化
	ui.qvtkWidget->GetRenderWindow()->Render();
}



void Matlab_control_show::on_pushButtonSelect_clicked() {
	ui.textBrowser->insertPlainText("Select button clicked!\n");

	PointPickedSignal* signal = new PointPickedSignal(this);
	vtkSmartPointer<MouseInteractorCommand> command = vtkSmartPointer<MouseInteractorCommand>::New();
	command->signal = signal;
	ui.qvtkWidget->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, command);

	QEventLoop loop;
	connect(signal, &PointPickedSignal::pointPicked, this, &Matlab_control_show::onPointPicked);
	connect(signal, &PointPickedSignal::pointPicked, &loop, &QEventLoop::quit);
	loop.exec();

	onPointPicked(command->pickedPoint);
}

void Matlab_control_show::onPointPicked(double* pos) {
	ui.textBrowser->insertPlainText(QString("Point picked: %1 %2 %3\n").arg(pos[0]).arg(pos[1]).arg(pos[2]));
	ui.textBrowser->moveCursor(QTextCursor::End);
	actor->SetOrigin(pos);
}

void Matlab_control_show::on_pushButtonSelDone_clicked() {
	ui.textBrowser->insertPlainText("Selection done, restore the default interactor style.\n");

	// 移除左键按下事件的观察者
	ui.qvtkWidget->GetRenderWindow()->GetInteractor()->RemoveObservers(vtkCommand::LeftButtonPressEvent);

	// 恢复默认的交互器样式。
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	ui.qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}


//表格显示
void Matlab_control_show::slotBtnClear()
{
	m_lineSeries->clear();
}

void Matlab_control_show::slotBtnStartAndStop()
{
	if (m_timer->isActive())
	{
		m_timer->stop();
		ui.btnStartAndStop->setText("启动定时器");
	}
	else
	{
		num = 0;
		m_timer->start(200);
		ui.btnStartAndStop->setText("停止定时器");
	}
}


void Matlab_control_show::slotTimeout()
{
	// 你的文件路径
	QString filePath = "C:/Users/yqh/Desktop/data/";
	filePath += "output"+QString::number(num) + ".txt";
	// 打开文件
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Can't open file for reading.";
	}
	else
	{
		QTextStream in(&file);

		// 数组，用于存储读取的数据,用于A-SCAN
		QVector<double> data1;
		QString line;		
		while (!in.atEnd())
		{
				// 读取文件的一行
				line = in.readLine();
				// 将读取的数据转换为 double 类型，并存储到数组中
				bool ok;
				double value = line.toDouble(&ok);
				if (ok) // 确保转换为 double 成功
				{
					data1.append(value);
				}
		}





		//
		file.close();
		// 如果需要确保读取了1000个数据
		if (data1.size() != 1000)
		{
			qDebug() << "Warning: The number of read data entries is not equal to 1000.";
		}
		// 在添加新数据之前，清除旧的数据点
		m_lineSeries->clear();

		for (int i = 0; i < 1000; i++)
		{
			m_lineSeries->append(QPointF((double)i, data1[i]));
		}
	}
	num++;
}

