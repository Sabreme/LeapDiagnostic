#include "leapwindow.h"
#include "ui_leapwindow.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkProperty.h"

#include "vtkCommand.h"
#include "vtkConeSource.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkPointWidget.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProbeFilter.h"
#include "vtkProperty.h"

#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkRendererCollection.h"

#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "QTimer"

#include "vtkCamera.h"
#include "vtkObjectFactory.h"

#include "vtkLineSource.h"
#include "vtkCellArray.h"
#include "vtkLineWidget2.h"
#include "vtkLineRepresentation.h"
#include "vtkCommand.h"

#include "vtkLegendScaleActor.h"
#include "vtkViewport.h"
#include "vtkOutlineFilter.h"
#include "vtkCubeSource.h"

#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkSliderRepresentation2D.h"
#include "vtkSliderRepresentation3D.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"


#include <sstream>

//// Callback for the interaction

class vtkmyPWCallback : public vtkCommand
{
public:
  static vtkmyPWCallback *New()
  {
    return new vtkmyPWCallback;
  }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    vtkPointWidget *pointWidget = reinterpret_cast<vtkPointWidget*>(caller);
    pointWidget->GetPolyData(this->PolyData);
    double position[3];
    pointWidget->GetPosition(position);
    std::ostringstream text;
    text << "cursor: "
         << std::fixed << std::setprecision(4)
         << position[0] << ", " << position[1] << ", " << position[2];
    this->PositionActor->SetInput(text.str().c_str());
    //this->CursorActor->VisibilityOn();
  }
  vtkmyPWCallback():PolyData(0) {}
  vtkPolyData  *PolyData;
  //vtkActor     *CursorActor;
  vtkTextActor *PositionActor;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnKeyPress()
    {
      // Get the keypress
      vtkRenderWindowInteractor *rwi = this->Interactor;
      std::string key = rwi->GetKeySym();

      // Output the key that was pressed
      std::cout << "Pressed " << key << std::endl;

      // Handle an arrow key
      if(key == "Up")
        {
        std::cout << "The up arrow was pressed." << std::endl;
        }

      // Handle a "normal" key
      if(key == "a")
        {
        std::cout << "The a key was pressed." << std::endl;
        }

      // Forward events
      vtkInteractorStyleTrackballCamera::OnKeyPress();
    }

};
vtkStandardNewMacro(KeyPressInteractorStyle);


LeapWindow::LeapWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LeapWindow)
{
    ui->setupUi(this);    
    this->controller_ = new Controller;   
    //this->listener_ = new SampleListener;

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000/60);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMe()));

    timer->start();
}

LeapWindow::~LeapWindow()
{
    delete ui;
}

void LeapWindow::LoadSphere()
{
    vtkSmartPointer<vtkPolyData> inputPolyData ;

    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetPhiResolution(15);
    sphereSource->SetThetaResolution(15);
    sphereSource->Update();
    inputPolyData = sphereSource->GetOutput();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInput(inputPolyData);

    vtkSmartPointer<vtkActor> sphereActor =
            vtkSmartPointer<vtkActor>::New();
    sphereActor->SetMapper(mapper);
    sphereActor->GetProperty()->SetRepresentationToWireframe();
    sphereActor->GetProperty()->SetColor(0.8900, 0.8100, 0.3400 );

    // Create the RenderWindow
    vtkRenderer * ren1 = vtkRenderer::New();
    global_Renderer = ren1;

    vtkRenderWindow * renWin = vtkRenderWindow::New();
    global_Window = renWin;

    renWin->AddRenderer(ren1);

    ren1->AddActor(sphereActor);

    ren1->SetBackground(0.1, 0.2, 0.4);

    this->ui->widget->SetRenderWindow(renWin);
    this->ui->widget->GetInteractor()->SetRenderWindow(renWin);
    global_Interactor = this->ui->widget->GetInteractor();

    sphereSource->GetCenter(global_SphereCenter);
    global_SphereRadius =  sphereSource->GetRadius();


    global_Sphere = sphereSource;
    global_SphereActor = sphereActor;
    sphereActor->RotateY(-90);

    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();
    global_Interactor->MouseWheelBackwardEvent();


    vtkLeapScaleActor * leapScaleActor =
            new vtkLeapScaleActor();


    //leapScaleActor->BuildRepresentation(global_Renderer->GetViewport());
    leapScaleActor->BuildRepresentation(global_Renderer);
    global_LeapLegend = leapScaleActor;
    //leapScaleActor->PrintSelf(std::cout);
//    global_Renderer->AddActor(leapScaleActor);


}

