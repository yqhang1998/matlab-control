// 读取STL文件
vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
reader->SetFileName("myModel.stl");
reader->Update();

// 创建一个渲染器，并添加STL模型
vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
renderWindow->AddRenderer(renderer);

// 创建模型的变换
vtkSmartPointer<vtkTransform> modelTransform = vtkSmartPointer<vtkTransform>::New();
modelTransform->Translate(10, 20, 30);   // 移动模型到新位置

vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
transformFilter->SetInputConnection(reader->GetOutputPort());
transformFilter->SetTransform(modelTransform);
transformFilter->Update();

// 包装为一个 actor
vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
mapper->SetInputConnection(transformFilter->GetOutputPort());
vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
actor->SetMapper(mapper);

renderer->AddActor(actor);

// 创建坐标轴
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