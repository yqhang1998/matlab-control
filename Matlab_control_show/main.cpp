#include "Matlab_control_show.h"
#include <QtWidgets/QApplication>

#include <vtkAutoInit.h>		//��ʼ��VTKģ��,һ��Ҫ�ӣ���Ȼһ�ѱ���
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Matlab_control_show w;
    w.show();
    return a.exec();
}