void LeapWindow::LoadAxes()
{

}

void LeapWindow::LoadPointer()
{

    vtkSmartPointer<vtkCubeSource> cubeSource =
            vtkSmartPointer<vtkCubeSource>::New();
    //cubeSource->SetBounds(global_Renderer->ComputeVisiblePropBounds());
    cubeSource->SetBounds(-1, 1, -1, 1, -1, 1);
    cubeSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
//    mapper->SetInput();


    /// The plane widget is used probe the dataset.
    vtkSmartPointer<vtkPolyData> point =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkProbeFilter> probe =
            vtkSmartPointer<vtkProbeFilter>::New();
    probe->SetInput(point);
    probe->SetSource(cubeSource->GetOutput());

    ///
    /// create glyph
    ///

    vtkSmartPointer<vtkSphereSource> pointMarker =
            vtkSmartPointer<vtkSphereSource>::New();
    pointMarker->SetRadius(0.1);

    vtkSmartPointer<vtkGlyph3D> glyph =
            vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(probe->GetOutputPort());
    glyph->SetSourceConnection(pointMarker->GetOutputPort());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToDataScalingOff();
//    glyph->SetScaleFactor(global_Volume->GetLength() * 0.1);

    vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> glyphActor =
            vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();


    // The SetInteractor method is how 3D widgets are associated with the render
    // window interactor. Internally, SetInteractor sets up a bunch of callbacks
    // using the Command/Observer mechanism (AddObserver()).
    vtkSmartPointer<vtkmyPWCallback> myCallback =
            vtkSmartPointer<vtkmyPWCallback>::New();
    myCallback->PolyData = point;
    //myCallback->CursorActor = glyphActor;
//    myCallback->PositionActor = textActor;

    pointWidget = vtkPointWidget::New();
    pointWidget->SetInteractor(this->ui->widget->GetInteractor());
    //pointWidget->SetInput(global_Reader->GetOutput());
    pointWidget->AllOff();
    pointWidget->PlaceWidget(-1, 1, -1, 1, -1, 1);
    pointWidget->GetPolyData(point);
    pointWidget->EnabledOff();
    //vtkEventConnector->Connect(pointWidget_, vtkCommand::InteractionEvent, this, SLOT(pointWidgetCallBack()));
    pointWidget->AddObserver(vtkCommand::InteractionEvent  ,myCallback);

    // Set the widget colour to GREEN
    //pointWidget->GetProperty()->SetColor(0.0, 1.0, 0.0);

    global_Renderer->AddActor(glyphActor);
//    global_Renderer->AddActor2D(textActor);

    pointWidget->EnabledOn();
    global_Initialised = true;

}

