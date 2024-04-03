#include "Matlab_control_show.h"
#pragma execution_character_set("utf-8")//���������������
Matlab_control_show::Matlab_control_show(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	// ���� VTK �ĳ�ʼ���ã��������������ʾstlģ��
	initVTK();
	//��������������
	server = new QTcpServer();

	//��ͼ���߲���
	m_timer = new QTimer(this);
	m_timer->setSingleShot(false);
	m_timer->setInterval(50);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

	QObject::connect(ui.btnClear, SIGNAL(clicked(bool)), this, SLOT(slotBtnClear()));
	QObject::connect(ui.btnStartAndStop, SIGNAL(clicked(bool)), this, SLOT(slotBtnStartAndStop()));

	//
	// �������������Ტ������ʾ��Χ
	//
	m_axisX = new QValueAxis();
	m_axisY = new QValueAxis();
	m_axisX->setTitleText("x");
	m_axisY->setTitleText("y");
	m_axisX->setMin(0);
	m_axisY->setMin(-1.5);
	m_axisX->setMax(1000);
	m_axisY->setMax(2.5);


	m_lineSeries = new QLineSeries();                             // �������߻��ƶ���
	m_b_start = new QLineSeries;
	m_b_end = new QLineSeries;
	m_c_start = new QLineSeries;
	m_c_end = new QLineSeries;

	m_lineSeries->setPointsVisible(true);                         // �������ݵ�ɼ�
	m_b_start->setPointsVisible(true);
	m_b_end->setPointsVisible(true);
	m_c_start->setPointsVisible(true);
	m_c_end->setPointsVisible(true);

	m_lineSeries->setName("curve");                            // ͼ������
	m_b_start->setName("b-scan range");
	m_c_start->setName("c-scan range");
	//�����Ƕ��������Խ�������
	// ����QPen����������������
	QPen pen,pen1,pen2;
	pen.setWidth(0.7); // �����������Ϊ2����
	pen1.setWidth(0.7);
	pen2.setWidth(0.7);
	// ����������������QPen���ԣ�������ɫ
	pen.setColor(Qt::red); // ����������ɫΪ��ɫ
	pen1.setColor(Qt::black);
	pen2.setColor(Qt::blue);
	// ��QPenӦ����QLineSeries����
	m_lineSeries->setPen(pen);
	m_b_start->setPen(pen1);
	m_b_end->setPen(pen1);
	m_c_start->setPen(pen2);
	m_c_end->setPen(pen2);



	m_chart = new QChart();                                        // ����ͼ�����
	m_chart->addAxis(m_axisY, Qt::AlignLeft);                      // ��X����ӵ�ͼ����
	m_chart->addAxis(m_axisX, Qt::AlignBottom);                    // ��Y����ӵ�ͼ����

	m_chart->addSeries(m_lineSeries);                              // �����߶�����ӵ�ͼ����
	m_chart->addSeries(m_b_start);
	m_chart->addSeries(m_b_end);
	m_chart->addSeries(m_c_start);
	m_chart->addSeries(m_c_end);

	m_chart->setAnimationOptions(QChart::SeriesAnimations);        // ��������ʹ���߻�����ʾ�ĸ�ƽ��������Ч�����ÿ�



	m_lineSeries->attachAxis(m_axisX);                             // ���߶��������X�ᣬ�˲��������m_chart->addSeries֮��
	m_lineSeries->attachAxis(m_axisY);                             // ���߶��������Y�ᣬ�˲��������m_chart->addSeries֮��

	m_b_start->attachAxis(m_axisX);                             // ���߶��������X�ᣬ�˲��������m_chart->addSeries֮��
	m_b_start->attachAxis(m_axisY);                             // ���߶��������Y�ᣬ�˲��������m_chart->addSeries֮��

	m_b_end->attachAxis(m_axisX);                             // ���߶��������X�ᣬ�˲��������m_chart->addSeries֮��
	m_b_end->attachAxis(m_axisY);                             // ���߶��������Y�ᣬ�˲��������m_chart->addSeries֮��

	m_c_start->attachAxis(m_axisX);                             // ���߶��������X�ᣬ�˲��������m_chart->addSeries֮��
	m_c_start->attachAxis(m_axisY);                             // ���߶��������Y�ᣬ�˲��������m_chart->addSeries֮��

	m_c_end->attachAxis(m_axisX);                             // ���߶��������X�ᣬ�˲��������m_chart->addSeries֮��
	m_c_end->attachAxis(m_axisY);                             // ���߶��������Y�ᣬ�˲��������m_chart->addSeries֮��

	ui.graphicsView->setChart(m_chart);                           // ��ͼ��������õ�graphicsView�Ͻ�����ʾ
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);       // ������Ⱦ������ݣ������������ô���߾��Եò�ƽ��

	//���ز���ͼ��
	m_chart->legend()->markers(m_b_end)[0]->setVisible(false);
	m_chart->legend()->markers(m_c_end)[0]->setVisible(false);
	//
	ui.label->installEventFilter(this);//b_scan
	ui.label_2->installEventFilter(this);//c_scan


	
	

	//���ƻ�ͼ���
	drawImage = false;
	//image1 = QImage(rows_c, cols_c, QImage::Format_RGB32);   
	image1 = QImage(rows_c, cols_c, QImage::Format_ARGB32);
	image1.fill(QColor(Qt::transparent));  //���д���Ὣͼ������Ϊ͸��
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
	// ��ȡ STL �ļ�  ���� STL ��ȡ��
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName("C:/Users/yqh/Desktop/222.stl");
	reader->Update();


	// ����VTK��Ⱦ�������������
	//axes = vtkSmartPointer<vtkAxesActor>::New();

	// ����ӳ��������Ա ��ҿ��������һ����̨������Ա���ű��ݷ��Ρ�Mapper(ӳ����)���Ѳ�ͬ���������ͣ�ת��ͼ�����ݡ�Actor(��Ա)��ִ����Ⱦmapper�Ķ���
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(reader->GetOutput());

	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// ���� VTK ��Ⱦ�� ��Ⱦ������˼����������Ⱦͼ��ģ�����Ա������Ⱦ�����൱����̨������Ավ������̨��
	renderer = vtkSmartPointer<vtkRenderer>::New();
	ui.qvtkWidget->GetRenderWindow()->AddRenderer(renderer);//��һ����ʹ��qt�Ĳ�֮ͬ����ui�����ȡqvtkwidget�ؼ�����Ⱦ���ڣ�����Ⱦ���ӽ�ȥ����Ⱦ���ڿ�������һ����Ժ����������̨����Ա���Ϳ��Ժܺõ�չʾͼ���ˡ�
	// �����Ա����Ⱦ��
	renderer->AddActor(actor);

	// �������������ӵ���Ⱦ����
	//renderer->AddActor(axes);
	// ������Ⱦ������ɫ������
	renderer->SetBackground(0.1, 0.1, 0.1);
	// ��Ⱦ���ڳ�ʼ��
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

	// �Ƴ���������¼��Ĺ۲���
	ui.qvtkWidget->GetRenderWindow()->GetInteractor()->RemoveObservers(vtkCommand::LeftButtonPressEvent);

	// �ָ�Ĭ�ϵĽ�������ʽ��
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	ui.qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}


