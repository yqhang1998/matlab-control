#include "Matlab_control_show.h"
#pragma execution_character_set("utf-8")//解决中文乱码问题

//sasa(0, 1, 5, 5, 1, 20480, 0, 10);

//extern "C" _declspec(dllexport) int sasa(int swId, int chId, float probeFreq,
//	int extVolt,
//	int transceiverMode,
//	int scanLength,
//	int readMode,
//	int aScanGain);

Matlab_control_show::Matlab_control_show(QWidget *parent)
	: QMainWindow(parent),XOffset(9700),
	YOffset(6100),
	ZoomDelta(1),
	XCenter(0),
	YCenter(0)
{
	ui.setupUi(this);
	// 配置 VTK 的初始设置，这个函数用来显示stl模型
	

	//initVTK("trumpet");
	//combobox
	QObject::connect(ui.comboBox, &QComboBox::currentTextChanged,
		[=](const QString& text) {
		if (text == "平面")
		{
			initVTK("222");
		}
		else if (text == "喇叭口")
		{
			initVTK("trumpet");
		}
	});
	//创建服务器对象
	server = new QTcpServer();

	//绘图曲线部分
	m_timer = new QTimer(this);
	m_timer->setSingleShot(false);
	m_timer->setInterval(50);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
	//清除曲线
	QObject::connect(ui.btnClear, SIGNAL(clicked(bool)), this, SLOT(slotBtnClear()));
	//启动定时器
	QObject::connect(ui.btnStartAndStop, SIGNAL(clicked(bool)), this, SLOT(slotBtnStartAndStop()));
	//label显示边框
	ui.label->setStyleSheet("border: 1px solid black");
	ui.label_2->setStyleSheet("border: 1px solid black");
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
	m_b_start = new QLineSeries;
	m_b_end = new QLineSeries;
	m_c_start = new QLineSeries;
	m_c_end = new QLineSeries;

	m_lineSeries->setPointsVisible(true);                         // 设置数据点可见
	m_b_start->setPointsVisible(true);
	m_b_end->setPointsVisible(true);
	m_c_start->setPointsVisible(true);
	m_c_end->setPointsVisible(true);

	m_lineSeries->setName("curve");                            // 图例名称
	m_b_start->setName("b-scan range");
	m_c_start->setName("c-scan range");
	//下面是对曲线属性进行设置
	// 设置QPen，定义线条的属性
	QPen pen,pen1,pen2;
	pen.setWidth(0.7); // 设置线条宽度为2像素
	pen1.setWidth(0.7);
	pen2.setWidth(0.7);
	// 还可以设置其他的QPen属性，例如颜色
	pen.setColor(Qt::red); // 设置线条颜色为红色
	pen1.setColor(Qt::black);
	pen2.setColor(Qt::blue);
	// 将QPen应用于QLineSeries对象
	m_lineSeries->setPen(pen);
	m_b_start->setPen(pen1);
	m_b_end->setPen(pen1);
	m_c_start->setPen(pen2);
	m_c_end->setPen(pen2);

	m_chart = new QChart();                                        // 创建图表对象
	m_chart->addAxis(m_axisY, Qt::AlignLeft);                      // 将X轴添加到图表上
	m_chart->addAxis(m_axisX, Qt::AlignBottom);                    // 将Y轴添加到图表上

	m_chart->addSeries(m_lineSeries);                              // 将曲线对象添加到图表上
	m_chart->addSeries(m_b_start);
	m_chart->addSeries(m_b_end);
	m_chart->addSeries(m_c_start);
	m_chart->addSeries(m_c_end);

	m_chart->setAnimationOptions(QChart::SeriesAnimations);        // 动画：能使曲线绘制显示的更平滑，过渡效果更好看

	m_lineSeries->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
	m_lineSeries->attachAxis(m_axisY);                             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

	m_b_start->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
	m_b_start->attachAxis(m_axisY);                             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

	m_b_end->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
	m_b_end->attachAxis(m_axisY);                             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

	m_c_start->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
	m_c_start->attachAxis(m_axisY);                             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

	m_c_end->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
	m_c_end->attachAxis(m_axisY);                             // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

	ui.graphicsView->setChart(m_chart);                           // 将图表对象设置到graphicsView上进行显示
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);       // 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑

	//隐藏部分图例
	m_chart->legend()->markers(m_b_end)[0]->setVisible(false);
	m_chart->legend()->markers(m_c_end)[0]->setVisible(false);
	//
	ui.label->installEventFilter(this);//b_scan
	ui.label_2->installEventFilter(this);//c_scan
	//控制绘图与否
	drawImage = false;
	//image1 = QImage(rows_c, cols_c, QImage::Format_RGB32);   
	image1 = QImage(rows_c, cols_c, QImage::Format_ARGB32);
	image1.fill(QColor(Qt::transparent));  //这行代码会将图像设置为透明


	//cad加载部分
	Scene = new QGraphicsScene;
	Scene->setBackgroundBrush(QBrush(QColor(0, 0, 0)));
	View = new InteractiveView(this);
	View->setInteractive(true);
	View->setDragMode(QGraphicsView::ScrollHandDrag);
	View->setScene(Scene);
	View->setRenderHints(QPainter::SmoothPixmapTransform);
	View->setGeometry(1500, 100, 150, 150);
	View->show();
	ui.EditXCenter->setText(QString::number(XCenter, 'f', 3));
	ui.EditYCenter->setText(QString::number(YCenter, 'f', 3));
	ui.XOffset->setText(QString::number(XOffset, 'f', 3));
	ui.YOffset->setText(QString::number(YOffset, 'f', 3));
	ui.Zoom->setText(QString::number(ZoomDelta, 'f', 3));

}

