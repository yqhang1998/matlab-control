// ��ȡSTL�ļ�
vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
reader->SetFileName("myModel.stl");
reader->Update();

// ����һ����Ⱦ���������STLģ��
vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
renderWindow->AddRenderer(renderer);

// ����ģ�͵ı任
vtkSmartPointer<vtkTransform> modelTransform = vtkSmartPointer<vtkTransform>::New();
modelTransform->Translate(10, 20, 30);   // �ƶ�ģ�͵���λ��

vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
transformFilter->SetInputConnection(reader->GetOutputPort());
transformFilter->SetTransform(modelTransform);
transformFilter->Update();

// ��װΪһ�� actor
vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
mapper->SetInputConnection(transformFilter->GetOutputPort());
vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
actor->SetMapper(mapper);

renderer->AddActor(actor);

// ����������
vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

vtkSmartPointer<vtkOrientationMarkerWidget> widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
widget->SetOrientationMarker(axes);
widget->SetInteractor(renderWindow->GetInteractor());
widget->SetViewport(0.0, 0.0, 0.4, 0.4);
widget->SetEnabled(1);
widget->InteractiveOn();

renderWindow->Render();

vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
renderWindowInteractor->SetRenderWindow(renderWindow);
renderWindowInteractor->Start();