//�����ʾ
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
		ui.btnStartAndStop->setText("������ʱ��");
	}
	else
	{
		num = 0;
		m_timer->start(20);
		ui.btnStartAndStop->setText("ֹͣ��ʱ��");
	}
}


void Matlab_control_show::slotTimeout()
{
	// ����ļ�·��
	QString filePath = "C:/Users/yqh/Desktop/data/";
	filePath += "output"+QString::number(num) + ".txt";
	// ���ļ�
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Can't open file for reading.";
	}
	else
	{
		QTextStream in(&file);

		// ���飬���ڴ洢��ȡ������,����A-SCAN
		QVector<double> data1;
		QString line;		
		while (!in.atEnd())
		{
				// ��ȡ�ļ���һ��
				line = in.readLine();
				// ����ȡ������ת��Ϊ double ���ͣ����洢��������
				bool ok;
				double value = line.toDouble(&ok);
				if (ok) // ȷ��ת��Ϊ double �ɹ�
				{
					data1.append(value);
				}
		}

		////c_scan���ݴ���
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

		////���˽���c_scan

		//B-SCAN  �洢�ĸ�ʽΪ31�У�263�У�ÿһ�ж���ÿһ��data1�����д�һ���ֽ�ȥ
		for (int i = b_scan_start; i <= b_scan_end; ++i)
		{
			// ��ÿһ���ļ��е�144-406��Χ�ڵ����ݼ���b_scan�����ҽ��д���
			b_scan[b][i - b_scan_start] = data1[i];
		}

		//b_scan  �����Ӧ���ڶ�ȡ��31���ļ�����
		if (a != 0 && b == 0)
		{
			datacalculate(b_scan);
			//qDebug()<<"b_scan data ok\n";
			//b_scan.clear(); �������֮��b_scan���0�����ʻ����
			//c_scan���ݴ���

			normalize(c_scan);
			drawImage = true;
			ui.label->update();
			// std::cout<<num<<std::endl;
			ui.label_2->update();
		}
		
		file.close();
		// �����������֮ǰ������ɵ����ݵ�
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
	drawImage = false;  // ���ò�����ͼ��
	ui.label->update();   // �����ػ棬eventFilter�����ص�paint�¼�
	ui.label_2->update();
}
//������
bool Matlab_control_show::eventFilter(QObject *watched, QEvent *event)
{
	//��b_scan
	if (watched == ui.label && event->type() == QEvent::Paint && drawImage)
	{
		upPaint();
		return true;
	}
	//��c_scan
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
	QImage image(rows, cols, QImage::Format_Grayscale8); // ��ֱ��ʾ
	// ��ͼƬ�������Ϊ��������
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j)
		{
			image.setPixelColor(i, j, qRgb(b_scan[i][j], b_scan[i][j], b_scan[i][j])); // ˮƽ��ʾ
		}
	}
	//��������ʾ����label��
	scaledImage = image.scaled(ui.label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	painter.drawImage(0, 0, scaledImage); // �ڣ�0��0��λ�ô�����ͼ��
}