Matlab_control_show::~Matlab_control_show()
{
	if (server != nullptr)
	{
		delete server;
		server = nullptr;
	}
	socket = nullptr;
}

void Matlab_control_show::initVTK(const QString &modelname)
{
	// 你的文件路径
	QString filePath = "C:/Users/yqh/Desktop/";
	filePath += modelname+ ".stl";
		// 读取 STL 文件  创建 STL 读取器
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	//reader->SetFileName("C:/Users/yqh/Desktop/222.stl");
	//reader->SetFileName("C:/Users/yqh/Desktop/trumpet.stl");
	reader->SetFileName(filePath.toStdString().c_str());
	reader->Update();

	// 创建VTK渲染器和坐标轴对象
	axes = vtkSmartPointer<vtkAxesActor>::New();
	// 设定坐标轴的位置
	axes->SetAxisLabels(1); //打开坐标轴标签
	axes->SetTotalLength(50, 50, 50);
	//// 将现有的坐标轴绕x轴旋转，但是旋转后坐标还是按照以前的坐标系来计算，无法更新坐标 ，所以使用将模型移动
	//vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
	//rotation->RotateX(-90);  // 顺时针旋转90°
	//// Apply the rotation to the axes actor
	//axes->SetUserTransform(rotation);
	// 创建一个 vtkTransform 对象
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	// 设置旋转，以度为单位
	// RotateWXYZ 方法的第一个参数是旋转角度，后三个参数是旋转轴上的向量
	transform->RotateWXYZ(90, 1.0, 0.0, 0.0); // 沿着X轴旋转45度

	// 创建一个 vtkTransformPolyDataFilter 对象，将 vtkTransform 应用到 PolyData 对象
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetTransform(transform);
	transformFilter->SetInputConnection(reader->GetOutputPort());
	transformFilter->Update();


	// 创建映射器和演员 大家可以想像成一个舞台表演人员穿着表演服饰。Mapper(映射器)：把不同的数据类型，转成图形数据。Actor(演员)：执行渲染mapper的对象。
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	/*mapper->SetInputData(reader->GetOutput());*/
	mapper->SetInputData(transformFilter->GetOutput());


	//设置位置
	// 计算模型的几何中心
	vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
	centerOfMassFilter->SetInputConnection(transformFilter->GetOutputPort());
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();
	double center[3];
	centerOfMassFilter->GetCenter(center);

	// 创建actor并设置位置
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	// 以模型几何中心为基准，将模型位置设置为原点
	actor->SetPosition(-center[0], -center[1], /*-center[2]*/0);
	std::cout << -center[0] << " " << -center[1] << " " << -center[2];



	// 创建 VTK 渲染器 渲染器顾名思义是用来渲染图像的，将演员加入渲染器，相当于舞台表演人员站上了舞台。
		// 在这里检查并清除已存在的模型数据
	if (renderer != nullptr) {
		renderer->RemoveAllViewProps(); // 移除所有的演员
	}
	else {
		renderer = vtkSmartPointer<vtkRenderer>::New();
	}



	ui.qvtkWidget->GetRenderWindow()->AddRenderer(renderer);//这一步是使用qt的不同之处从ui界面获取qvtkwidget控件的渲染窗口，将渲染器加进去。渲染窗口可以理解成一个剧院，里面有舞台、演员。就可以很好的展示图形了。
	// 添加演员到渲染器
	renderer->AddActor(actor);
	// 将坐标轴对象添加到渲染器中
	renderer->AddActor(axes);
	// 设置渲染器背景色等属性
	renderer->SetBackground(0.1, 0.1, 0.1);
	// 渲染窗口初始化
	ui.qvtkWidget->GetRenderWindow()->Render();



}