void LeapWindow::LoadSlider()
{
    vtkSmartPointer<vtkSliderRepresentation3D> sliderRep =
      vtkSmartPointer<vtkSliderRepresentation3D>::New();

    sliderRep->SetMinimumValue(scaling_Min);
    sliderRep->SetMaximumValue(scaling_Max);
    sliderRep->SetValue(scaling_Start);

    // Set color properties:
    // Change the color of the knob that slides
    sliderRep->GetSliderProperty()->SetColor(0,1,0);//Green

    // Change the color of the text indicating what the slider controls
///    sliderRep->GetTitleProperty()->SetColor(1,0,0);//red


    // Change the color of the text displaying the value
 ///   sliderRep->GetLabelProperty()->SetColor(1,0,0);//red
///    sliderRep->

    //Change the color of the text displaying the value location
    sliderRep->ShowSliderLabelOff();

    // Change the color of the knob when the mouse is held on it
    sliderRep->GetSelectedProperty()->SetColor(0,1,0);//green

    // Change the color of the bar
    sliderRep->GetTubeProperty()->SetColor(1,1,1);//white
    sliderRep->GetTubeProperty()->SetLineWidth(3);

    // Change the color of the ends of the bar
    sliderRep->GetCapProperty()->SetColor(0,1,0);//Green
    sliderRep->GetCapProperty()->SetLineWidth(2);

//    sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
//    sliderRep->GetPoint1Coordinate()->SetValue(450 ,420);
//    sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
//    sliderRep->GetPoint2Coordinate()->SetValue(150, 420);

    sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
    sliderRep->GetPoint1Coordinate()->SetValue(450 ,420);
    sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
    sliderRep->GetPoint2Coordinate()->SetValue(150, 420);

    global_Slider  = vtkSliderWidget::New();
    global_Slider->SetInteractor(this->ui->widget->GetInteractor());
    global_Slider->SetRepresentation(sliderRep);
    global_Slider->SetAnimationModeToAnimate();

    global_Slider->EnabledOn();
    global_CameraPosition = 7;
}

void LeapWindow::KeyPressTracker()
{
    vtkSmartPointer<KeyPressInteractorStyle> style =
            vtkSmartPointer<KeyPressInteractorStyle>::New();

    global_Interactor->SetInteractorStyle(style);
    style->SetCurrentRenderer(global_Renderer);
}

void LeapWindow::LoadAxisArrow()
{
    vtkSmartPointer<vtkArrowSource> arrowSource =
            vtkSmartPointer<vtkArrowSource>::New();

    double startPoint[3], endPoint[3];
#ifndef main
    vtkMath::RandomSeed(time(NULL));
#else
    vtkMath::RandomSeed(8775070);
#endif

    startPoint[0] = 0;
    startPoint[1] = 0;
    startPoint[2] = 0.5;

    endPoint[0] = 0;
    endPoint[1] = 0;
    endPoint[2] = -0.5;

    // Compute a basis
    double normalizedX[3];
    double normalizedY[3];
    double normalizedZ[3];

    //THe X axis is a vector from start to end

    vtkMath::Subtract(endPoint, startPoint, normalizedX);
    ///double length = vtkMath::Norm(normalizedX);
    double length = global_SphereRadius * 2;


    vtkMath::Normalize(normalizedX);

    // THe Z axis is an arbitrary vector cross X

    double arbitratry[3];

    arbitratry[0]  = vtkMath::Random(-10, 10);
    arbitratry[1]  = vtkMath::Random(-10, 10);
    arbitratry[2]  = vtkMath::Random(-10, 10);
    vtkMath::Cross(normalizedX,arbitratry,normalizedZ);
    vtkMath::Normalize(normalizedZ);

    // The Y axis is Z cross X
    vtkMath::Cross(normalizedZ,normalizedX,normalizedY);
    vtkSmartPointer<vtkMatrix4x4> matrix =
            vtkSmartPointer<vtkMatrix4x4>::New();


    //Create the direction cosine matrix
    matrix->Identity();
    for (unsigned int i = 0; i < 3; i++)
    {
        matrix->SetElement(i, 0,normalizedX[i]);
        matrix->SetElement(i, 1, normalizedY[i]);
        matrix->SetElement(i, 2, normalizedZ[i]);
    }

    // Apply the transforms
    vtkSmartPointer<vtkTransform> transform =
            vtkSmartPointer<vtkTransform>::New();

    transform->Translate(startPoint);
    transform->Concatenate(matrix);
    transform->Scale(length, length, length);

    // Transform the polydata
    vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformPD->SetTransform(transform);
    transformPD->SetInputConnection(arrowSource->GetOutputPort());

    //Create a mapper and actor for the arrow
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();

#ifdef USER_MATRIX
    mapper->SetInputConnection(arrowSource->GetOutputPort());
    actor->SetUserMatrix(transform->GetMatrix());
#else
    mapper->SetInputConnection(transformPD->GetOutputPort());
#endif
    actor->SetMapper(mapper);


    global_Renderer->AddActor(actor);    

    global_Arrow = arrowSource;
    global_ArrowActor = actor;
    arrowSource;

    /// Align the arrow with the Widget
    actor->RotateY(-90);

}

