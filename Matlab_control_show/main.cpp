#include "Matlab_control_show.h"
#include <QtWidgets/QApplication>

#include <vtkAutoInit.h>		//初始化VTK模块,一定要加，不然一堆报错
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
