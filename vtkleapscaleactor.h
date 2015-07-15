#ifndef VTKLEAPSCALEACTOR_H
#include "vtkAxisActor2D.h"
#include "vtkMath.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkWindow.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkCellData.h"
#include "vtkCoordinate.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkIndent.h"

#define VTKLEAPSCALEACTOR_H

class vtkLeapScaleActor
{
public:
    vtkLeapScaleActor();

    void AllAxesOn();
    void AllAxesOff();
    virtual void BuildRepresentation(vtkViewport * viewport);
    void PrintSelf(std::ostream& os);

    ~vtkLeapScaleActor();

    enum AttributeLocation
    {
        DISTANCE=0,
        XY_COORDINATES=1
    };

protected:
    int             LabelMode;

    int             RightBorderOffset;
    int             TopBorderOffset;
    int             LeftBorderOffset;
    int             BottomBorderOffset;
    int             BottomMarginOffset;

    double          CornerOffsetFactor;

    /// The Size axes around the borders of the renderer

    vtkAxisActor2D *YAxisLegend;
    vtkAxisActor2D *XAxisLegend;
    vtkAxisActor2D *ZAxisLegend;


    /// The Suppost for the Legend
    int                  LegendVisibility;
    vtkPolyData         *Legend;
    vtkPoints           *LegendPoints;
    vtkPolyDataMapper2D *LegendMapper;
    vtkActor2D          *LegendActor;
    vtkTextMapper       *LabelMappers[6];
    vtkActor2D          *LabelActors[6];
    vtkTextProperty     *LegendTitleProperty;
    vtkTextProperty     *LegendLabelProperty;
    vtkCoordinate       *Coordinate;

    vtkTimeStamp        BuildTime;


};

#endif // VTKLEAPSCALEACTOR_H