void Matlab_control_show::pushButtonSelect_clicked() {
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

void Matlab_control_show::pushButtonSelDone_clicked() {
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
	clearLabel();
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
		m_timer->start(20);
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

		////c_scan数据处理
		slice_vector = data1.mid(c_scan_start-1, c_scan_end-c_scan_start+2);
		a = (num - 1) / 31;
		b = (num - 1) % 31;//取值范围0~30
		if (a % 2 == 0)
		{
			c_scan[b] = differenceMinMax(slice_vector);
			//qDebug()<<a<<" r"<<b<<" c "<<differenceMinMax(slice_vector);
		}
		else
		{
			c_scan[30 - b] = differenceMinMax(slice_vector);
			//qDebug()<<a<<" r"<<(30-b)<<" c "<<differenceMinMax(slice_vector);
		}

		////到此结束c_scan

		//B-SCAN  存储的格式为31行，263列，每一行都是每一个data1数据中存一部分进去
		for (int i = b_scan_start; i <= b_scan_end; ++i)
		{
			// 将每一个文件中的144-406范围内的数据加入b_scan，并且进行处理
			b_scan[b][i - b_scan_start] = data1[i];
		}

		//b_scan  下面的应该在读取了31个文件后处理
		if (a != 0 && b == 0)
		{
			datacalculate(b_scan);
			//qDebug()<<"b_scan data ok\n";
			//b_scan.clear(); 用了这个之后b_scan会变0，访问会出错
			//c_scan数据处理

			normalize(c_scan);
			drawImage = true;
			ui.label->update();
			// std::cout<<num<<std::endl;
			ui.label_2->update();
		}
		
		file.close();
		// 在添加新数据之前，清除旧的数据点
		m_lineSeries->clear();

		for (int i = 0; i < 1000; i++)
		{
			m_lineSeries->append(QPointF((double)i, data1[i]));
		}
	}
	data1.clear();
	//////更新扫描轨迹
	point2line(points[num][0], points[num][1], points[num][2], points[num+1][0], points[num+1][1], points[num+1][2], renderer);
	if(num%3==0)
		ui.qvtkWidget->GetRenderWindow()->Render();
	//更换文件
	num++;
}


void Matlab_control_show::clearLabel()
{
	drawImage = false;  // 设置不绘制图像
	ui.label->update();   // 触发重绘，eventFilter会拦截到paint事件
	ui.label_2->update();
}
//过滤器
bool Matlab_control_show::eventFilter(QObject *watched, QEvent *event)
{
	//画b_scan
	if (watched == ui.label && event->type() == QEvent::Paint && drawImage)
	{
		upPaint();
		return true;
	}
	//画c_scan
	if (watched == ui.label_2 && event->type() == QEvent::Paint && drawImage)
	{
		upPaint1();
		return true;
	}
	return QWidget::eventFilter(watched, event);
}


void Matlab_control_show::upPaint()
{
	QPainter painter(ui.label);
	QImage image(rows, cols, QImage::Format_Grayscale8); // 竖直显示
	// 将图片数据填充为数组数据
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j)
		{
			image.setPixelColor(i, j, qRgb(b_scan[i][j], b_scan[i][j], b_scan[i][j])); // 水平显示
		}
	}
	//将整个显示充满label中
	scaledImage = image.scaled(ui.label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	painter.drawImage(0, 0, scaledImage); // 在（0，0）位置处绘制图像
}