void Matlab_control_show::upPaint1()
{
	QPainter painter(ui.label_2);
	for (int i = 0; i < 31; ++i)
	{
		QColor color = valueToColor(c_scan[i]);
		image1.setPixelColor(i, a-1, color);
	}
	//��������ʾ����label��
	scaledImage = image1.scaled(ui.label_2->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	painter.drawImage(0, 0, scaledImage); // �ڣ�0��0��λ�ô�����ͼ��

}



//b_scan���ݽ��д���
void Matlab_control_show::datacalculate(QVector<QVector<double>>& b_scan)
{
	int rows = b_scan.size(); // b_scan ������
	int cols = b_scan.first().size(); // b_scan �����������������г��ȶ���ͬ

	// ��ʼ��Ϊ������ֵ����С����ֵ
	double minVal = std::numeric_limits<double>::max();
	double maxVal = std::numeric_limits<double>::lowest();

	// ���� b_scan �е�ÿ��Ԫ���ҵ���Сֵ�����ֵ
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
			double normalizedValue = (b_scan[r][c] - minVal) / (maxVal - minVal); // ��һ���� [0.0, 1.0]
			b_scan[r][c] = static_cast<uchar>(normalizedValue * 255.0); // ӳ�䵽 [0, 255]
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
	//ur_tcp
	//server_ = new QTcpSocket(this);
	//IP_ = ui.lineEdit->text();
	//port_ = ui.lineEdit_2->text();
	//unsigned short port = port_.toUShort();  //ע�⴫���port������
	//server_->abort();//ȡ���������ӣ������׽���
	//server_->connectToHost(QHostAddress(IP_), port);  //ע�⴫��IP������
	//if (!server_->waitForConnected(100))  //�������ӣ����ӳ���ʱ��Ϊ1000ms
	//{
	//	QMessageBox::warning(this, "error", "connect false!!");
	//	return;
	//}

	//��ȡUI����Ķ˿ں�
	quint16 port = ui.lineEdit_2->text().toUInt();
	//������������Ϊ��������״̬
	//bool QTcpServer::listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0)
	//����1��Ҫ������������ַ�������any����ʾ��������������ַ��Ҳ���Ը��ض���������ַ���м���
	//����2��ͨ��ָ���Ķ˿ںŽ��з��ʷ������������0����ʾ�ɷ������Զ����䡣�����0�����ʾָ���˿ں�
	//����ֵ���ɹ������棬ʧ�ܷ��ؼ�
	if (!server->listen(QHostAddress::Any, port))
	{
		QMessageBox::critical(this, "ʧ��", "����������ʧ��");
	}
	else {
		QMessageBox::information(this, "�ɹ�", "�����������ɹ�");
	}

	//ִ�е�����������������ɹ������Կͻ������ӽ��м���������пͻ������������������������ô�÷������ͻ��Զ�����һ��newConnection�ź�
	//���ǿ��Խ��ź����ӵ���Ӧ�Ĳۺ����д�������߼�
	connect(server, &QTcpServer::newConnection, this, &Matlab_control_show::newConnection_slot);
}

void Matlab_control_show::newConnection_slot()
{
	qDebug() << "�пͻ�����������";

	//��ȡ�������ӵĿͻ����׽���
	//[virtual] QTcpSocket *QTcpServer::nextPendingConnection()
	QTcpSocket *s = server->nextPendingConnection();

	//����ȡ���׽��ִ�ŵ��ͻ���������
	clientList.push_back(s);

	//��ʱ���ͻ��˾ͺͷ���������������ϵ��
	//����ͻ�������������������͹�������ô���׽��־ͻ��Զ�����һ��readyread�ź�
	//���ǿ��Խ����ź����ӵ��Զ���Ĳۺ����д�������߼�
	connect(s, &QTcpSocket::readyRead, this, &Matlab_control_show::readyRead_slot);
}

//����readyRead�źŶ�Ӧ�ۺ�����ʵ��
void Matlab_control_show::readyRead_slot()
{
	//ɾ���ͻ��������е���Ч�ͻ����׽���
	for (int i = 0; i < clientList.count(); i++)
	{
		//�ж��׽��ֵ�״̬
		//����ԭ��     SocketState state() const;
		//���ܣ����ؿͻ���״̬
		//����ֵ���ͻ���״̬�������0����ʾ������
		if (clientList[i]->state() == 0)
		{
			clientList.removeAt(i);     //���±�Ϊi�Ŀͻ����Ƴ�
		}
	}

	//�������пͻ��ˣ��鿴���ĸ��ͻ��˷�������
	for (int i = 0; i < clientList.count(); i++)
	{
		//����ԭ�ͣ�qint64 bytesAvailable() const override;
		//���ܣ����ص�ǰ�ͻ����׽����еĿɶ������ֽڸ���
		//����ֵ����ǰ�ͻ��˴������ֽ��������������0����ʾ�޴�������
		if (clientList[i]->bytesAvailable() != 0)
		{
			//��ȡ��ǰ�ͻ��˵��������
			//����ԭ�ͣ�QByteArray readAll();
			//���ܣ���ȡ��ǰ�׽����е��������ݣ�������һ���ֽ�����
			//����ֵ�����ݵ��ֽ�����
			QByteArray msg = clientList[i]->readAll();

			//������ս����ui������
			ui.listWidget->addItem(QString::fromLocal8Bit(msg));

			//�����յ��ĸ���Ϣ�����͸����пͻ���
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
