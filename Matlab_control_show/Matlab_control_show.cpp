#include "Matlab_control_show.h"
#pragma execution_character_set("utf-8")//���������������
Matlab_control_show::Matlab_control_show(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	// ���� VTK �ĳ�ʼ���ã��������������ʾstlģ��
	initVTK();


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
	m_lineSeries->setPointsVisible(true);                         // �������ݵ�ɼ�
	m_lineSeries->setName("curve");                            // ͼ������
	//�����Ƕ��������Խ�������
	// ����QPen����������������
	QPen pen;
	pen.setWidth(0.7); // �����������Ϊ2����
	// ����������������QPen���ԣ�������ɫ
	pen.setColor(Qt::red); // ����������ɫΪ��ɫ
	// ��QPenӦ����QLineSeries����
	m_lineSeries->setPen(pen);


	m_chart = new QChart();                                        // ����ͼ�����
	m_chart->addAxis(m_axisY, Qt::AlignLeft);                      // ��X����ӵ�ͼ����
	m_chart->addAxis(m_axisX, Qt::AlignBottom);                    // ��Y����ӵ�ͼ����
	m_chart->addSeries(m_lineSeries);                              // �����߶�����ӵ�ͼ����
	m_chart->setAnimationOptions(QChart::SeriesAnimations);        // ��������ʹ���߻�����ʾ�ĸ�ƽ��������Ч�����ÿ�

	m_lineSeries->attachAxis(m_axisX);                             // ���߶��������X�ᣬ�˲��������m_chart->addSeries֮��
	m_lineSeries->attachAxis(m_axisY);                             // ���߶��������Y�ᣬ�˲��������m_chart->addSeries֮��

	ui.graphicsView->setChart(m_chart);                           // ��ͼ��������õ�graphicsView�Ͻ�����ʾ
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);       // ������Ⱦ������ݣ������������ô���߾��Եò�ƽ��



}

Matlab_control_show::~Matlab_control_show()
{}

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
		m_timer->start(200);
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





		//
		file.close();
		// �����Ҫȷ����ȡ��1000������
		if (data1.size() != 1000)
		{
			qDebug() << "Warning: The number of read data entries is not equal to 1000.";
		}
		// �����������֮ǰ������ɵ����ݵ�
		m_lineSeries->clear();

		for (int i = 0; i < 1000; i++)
		{
			m_lineSeries->append(QPointF((double)i, data1[i]));
		}
	}
	num++;
}