void Matlab_control_show::upPaint1()
{
	QPainter painter(ui.label_2);
	for (int i = 0; i < 31; ++i)
	{
		//if (c_scan[i] > 0.8)
		//{
		//	drawPoint(points[i][0],points[i][1],z_start,renderer);
		//}
		QColor color = valueToColor(c_scan[i]);
		image1.setPixelColor(i, a-1, color);
	}
	//将整个显示充满label中
	//scaledImage = image1.scaled(ui.label_2->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  Qt::KeepAspectRatio
	scaledImage = image1.scaled(ui.label_2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	painter.drawImage(0, 0, scaledImage); // 在（0，0）位置处绘制图像

}



//b_scan数据进行处理
void Matlab_control_show::datacalculate(QVector<QVector<double>>& b_scan)
{
	int rows = b_scan.size(); // b_scan 的行数
	int cols = b_scan.first().size(); // b_scan 的列数，假设所有行长度都相同

	// 初始化为double可能表示的最大可能值和最小可能值
	double minVal = -1;//std::numeric_limits<double>::max();
	double maxVal = 1.9;// std::numeric_limits<double>::lowest();

	// 遍历 b_scan 中的每个元素找到最小值和最大值
	for (const QVector<double>& vec : b_scan) {
		for (double val : vec) {
			if (val < minVal) {
				minVal = val;
			}
			if (val > maxVal) {
				maxVal = val;
			}
		}
	}
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			double normalizedValue = (b_scan[r][c] - minVal) / (maxVal - minVal); // 归一化到 [0.0, 1.0]
			b_scan[r][c] = static_cast<uchar>(normalizedValue * 255.0); // 映射到 [0, 255]
		}
	}
}

double Matlab_control_show::differenceMinMax(QVector<double> vec)
{
	auto it_max = std::max_element(vec.begin(), vec.end());
	auto it_min = std::min_element(vec.begin(), vec.end());
	return *it_max - *it_min;
}

QVector<double> Matlab_control_show::normalize(QVector<double>& vec) {
	double minVal = 0.126; //*std::min_element(vec.begin(), vec.end());
	double maxVal = 0.51;//*std::max_element(vec.begin(), vec.end());

	for (double &val : vec) {
		val = (val - minVal) / (maxVal - minVal);
	}

	return vec;
}

QColor Matlab_control_show::valueToColor(float value) {

	QColor color;
	color.setHsvF(2.0 / 3.0 - value * 2.0 / 3.0, 1.0, 1.0, 1.0);
	return color;
}

void Matlab_control_show::pushButton_clicked()
{
	//获取UI界面的端口号
	quint16 port = ui.lineEdit_2->text().toUInt();
	//将服务器设置为被动监听状态
	//bool QTcpServer::listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0)
	//参数1：要监听的主机地址，如果是any，表示监听所有主机地址，也可以给特定的主机地址进行监听
	//参数2：通过指定的端口号进行访问服务器，如果是0，表示由服务器自动分配。如果非0，则表示指定端口号
	//返回值：成功返回真，失败返回假
	if (!server->listen(QHostAddress::Any, port))
	{
		QMessageBox::critical(this, "失败", "服务器启动失败");
	}
	else {
		QMessageBox::information(this, "成功", "服务器启动成功");
	}

	//执行到这表明服务器启动成功，并对客户端连接进行监听，如果有客户端向服务器发来连接请求，那么该服务器就会自动发射一个newConnection信号
	//我们可以将信号连接到对应的槽函数中处理相关逻辑
	connect(server, &QTcpServer::newConnection, this, &Matlab_control_show::newConnection_slot);
}

void Matlab_control_show::newConnection_slot()
{
	qDebug() << "有客户端申请连接";

	//获取最新连接的客户端套接字
	//[virtual] QTcpSocket *QTcpServer::nextPendingConnection()
	QTcpSocket *s = server->nextPendingConnection();

	//将获取的套接字存放到客户端容器中
	clientList.push_back(s);

	//此时，客户端就和服务器建立起来联系了
	//如果客户端有数据向服务器发送过来，那么该套接字就会自动发送一个readyread信号
	//我们可以将该信号连接到自定义的槽函数中处理相关逻辑
	connect(s, &QTcpSocket::readyRead, this, &Matlab_control_show::readyRead_slot);
}

//关于readyRead信号对应槽函数的实现
void Matlab_control_show::readyRead_slot()
{
	//删除客户端链表中的无效客户端套接字
	for (int i = 0; i < clientList.count(); i++)
	{
		//判断套接字的状态
		//函数原型     SocketState state() const;
		//功能：返回客户端状态
		//返回值：客户端状态，如果是0，表示无连接
		if (clientList[i]->state() == 0)
		{
			clientList.removeAt(i);     //将下标为i的客户端移除
		}
	}

	//遍历所有客户端，查看是哪个客户端发来数据
	for (int i = 0; i < clientList.count(); i++)
	{
		//函数原型：qint64 bytesAvailable() const override;
		//功能：返回当前客户端套接字中的可读数据字节个数
		//返回值：当前客户端待读的字节数，如果该数据0，表示无待读数据
		if (clientList[i]->bytesAvailable() != 0)
		{
			//读取当前客户端的相关数据
			//函数原型：QByteArray readAll();
			//功能：读取当前套接字中的所有数据，并返回一个字节数组
			//返回值：数据的字节数组
			QByteArray msg = clientList[i]->readAll();

			//将数据战术到ui界面上
			ui.listWidget->addItem(QString::fromLocal8Bit(msg));

			//将接收到的该消息，发送给所有客户端
			for (int j = 0; j < clientList.count(); j++)
			{
				clientList[j]->write(msg);
			}
		}
	}

}

