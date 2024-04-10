#include "Matlab_control_show.h"
#pragma execution_character_set("utf-8")//解决中文乱码问题
Matlab_control_show::Matlab_control_show(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	// 配置 VTK 的初始设置，这个函数用来显示stl模型
	initVTK();
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

void Matlab_control_show::initVTK()
{
	// 读取 STL 文件  创建 STL 读取器
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName("C:/Users/yqh/Desktop/222.stl");
	reader->Update();
	
	
	
	// 创建VTK渲染器和坐标轴对象
	axes = vtkSmartPointer<vtkAxesActor>::New();
	// 设定坐标轴的位置
	axes->SetAxisLabels(1); //打开坐标轴标签
	axes->SetTotalLength(5, 5, 5); 

	// 创建映射器和演员 大家可以想像成一个舞台表演人员穿着表演服饰。Mapper(映射器)：把不同的数据类型，转成图形数据。Actor(演员)：执行渲染mapper的对象。
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(reader->GetOutput());

	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetPosition(0.0, 0.0, 0.0);
	// 创建 VTK 渲染器 渲染器顾名思义是用来渲染图像的，将演员加入渲染器，相当于舞台表演人员站上了舞台。
	renderer = vtkSmartPointer<vtkRenderer>::New();
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
		b = (num - 1) % 31;
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

	// 初始化为最大可能值和最小可能值
	double minVal = std::numeric_limits<double>::max();
	double maxVal = std::numeric_limits<double>::lowest();

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

//生成点的函数
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

//划线
void Matlab_control_show::drawline()
{

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
	//points = Fcn_plane_xyzrpy(30, 30, 0.1, 0.1, 20, 20, 10);
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
	drawPoint(1, 1, 10, renderer);
	drawPoint(1, 5, 10, renderer);
	drawPoint(3, 6, 10, renderer);
	drawPoint(1, 11, 10, renderer);
	drawPoint(15, 20, 10, renderer);
	drawPoint(12, 18, 10, renderer);
}
//画点
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