void LeapWindow::LoadPlane()
{
    ///
    /// \brief vtkPlaneWidget
    ///
    global_PlaneWidget = vtkPlaneWidget::New();
    global_PlaneWidget->SetInput(global_Sphere->GetOutput());
    global_PlaneWidget->NormalToXAxisOn();
    global_PlaneWidget->SetResolution(20);
    global_PlaneWidget->SetRepresentationToOutline();
    global_PlaneWidget->PlaceWidget();

    global_PlaneWidget->SetInteractor(this->ui->widget->GetInteractor());
    global_PlaneWidget->EnabledOn();
}

void LeapWindow::LoadFingers()
{
    double p0[3] = {0.0, -1.0, 0.0};
    double p1[3] = {0.0, 1.0 , 0.0};

    for (int i = 0; i < 5;i++)
    {
        global_Fingers[i] = vtkLineWidget::New();
        global_Fingers[i]->SetInteractor(this->ui->widget->GetRenderWindow()->GetInteractor());
    }


}

void LeapWindow::LoadBones()
{
    for (int f = 0; f < 5; f++)
    {
        for (int b = 0; b < 4 ; b++)
        {
            global_Bones[f][b] = vtkLineWidget::New();
            global_Bones[f][b]->SetInteractor(this->ui->widget->GetRenderWindow()->GetInteractor());
        }
    }
}



void LeapWindow::LoadFingerTips()
{
    ///Fingertip Start Positions

    double tipPos[5][3] = {
                            {-0.67, 0   , 0},
                            {-0.35, 0.47, 0},
                            { 0   , 0.58, 0},
                            { 0.33, 0.46, 0},
                            { 0.5 , 0.26, 0}
                           };

    ///Fingertip Colors


    double tipClr[5][3] = {
                            { 1   , 0   , 0},
                            { 1   , 1   , 0},
                            { 0   , 0   , 1},
                            { 1   , 0   , 1},
                            { 0   , 1   , 1},
                           };

    double tipBlck[3] = { 0   , 0   , 0};


    vtkSmartPointer<vtkSphereSource> fingerTipSource =
            vtkSmartPointer<vtkSphereSource>::New();
    fingerTipSource->SetRadius(0.05);

    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(fingerTipSource->GetOutputPort());

    for (int i = 0; i < 5; i++)
    {
         global_FingerTip[i] = vtkActor::New();
         global_FingerTip[i]->SetMapper(mapper);
         global_FingerTip[i]->GetProperty()->SetColor(tipBlck);

         global_FingerTip[i]->SetPosition(tipPos[i]);
         global_Renderer->AddActor(global_FingerTip[i]);

         global_FingerClr[i][0] = tipClr[i][0];
         global_FingerClr[i][1] = tipClr[i][1];
         global_FingerClr[i][2] = tipClr[i][2];

    }
}



void LeapWindow::on_buttonApply_clicked()
{
    LoadSphere();

    LoadAxisArrow();

    LoadPlane();

    LoadFingers();

    LoadFingerTips();

    LoadPointer();

    LoadSlider();

}

void LeapWindow::on_checkBoxFingers_clicked()
{
    for (int i =0; i < 5; i++)
    {
        if (this->ui->checkBoxFingers->isChecked())
            global_Fingers[i]->On();
        else
            global_Fingers[i]->Off();
    }

}