void Matlab_control_show::pushButton_2_clicked()
{
	//server_->close();
}


//参数配置
void Matlab_control_show::pushButtonConfig_clicked()
{
	//b_scan
	rows = 31;
	b_scan_end = ui.lineEdit_4->text().toInt();
	b_scan_start = ui.lineEdit_3->text().toInt();
	cols = b_scan_end - b_scan_start+1;
	b_scan = QVector<QVector<double>>(rows, QVector<double>(cols));

	//c_scan
	c_scan_start = ui.lineEdit_5->text().toInt();
	c_scan_end = ui.lineEdit_6->text().toInt();
	//end
	c_scan = QVector<double>(31);

	if (m_b_start->count() != 0) {
		m_b_start->clear();
	}
	for (int i = -10; i <= 2000; ++i)
	{
		m_b_start->append(b_scan_start, i);
	}

	if (m_b_end->count() != 0) {
		m_b_end->clear();
	}
	for (int i = -10; i <= 2000; ++i)
	{
		m_b_end->append(b_scan_end, i);
	}

	if (m_c_start->count() != 0) {
		m_c_start->clear();
	}
	for (int i = -10; i <= 2000; ++i)
	{
		m_c_start->append(c_scan_start, i);
	}

	if (m_c_end->count() != 0) {
		m_c_end->clear();
	}
	for (int i = -10; i <= 2000; ++i)
	{
		m_c_end->append(c_scan_end, i);
	}


}

//生成平面工件扫查点的函数（机械臂位姿点）
bool Matlab_control_show::isEven(int num) {
	return num % 2 == 0;
}
std::vector<std::vector<double>> Matlab_control_show::Fcn_plane_xyzrpy(int count1, int count2, double x_step, double y_step, double x_start, double y_start, double z) {
	std::vector<std::vector<double>> points;

	for (int j = 0; j <= count1; ++j) {
		for (int i = 0; i <= count2; ++i) {
			double x;
			if (isEven(j)) {
				x = x_start - i * x_step;
			}
			else {
				x = x_start - (count1 - i) * x_step;
			}
			double y = y_start - j * y_step;
			double rx = M_PI;
			double ry = 0;
			double rz = 0;

			std::vector<double> point = { x, y, z, rx, ry, rz };
			points.push_back(point);
		}
	}
	return points;
}

