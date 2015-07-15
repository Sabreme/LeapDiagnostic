#ifndef LEAPWINDOW_H
#define LEAPWINDOW_H

#include <QWidget>
#include "QVTKWidget.h"

#include "vtkArrowSource.h"
#include "vtkSphereSource.h"
#include "vtkActor.h"
#include "vtkPlaneSource.h"
#include "vtkPlaneWidget.h"
#include "vtkLineWidget.h"
#include "vtkLineWidget2.h"
#include "vtkLegendScaleActor.h"
#include "vtkleapscaleactor.h"
#include "vtkPointWidget.h"
#include "vtkSliderWidget.h"

#include "Leap.h"
//#include "Sample.h"

using namespace Leap;

namespace Ui {
class LeapWindow;
}

class LeapWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit LeapWindow(QWidget *parent = 0);
    ~LeapWindow();
    void LoadSphere();
    void LoadAxisArrow();
    void LoadPlane();
    void LoadFingers();
    void LoadFingerTips();
    void LoadAxes();
    void LoadPointer();
    void LoadSlider();
    void KeyPressTracker();



    
private slots:
    void on_buttonApply_clicked();

    void on_buttonPrint_clicked();

    void updateMe();

    void on_checkBoxFingers_clicked();

    void on_checkBxFingerTips_clicked();

    void on_buttonTest_clicked();

private:
    Ui::LeapWindow *ui;

    vtkRenderer                 * global_Renderer;
    vtkRenderWindow             * global_Window;
    vtkRenderWindowInteractor   * global_Interactor;

    vtkSphereSource             * global_Sphere;
    vtkArrowSource              * global_Arrow;
    vtkPlaneSource              * global_Plane;
    vtkPlaneWidget              * global_PlaneWidget;
    vtkLineWidget               * global_LineWidget;

    vtkActor                    * global_SphereActor;
    vtkActor                    * global_ArrowActor;
    vtkActor                    * global_FingerTip[5];
    vtkLineWidget               * global_Fingers[5];
    vtkLegendScaleActor         * global_Legend;
    vtkLeapScaleActor           * global_LeapLegend;    


    vtkPointWidget              * pointWidget;

    vtkSliderWidget         *global_Slider;

    double global_SphereTop[3];
    double global_SphereBottom[3];
    double global_SphereCenter[3];
    double global_SphereRadius;

    double global_FingerClr[5][3];
    bool global_Initialised = false;

    double global_LastScalePosition;

    double global_CameraPosition;

    //double** global_TipClr = new double*[5];

    //SampleListener              * listener_;
    Controller                  * controller_;

};

#endif // LEAPWINDOW_H