void LeapWindow::on_checkBxFingerTips_clicked()
{


    for (int i =0; i < 5; i++)
    {
        if (this->ui->checkBoxFingerTips->isChecked())
            global_FingerTip[i]->VisibilityOn();
        else
            global_FingerTip[i]->VisibilityOff();
    }

}

void LeapWindow::on_buttonPrint_clicked()
{
    KeyPressTracker();

    //global_LeapLegend->PrintSelf(std::cout);


}

void LeapWindow::updateMe()
{
    this->ui->widget->GetRenderWindow()->Render();

    if(controller_->isConnected())  // controller is a controller object
    {
        /// Get the most recent frame and report some basic information
        const Frame frame = controller_->frame();


        bool shouldTranslate = true;
        bool shouldRotate = true;
        bool shouldScale = true;
        bool shouldCheck  = false;

        double frameScale;
        Leap::Matrix mtxFrameTransform ;


        if (global_Initialised == true)
        {
            pointWidget->GetProperty()->SetColor(1.0, 1.0, 1.0);
            global_SphereActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
            static_cast<vtkSliderRepresentation3D*>(global_Slider->GetRepresentation())->SetValue(scaling_Start);
        }


        mtxFrameTransform.origin = Leap::Vector(0.0, 0.0, 0.0);
        frameScale = 0.0075;

        shouldTranslate = frame.translationProbability(controller_->frame(1)) > 0.40;
        shouldRotate = frame.rotationProbability(controller_->frame(1)) > 0.40;
        shouldScale = frame.scaleProbability(controller_->frame(1)) > 0.40;
        //pointWidget->GetProperty()->SetColor(0.0, 1.0, 0.0);

        //        /// Get the most recent frame and report some basic information
        //        std::cout   << "Frame id: " << frame.id()
        //                    << ", timestamp: " << frame.timestamp()
        //                    << ", hands: " << frame.hands().count()
        //                    << ", fingers: " << frame.fingers().count()
        //                    << ", Rotation (Y): " << shouldRotate
        //                    << ", Translate (Y): " << shouldTranslate
        //                    << ", Scale (Y): " << shouldScale
        //                    //<< ", tools: " << frame.tools().count()
        //                    //<< ", gestures: " << frame.gestures().count()
        //                    //<< ", PalmNormal: " << frame.hands()[0].palmNormal()
        //                    //<< ", PalmDirect: " << frame.hands()[0].direction()
        //                    << ", yaw: " << frame.hands()[0].direction().yaw()
        //                    //<< ", Angle: " << frame.rotationAngle(controller_->frame(1))
        //                    << std::endl;

//        std::cout << "TRACKING: " << frame.id() << endl;

        if (!frame.hands().isEmpty() && !frame.hands()[0].fingers().isEmpty())
        {
            //                    Get the first hand
            const Hand hand = frame.hands().rightmost();
            //                    Get the 2nd hand
            const Hand hand2 = frame.hands().leftmost();



            if(frame.hands().count() ==1)
            {
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                //////////////////////////    FingerTip Callibration TRACKING  /////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////

                Vector handPos = hand.palmPosition();
                Pointable frontFinger = controller_->frame(1).fingers().frontmost();

                double sensitivity = 0.01;
                double offSetX = 0;
                double offSetY = 2.5;
                double offSetZ = 1;

                double handPosPoint[3] = {handPos.x * sensitivity - offSetX,
                                          handPos.y * sensitivity - offSetY,
                                          handPos.z * sensitivity - offSetZ
                                         };

                pointWidget->SetPosition(handPosPoint);//handPos.
                pointWidget->GetProperty()->SetColor(0.0, 1.0, 0.0);

            }
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    Hand TRACKING  /////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if((frame.hands().count() ==1)       &&
                    (frame.fingers().count() >= 4))
            {
                Vector newNormal = hand.palmNormal();

                double oldNormal[3] ;
                global_PlaneWidget->GetNormal(oldNormal);
                double newNormalD [3];
                double theta, rotVector[3];

                double *point1 = global_PlaneWidget->GetPoint1();
                double *origin = global_PlaneWidget->GetOrigin();
                double *center = global_PlaneWidget->GetCenter();


                newNormalD[0] = newNormal.x;
                newNormalD[1] = newNormal.y;
                newNormalD[2] = newNormal.z;

                global_PlaneWidget->SetNormal(newNormal.x, newNormal.y, newNormal.z);
                global_PlaneWidget->UpdatePlacement();


                ///Compute the rotation vector using a transformation matrix
                ///Note that is fnromals are aparelle then te rotation is either 0 or 180 Degrees

                double dp = vtkMath::Dot(oldNormal, newNormalD);
                if (dp >= 1.0)
                {
                    return;    ///zero rotation
                }

                else if (dp <= -1.0)
                {
                    theta = 180.0;
                    rotVector[0] = point1[0] - origin[0];
                    rotVector[1] = point1[1] - origin[1];
                    rotVector[2] = point1[2] - origin[2];
                }
                else
                {
                    vtkMath::Cross(oldNormal, newNormalD,rotVector);
                    theta = vtkMath::DegreesFromRadians(acos(dp));
                }

                global_ArrowActor->RotateWXYZ(theta, rotVector[0], rotVector[1], rotVector[2]);
                global_SphereActor->RotateWXYZ(theta, rotVector[0], rotVector[1], rotVector[2]);
                global_SphereActor->GetProperty()->SetColor(0.0, 1.0, 0.0);

            //    global_PlaneWidget->Print(std::cout);

                /// Compute the Scale Factor using the leap motion factor

                /// The following code checks to see if the sensor has regained focus.
                /// if so, we set the global_CameraPosition to the default value
                /// Effectively functioning as a reset value.
                /// We also have a skip value to true to not invert the slider.

                bool do_Invert = true;
                if (abs(controller_->frame(1).id() - global_ScaleFactorID) > 15 )
                {
                    global_CameraPosition = static_cast<vtkSliderRepresentation3D*>(global_Slider->GetRepresentation())->GetValue();
//                    std::cout << "Return focus" << endl;
                    do_Invert = false;
                }

                global_ScaleFactorID = frame.id();       //Current Frame

                float scaleFactor = frame.hands()[0].scaleFactor(controller_->frame(2));

                double oldPosition = global_CameraPosition;

                global_CameraPosition = oldPosition / (scaleFactor) ;

                double newPosition = global_CameraPosition;

                /// We add color chromatic scale to the Slider Widget Propoert to highligh strength

                double colourRange = (newPosition /  scaling_Max) ;

                if (colourRange < 0) colourRange = 0;
                else
                if(colourRange > 1) colourRange = 1;

                if (scaleFactor > 1.0000001)            /// EXPANDING .... ColourRange Getting SMALLER - Blue Adjustment
                {
                     static_cast<vtkSliderRepresentation3D*>(global_Slider->GetRepresentation())->SetValue(newPosition );
                    static_cast<vtkSliderRepresentation3D*>(global_Slider->GetRepresentation())->GetTubeProperty()->SetColor(colourRange,colourRange,1);
                }
                else                                           /// SCHINKING.... ColourRange Getting BIGGER -- Red Adjustment
                {
                    static_cast<vtkSliderRepresentation3D*>(global_Slider->GetRepresentation())->SetValue(newPosition );
                    static_cast<vtkSliderRepresentation3D*>(global_Slider->GetRepresentation())->GetTubeProperty()->SetColor(1,1-colourRange,1-colourRange);
                }

//                 std::cout  << "scaleFactor: " << scaleFactor
//                              << "\t, oldPosition: "  << oldPosition
//                              << "\t, newPosition: "  << newPosition
//                              << "\t, colourRange: "  << colourRange
//                              << "\t, CameraPos: "  << global_CameraPosition
//                             <<  "\t, FrameID: "  << frame.id()
//                              << "\t" << endl;


            } /// Hand tracking


            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////    FINGER TRACKING  ///////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////

            if (this->ui->checkBoxFingers->isChecked() || this->ui->checkBoxFingerTips->isChecked())
            {
                ///Set default finger values
                ///
                for (int t =0; t < 5; t++)
                {
                    global_FingerTip[t]->VisibilityOff();
                }


                if((frame.hands().count() ==1)       &&
                        (frame.fingers().count() >= 1 && frame.fingers().count() < 6))
                {

                    const Leap::FingerList fingers = frame.fingers();
                    double handDropPos = 1;

                    for (Leap::FingerList::const_iterator f1 = fingers.begin(); f1 != fingers.end(); f1++)
                    {
                        Leap::Bone bone;
                        Leap::Bone::Type boneType;
                        for(int b = 0; b < 4; b++)
                        {
                            boneType = static_cast<Leap::Bone::Type>(b);
                            bone = (*f1).bone(boneType);
                            std::cout << "Finger index: " << (*f1).type() << " " << bone <<  "x1" << std::endl;
                        }
                    };

                    for(int i =0; i < fingers.count(); i++)
                    {
                        const Leap::Finger finger = fingers[i];
//                        Leap::Vector vStartPos = mtxFrameTransform.transformPoint(finger.stabilizedTipPosition() * frameScale);
//                        Leap::Vector vEndPos = mtxFrameTransform.transformDirection(finger.direction()) * 30;

                        Leap::Vector vStartPos = mtxFrameTransform.transformPoint(finger.stabilizedTipPosition() * frameScale);
                        Leap::Vector vEndPos = mtxFrameTransform.transformDirection(finger.direction()) * 30;

                        //Leap::Vector v

                        if (this->ui->checkBoxFingerTips->isChecked())
                        {                                                        
                            //global_FingerTip[i]->SetPosition(vStartPos.x, vStartPos.y -handDropPos, vStartPos.z);
                            global_FingerTip[i]->SetPosition(vStartPos.x, vStartPos.y, vStartPos.z);
                            global_FingerTip[i]->GetProperty()->SetColor(global_FingerClr[i]);
                            global_FingerTip[i]->VisibilityOn();
                        }

                        double * output;

                        /// Optional Lines (Very confusing)
                        if (this->ui->checkBoxFingers->isChecked())
                        {
//                            double startPoint [3] = {vStartPos.x, vStartPos.y -handDropPos, vStartPos.z} ;
//                            double endPoint [3] = {vEndPos.x, vEndPos.y -handDropPos, vEndPos.z};
                            double startPoint [3] = {vStartPos.x, vStartPos.y, vStartPos.z} ;
                            double endPoint [3] = {vEndPos.x, vEndPos.y, vEndPos.z};
                            double radius = 25;

                          //  std::cout  << "endPoint A: " << endPoint[0] << ","  << endPoint[1] << ","  << endPoint[2] << "\t" << endl;


                            //// SHORT LINE FUNCTION C++ Too Silly for Simple Array Return Function
                            double dx = endPoint[0] - startPoint[0];
                            double dy = endPoint[1] - startPoint[1];
                            double dz = endPoint[2] - startPoint[2];

                         //   std::cout << "A dx = " << dx << "A dy = " << dy << "A dz = " << dz << endl;

                            double length = std::sqrt(dx * dx + dy * dy + dz * dz);

                            if (length > 0)
                            {
                                dx /=length;
                                dy /=length;
                                dz /=length;
                            }


                            dx *= length - radius;
                            dy *= length - radius;
                            dz *= length - radius;

                          //  std::cout << "B dx = " << dx << "B dy = " << dy << "B dz = " << dz << endl;


                            double newDistance[3] = {
                                                     startPoint[0] + dx,
                                                     startPoint[1] + dy,
                                                     startPoint[2] + dz
                                                    };

                          //  std::cout  << "endPointB: " << newDistance[0] << ","  << newDistance[1] << ","  << newDistance[2] << "\t" << endl;

                            //global_Fingers[i]->SetPoint1(vStartPos.x, vStartPos.y, vStartPos.z);
                            global_Fingers[i]->SetPoint1(startPoint);
                            global_Fingers[i]->GetLineProperty()->SetColor(global_FingerClr[i]);
                            //global_Fingers[i]->SetPoint2(vEndPos.x, vEndPos.y , vEndPos.z);
                            global_Fingers[i]->SetPoint2(newDistance);                            

                            global_Fingers[i]->On();

                            output = newDistance;
                        }

                        /// NEW BONES DISPLAY Lines (SHOULD BE Very NON CONFUSING confusing)
                        if (this->ui->checkBoxBones->isChecked())
                        {
//                            double startPoint [3] = {vStartPos.x, vStartPos.y -handDropPos, vStartPos.z} ;
//                            double endPoint [3] = {vEndPos.x, vEndPos.y -handDropPos, vEndPos.z};

                            for (int b = 0; b <  4 ; b++)
                            {
//                                global_Bones[i][b]->SetPoint1
//                                        (
//                                            finger.bone.prevJoint().x,
//                                            finger.bone.prevJoint().y,
//                                            finger.bone.prevJoint().z,
//                                            );

//                                global_Bones[i][b]->SetPoint2
//                                        (
//                                            finger.bone.nextJoint().x,
//                                            finger.bone.nextJoint().y,
//                                            finger.bone.nextJoint().z,
//                                            );

                               global_Bones[i][b]->GetLineProperty()->SetColor(global_FingerClr[i]);
                                //global_Fingers[i]->SetPoint2(vEndPos.x, vEndPos.y , vEndPos.z);


                                global_Bones[i][b]->On();

                            }
//                            double startPoint [3] = {vStartPos.x, vStartPos.y, vStartPos.z} ;
//                            double endPoint [3] = {vEndPos.x, vEndPos.y, vEndPos.z};
//                            double radius = 25;

//                          //  std::cout  << "endPoint A: " << endPoint[0] << ","  << endPoint[1] << ","  << endPoint[2] << "\t" << endl;


//                            //// SHORT LINE FUNCTION C++ Too Silly for Simple Array Return Function
//                            /// Basically generates a line between 2 vectors which are 3D points
//                            double dx = endPoint[0] - startPoint[0];
//                            double dy = endPoint[1] - startPoint[1];
//                            double dz = endPoint[2] - startPoint[2];

//                         //   std::cout << "A dx = " << dx << "A dy = " << dy << "A dz = " << dz << endl;

//                            double length = std::sqrt(dx * dx + dy * dy + dz * dz);

//                            if (length > 0)
//                            {
//                                dx /=length;
//                                dy /=length;
//                                dz /=length;
//                            }


//                            dx *= length - radius;
//                            dy *= length - radius;
//                            dz *= length - radius;

//                          //  std::cout << "B dx = " << dx << "B dy = " << dy << "B dz = " << dz << endl;


//                            double newDistance[3] = {
//                                                     startPoint[0] + dx,
//                                                     startPoint[1] + dy,
//                                                     startPoint[2] + dz
//                                                    };

                          //  std::cout  << "endPointB: " << newDistance[0] << ","  << newDistance[1] << ","  << newDistance[2] << "\t" << endl;

                            //global_Fingers[i]->SetPoint1(vStartPos.x, vStartPos.y, vStartPos.z);
                            ///global_Fingers[i]->SetPoint1(startPoint);


                        }
                    }
                }
            } /// Finger Tracking
        }
    }
}


void LeapWindow::on_buttonTest_clicked()
{
    //LoadSlider();
}