//平面工件路径生成函数
void Matlab_control_show::drawline()
{
	//如果有现有路径就先移除，不然会重复叠加路径的曲线
	if (actor1)
	{
		renderer->RemoveActor(actor1);
	}
	//x起始坐标y起始坐标z坐标以及对应步距的获取
	//x方向采集数量
	x_num = ui.lineEdit_7->text().toInt();
	//y方向采集数量
	y_num = ui.lineEdit_8->text().toInt();
	//x方向步距    
	x_step = ui.lineEdit_9->text().toDouble();
	//y方向步距
	y_step = ui.lineEdit_10->text().toDouble();
	//起点坐标x
	x_start = ui.lineEdit_11->text().toDouble();
	//起点坐标y
	y_start = ui.lineEdit_12->text().toDouble();
	//起点坐标z
	z_start = ui.lineEdit_13->text().toDouble();
	//std::cout << x_num << " " << y_num << " " << x_step << " " << y_step << " " << x_start << " " << y_start << " " << z_start << std::endl;
	//计算出来的坐标点
	points = Fcn_plane_xyzrpy(x_num, y_num, x_step, y_step, x_start, y_start, z_start);
	vtkSmartPointer<vtkPoints> vpoints = vtkSmartPointer<vtkPoints>::New();
	for (size_t i = 0; i < points.size(); ++i)
	{
		vpoints->InsertNextPoint(points[i][0], points[i][1], points[i][2]);
	}
	//连接这些点
	vtkSmartPointer<vtkPolyLine> vpolyLine = vtkSmartPointer<vtkPolyLine>::New();
	vpolyLine->GetPointIds()->SetNumberOfIds(vpoints->GetNumberOfPoints());
	for (unsigned int i = 0; i < points.size(); i++)
	{
		vpolyLine->GetPointIds()->SetId(i, i);
	}
	//创造单元格数组
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(vpolyLine);
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(vpoints);
	polyData->SetLines(cells);
	vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper1->SetInputData(polyData);
	actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(mapper1);
	// 设置线宽
	actor1->GetProperty()->SetLineWidth(2); //设置线宽为2
	// 设置颜色，RGB值范围0-1
	actor1->GetProperty()->SetColor(1.0, 0.0, 0.0); //设置颜色为红色
	renderer->AddActor(actor1);	
	ui.qvtkWidget->GetRenderWindow()->Render();//刷新显示
	//drawPoint(1, 1, 10, renderer);
	//drawPoint(1, 5, 10, renderer);
	//drawPoint(3, 6, 10, renderer);
	//drawPoint(1, 11, 10, renderer);
	//drawPoint(15, 20, 10, renderer);
	//drawPoint(12, 18, 10, renderer);
}
//画缺陷点函数
void Matlab_control_show::drawPoint(double x, double y, double z, vtkSmartPointer<vtkRenderer> renderer)
{
	// 创建点
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->InsertNextPoint(x, y, z);

	// 创建基于点的PolyData
	vtkSmartPointer<vtkPolyData> pointsPolyData = vtkSmartPointer<vtkPolyData>::New();
	pointsPolyData->SetPoints(points);

	// 创建顶点数据
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	vertexFilter->SetInputData(pointsPolyData);
	vertexFilter->Update();

	// 创建映射器并将顶点数据添加到其中
	vtkSmartPointer<vtkPolyDataMapper> pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	pointsMapper->SetInputConnection(vertexFilter->GetOutputPort());

	// 创建actor并将映射器添加到其中
	vtkSmartPointer<vtkActor> pointsActor = vtkSmartPointer<vtkActor>::New();
	pointsActor->SetMapper(pointsMapper);
	pointsActor->GetProperty()->SetPointSize(5);  // 设置点的大小
	pointsActor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // 设置颜色为红色

	// 将actor添加到渲染器中
	renderer->AddActor(pointsActor);
}
//点连成线的函数
void Matlab_control_show::point2line(double x1, double y1, double z1, double x2, double y2, double z2, vtkSmartPointer<vtkRenderer> renderer)
{
	// 创建起点和终点
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->InsertNextPoint(x1, y1, z1);
	points->InsertNextPoint(x2, y2, z2);

	// 创建线
	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 0); // 第一个点的索引
	line->GetPointIds()->SetId(1, 1); // 第二个点的索引

	// 创建用于存储 line 的单元格数组
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	lines->InsertNextCell(line);

	// 创建线的PolyData
	vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();
	linesPolyData->SetPoints(points);
	linesPolyData->SetLines(lines);

	// 创建映射器并设置PolyData
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(linesPolyData);

	// 创建actor并设置映射器
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	// 设置线宽
	actor->GetProperty()->SetLineWidth(6); //设置线宽为2
	// 设置颜色，RGB值范围0-1
	actor->GetProperty()->SetColor(0.0, 0.0, 1.0); //设置颜色为红色
	// 将actor添加到渲染器中
	renderer->AddActor(actor);
}

//喇叭口工件路径点生成函数
std::vector<std::vector<double>> Matlab_control_show::Fcn_trumpet_xyzrpy(double cx, double cy, double cz, double step, double tool_len)
{
	int samples= 287.9577 / step;
	double radius2 = 270.0;
	double angle2 = 44.13 * M_PI / 180.0;
	double r_len = angle2 * radius2;
	samples1 = static_cast<int>(round(samples * 80 / (r_len + 80)));
	samples2 = samples - samples1;

	std::vector<std::vector<double>> pts;
	for (int i = 0; i < samples1; ++i)
	{
		std::vector<double> pt(6);
		pt[0] = cx;
		pt[1] = cy + 100 + tool_len;
		pt[2] = cz + 280 - i / (double)(samples1 - 1) * 80;
		pt[3] = 90 * M_PI / 180;
		pt[4] = 0;
		pt[5] = 0;

		pts.push_back(pt);
	}

	for (int j = 0; j < samples2; ++j)
	{
		std::vector<double> pt(6);
		pt[0] = cx;
		pt[1] = cy + 100 + tool_len + (radius2 - tool_len) * (1 - cos(j / (double)samples2 * angle2));
		pt[2] = cz + 200 - (radius2 - tool_len) * sin(j / (double)samples2 * angle2);
		pt[3] = (90 + j / (double)samples2 * angle2 * 180 / M_PI) * M_PI / 180;
		pt[4] = 0;
		pt[5] = 0;

		pts.push_back(pt);
	}
	return pts;
}
//喇叭口画线函数
void Matlab_control_show::drawcurve()
{
	//如果有现有路径就先移除，不然会重复叠加路径的曲线
	if (actor1)
	{
		renderer->RemoveActor(actor1);
	}
	cx = ui.lineEdit_14->text().toDouble();
	cy = ui.lineEdit_15->text().toDouble();
	cz= ui.lineEdit_16->text().toDouble();
	step = ui.lineEdit_17->text().toDouble();
	//计算出来的坐标点 tool_len设置190
	if (!points.empty()) {
		points.clear();
	}
	points = Fcn_trumpet_xyzrpy(cx, cy, cz, step,0);

	vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();

	for (size_t i = 0; i < points.size(); ++i) {
		vtk_points->InsertNextPoint(points[i][0], points[i][1], points[i][2]);
	}

	vtkSmartPointer<vtkPolyLine> polyLine1 = vtkSmartPointer<vtkPolyLine>::New();
	polyLine1->GetPointIds()->SetNumberOfIds(samples1);

	for (unsigned int i = 0; i < samples1; ++i) {
		polyLine1->GetPointIds()->SetId(i, i);
	}

	vtkSmartPointer<vtkPolyLine> polyLine2 = vtkSmartPointer<vtkPolyLine>::New();
	polyLine2->GetPointIds()->SetNumberOfIds(points.size() - samples1);

	for (size_t j = samples1; j < points.size(); ++j) {
		polyLine2->GetPointIds()->SetId(j - samples1, j);
	}

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(polyLine1);
	cells->InsertNextCell(polyLine2);

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(vtk_points);
	polyData->SetLines(cells);

	vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper1->SetInputData(polyData);

	actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(mapper1);
	// 设置线宽
	actor1->GetProperty()->SetLineWidth(6); //设置线宽为2
	// 设置颜色，RGB值范围0-1
	actor1->GetProperty()->SetColor(1.0, 0.0, 0.0); //设置颜色为红色
	renderer->AddActor(actor1);
	ui.qvtkWidget->GetRenderWindow()->Render();
}

//加载dll库
void Matlab_control_show::dllload()
{
		//要加载两个dll库
    //C: / Users / yqh / Desktop /
	QString path_1 = "c.dll";
	QString path_2 = "UTNetServiceUtil.dll";
	QString path_3 = "UTNetUtil.dll";
	QLibrary dllLib_1;
	QLibrary dllLib_2;
	QLibrary dllLib_3;
	dllLib_1.setFileName(path_1);
	dllLib_2.setFileName(path_2);
	dllLib_3.setFileName(path_3);
	if(dllLib_1.load()&& dllLib_2.load()&& dllLib_3.load())
	{
		ui.label_22->setText("success");
	/*dllLib_1.unload();*/
	}
	else
	{
		ui.label_22->setText("load UTNetServiceUtil.dll error: " + dllLib_1.errorString()+ dllLib_2.errorString() + dllLib_3.errorString());
	}
	typedef bool(*SasaFunc)(int, int, float, int, int, int, int, int);
	SasaFunc sasa = (SasaFunc)dllLib_1.resolve("sasa");

	if (sasa) 
	{
		qDebug() << "Could  locate the function";
		while (!sasa(0, 1, 5, 5, 1, 20480, 0, 10))
		{
			qDebug() << "aaaaa";
			Sleep(100);
		}
	}
	qDebug() << "加载成功";
	//dllLib_1.unload();
}



//CAD加载部分
void Matlab_control_show::BtnReadDXF_clicked()
{
	QString file = "C:/Users/yqh/Desktop/9.dxf";
	DxfReader dxfReader(file);
	QPen pen;
	pen.setColor(Qt::yellow);
	pen.setWidth(0);
	for (auto d : dxfReader.dxfLines) {
		QLineF line(d.x1 - XOffset, YOffset - d.y1, d.x2 - XOffset, YOffset - d.y2);
		dxfLines << line;
		QGraphicsLineItem *lineItem = new QGraphicsLineItem(line);
		pen.setCosmetic(true);
		lineItem->setPen(pen);
		Scene->addItem(lineItem);
	}
	pen.setColor(Qt::green);
	pen.setWidth(0);
	for (auto d : dxfReader.dxfArc) {
		// Calculate start and span angles for QPainterPath::arcTo in degrees
		double startAngle = d.angle1;
		double spanAngle = -(d.angle2 - d.angle1);
		qDebug() << d.angle1 << " " << d.angle2 << " " << startAngle << " " << spanAngle;
		// Calculate the bounding rectangle for the arc
		QRectF rect((d.cx - d.radius - XOffset), (YOffset - d.cy - d.radius), 2 * d.radius, 2 * d.radius);

		QPainterPath path;
		qreal startX = d.cx + cos(abs(d.angle1)*M_PI / 180)*d.radius - XOffset;
		qreal startY = YOffset - d.cy - sin(abs(d.angle1)*M_PI / 180)*d.radius;
		path.moveTo(startX, startY);

		path.arcTo(rect, startAngle, -spanAngle);
		QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
		pen.setCosmetic(true);
		pathItem->setPen(pen);
		Scene->addItem(pathItem);
	}
}

void Matlab_control_show::XOffset_editingFinished()
{
	XOffset = ui.XOffset->text().toDouble();
}

void Matlab_control_show::YOffset_editingFinished()
{
	YOffset = ui.YOffset->text().toDouble();
}

void Matlab_control_show::Zoom_editingFinished()
{
	ZoomDelta = ui.Zoom->text().toDouble();
	qreal factor = View->transform().scale(static_cast<qreal>(ZoomDelta), static_cast<qreal>(ZoomDelta)).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;
	View->scale(ZoomDelta, ZoomDelta);
}

void Matlab_control_show::EditXCenter_editingFinished()
{
	XCenter = ui.EditXCenter->text().toDouble();
	View->centerOn(XCenter, YCenter);
}

void Matlab_control_show::EditYCenter_editingFinished()
{
	YCenter = ui.EditYCenter->text().toDouble();
	View->centerOn(XCenter, YCenter);
}

void Matlab_control_show::DWF_pushButton_clicked()
{
#if 0
	qreal x = 20404614;
	qreal y = 3918540;
	QGraphicsEllipseItem* Ellipse = new QGraphicsEllipseItem;
	Ellipse->setRect(x - XOffset, y - YOffset, 30, 30);
	Ellipse->setPen(QColor(Qt::white));
	Ellipse->setBrush(QBrush(QColor(Qt::red)));
	Ellipse->setFlags(QGraphicsItem::ItemIsSelectable);
	Scene->addItem(Ellipse);
	View->centerOn(x - XOffset, y - YOffset);
#endif
	/*
	JTextItem *textItem = new JTextItem("Hello World");
	textItem->setX(0);
	textItem->setY(0);
	textItem->setDefaultTextColor(Qt::white);
	textItem->setScale(0.5);
	textItem->setCacheMode(QGraphicsItem::ItemCoordinateCache);
	textItem->setFlag(QGraphicsItem::ItemClipsToShape);
	Scene->addItem(textItem);
	View->centerOn(0, 0);
	*/

	QPen pen;
	pen.setWidth(0);
	pen.setColor(QColor(212, 205, 34));
	QPolygonF poly;
	poly << QPointF(10, 10) << QPointF(100, 10) << QPointF(100, 100) << QPointF(20, 20);
	for (auto d = poly.begin(); d != poly.end();) {
		auto p1 = *d++;
		if (d == poly.end())
			continue;
		auto p2 = *d;
		QGraphicsLineItem *lineItem = new QGraphicsLineItem(QLineF(p1, p2));
		pen.setCosmetic(true);
		lineItem->setPen(pen);
		Scene->addItem(lineItem);

		//        QGraphicsPolygonItem *item = new QGraphicsPolygonItem(poly);
		//        Scene->addItem(item);
		//        item->setPen(pen);
	}
	View->centerOn(0, 0);
}

void Matlab_control_show::DWF_pushButton_2_clicked()
{
#if 1
	qreal x = 20404614;
	qreal y = 3918540;
	// x - XOffset - 30, y - YOffset - 30, x - XOffset + 30, y - YOffset + 30
	QRectF Rect(x - XOffset, y - YOffset, 5, 5);
	QList<QGraphicsItem *> itemList = Scene->items(Rect);
#else
	QList<QGraphicsItem *> itemList = Scene->selectedItems();
#endif
	for (auto i = 0; i < itemList.size(); i++) {
		Scene->removeItem(itemList[i]);
		delete itemList[i];
	}
	qDebug() << qVersion();
}



