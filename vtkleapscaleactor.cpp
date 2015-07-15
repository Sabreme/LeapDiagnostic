#include "vtkleapscaleactor.h"

vtkLeapScaleActor::vtkLeapScaleActor()
{
    this->RightBorderOffset = 50;
    this->TopBorderOffset = 20;     // -
    this->LeftBorderOffset = 50;    // |
    this->BottomBorderOffset = 20;  // _
    this->CornerOffsetFactor = 2.0;
    this->BottomMarginOffset = 10;
//    this->FronBorderOffset = 50;
//    this->BackBorderOffset = 50;

    this->YAxisLegend = vtkAxisActor2D::New();
    this->YAxisLegend->GetPositionCoordinate()->SetCoordinateSystemToViewport();
    this->YAxisLegend->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
    this->YAxisLegend->GetPositionCoordinate()->SetReferenceCoordinate(NULL);
    this->YAxisLegend->SetFontFactor(0.6);
    this->YAxisLegend->SetNumberOfLabels(5);
    this->YAxisLegend->AdjustLabelsOff();

    this->XAxisLegend = vtkAxisActor2D::New();
    this->XAxisLegend->GetPositionCoordinate()->SetCoordinateSystemToViewport();
    this->XAxisLegend->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
    this->XAxisLegend->GetPositionCoordinate()->SetReferenceCoordinate(NULL);
    this->XAxisLegend->SetFontFactor(0.6);
    this->XAxisLegend->SetNumberOfLabels(5);
    this->XAxisLegend->AdjustLabelsOff();

    this->ZAxisLegend = vtkAxisActor2D::New();
    this->ZAxisLegend->GetPositionCoordinate()->SetCoordinateSystemToViewport();
    this->ZAxisLegend->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
    this->ZAxisLegend->GetPositionCoordinate()->SetReferenceCoordinate(NULL);
    this->ZAxisLegend->SetFontFactor(0.6);
    this->ZAxisLegend->SetNumberOfLabels(5);
    this->ZAxisLegend->AdjustLabelsOff();

    this->LegendVisibility = 1;
    this->Legend = vtkPolyData::New();
    this->LegendPoints = vtkPoints::New();
    this->Legend->SetPoints(this->LegendPoints);
    this->LegendMapper = vtkPolyDataMapper2D::New();
    this->LegendMapper->SetInput(this->Legend);
    this->LegendActor = vtkActor2D::New();
    this->LegendActor->SetMapper(this->LegendMapper);

    // Create the legend
    vtkIdType pts[4];
    this->LegendPoints->SetNumberOfPoints(10);
    vtkCellArray *legendPolys = vtkCellArray::New();
    legendPolys->Allocate(legendPolys->EstimateSize(4,4));
    pts[0] = 0; pts[1] = 1; pts[2] = 6; pts[3] = 5;
    legendPolys->InsertNextCell(4,pts);
    pts[0] = 1; pts[1] = 2; pts[2] = 7; pts[3] = 6;
    legendPolys->InsertNextCell(4,pts);
    pts[0] = 2; pts[1] = 3; pts[2] = 8; pts[3] = 7;
    legendPolys->InsertNextCell(4,pts);
    pts[0] = 3; pts[1] = 4; pts[2] = 9; pts[3] = 8;
    legendPolys->InsertNextCell(4,pts);
    this->Legend->SetPolys(legendPolys);
    legendPolys->Delete();

    // Create the cell data
    vtkUnsignedCharArray *colors = vtkUnsignedCharArray::New();
    colors->SetNumberOfComponents(3);
    colors->SetNumberOfTuples(4);
    colors->SetTuple3(0,0,0,0);
    colors->SetTuple3(1,255,255,255);
    colors->SetTuple3(2,0,0,0);
    colors->SetTuple3(3,255,255,255);
    this->Legend->GetCellData()->SetScalars(colors);
    colors->Delete();


    // Now the text. The first five are for the 0,1/4,1/2,3/4,1 labels.
    this->LegendTitleProperty = vtkTextProperty::New();
    this->LegendTitleProperty->SetJustificationToCentered();
    this->LegendTitleProperty->SetVerticalJustificationToBottom();
    this->LegendTitleProperty->SetBold(1);
    this->LegendTitleProperty->SetItalic(1);
    this->LegendTitleProperty->SetShadow(1);
    this->LegendTitleProperty->SetFontFamilyToArial();
    this->LegendTitleProperty->SetFontSize(10);
    this->LegendLabelProperty = vtkTextProperty::New();
    this->LegendLabelProperty->SetJustificationToCentered();
    this->LegendLabelProperty->SetVerticalJustificationToTop();
    this->LegendLabelProperty->SetBold(1);
    this->LegendLabelProperty->SetItalic(1);
    this->LegendLabelProperty->SetShadow(1);
    this->LegendLabelProperty->SetFontFamilyToArial();
    this->LegendLabelProperty->SetFontSize(8);
    for (int i=0; i<6; i++)
    {
        this->LabelMappers[i] = vtkTextMapper::New();
        this->LabelMappers[i]->SetTextProperty(this->LegendLabelProperty);
        this->LabelActors[i] = vtkActor2D::New();
        this->LabelActors[i]->SetMapper(this->LabelMappers[i]);
    }
    this->LabelMappers[5]->SetTextProperty(this->LegendTitleProperty);
    this->LabelMappers[0]->SetInput("0");
    this->LabelMappers[1]->SetInput("1/4");
    this->LabelMappers[2]->SetInput("1/2");
    this->LabelMappers[3]->SetInput("3/4");
    this->LabelMappers[4]->SetInput("1");

    this->Coordinate = vtkCoordinate::New();
    this->Coordinate->SetCoordinateSystemToDisplay();


}


vtkLeapScaleActor::~vtkLeapScaleActor()
{
    //this->RightAxis->Delete();
    //this->TopAxis->Delete();
    this->YAxisLegend->Delete();
    this->XAxisLegend->Delete();
    this->ZAxisLegend->Delete();

    this->Legend->Delete();
    this->LegendPoints->Delete();
    this->LegendMapper->Delete();
    this->LegendActor->Delete();

    for (int i=0; i<6; i++)
    {
        this->LabelMappers[i]->Delete();
        this->LabelActors[i]->Delete();
    }
    this->LegendTitleProperty->Delete();
    this->LegendLabelProperty->Delete();
    this->Coordinate->Delete();
}


void vtkLeapScaleActor::BuildRepresentation(vtkViewport *viewport)
{
    if ( 1 ) //it's probably best just to rerender every time
        // if ( this->GetMTime() > this->BuildTime ||
        // (this->Renderer && this->Renderer->GetVTKWindow() &&
        // this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime) )
    {
        // Specify the locations of the axes.
        int *size = viewport->GetSize();        


        this->YAxisLegend->GetPositionCoordinate()->
                SetValue(this->LeftBorderOffset,
                         size[1]-this->CornerOffsetFactor*this->TopBorderOffset,
                            0.0);
        this->YAxisLegend->GetPosition2Coordinate()->
                SetValue(this->LeftBorderOffset,
                         this->CornerOffsetFactor*this->BottomBorderOffset + 20,
                            0.0);

        this->ZAxisLegend->GetPositionCoordinate()->
                SetValue(this->LeftBorderOffset + 80,
                         this->CornerOffsetFactor*this->BottomBorderOffset + 80,
                         +10.0);
        this->ZAxisLegend->GetPosition2Coordinate()->
                SetValue(this->LeftBorderOffset + 20,
                         this->CornerOffsetFactor*this->BottomBorderOffset + 5,
                         0.0);

        if ( this->LegendVisibility )
        {
            this->XAxisLegend->GetPositionCoordinate()->
                    SetValue(this->CornerOffsetFactor*this->LeftBorderOffset,
                             2*this->BottomBorderOffset,0.0);
            this->XAxisLegend->GetPosition2Coordinate()->
                    SetValue(size[0]-this->CornerOffsetFactor*this->RightBorderOffset,
                    2*this->BottomBorderOffset,0.0);
        }
        else
        {
            this->XAxisLegend->GetPositionCoordinate()->
                    SetValue(this->CornerOffsetFactor*this->LeftBorderOffset,
                             this->BottomBorderOffset,0.0);
            this->XAxisLegend->GetPosition2Coordinate()->
                    SetValue(size[0]-this->CornerOffsetFactor*this->RightBorderOffset,
                    this->BottomBorderOffset,0.0);
        }

        // Now specify the axis values
        if ( this->LabelMode == XY_COORDINATES )
        {

            double *xL = this->YAxisLegend->GetPositionCoordinate()->
                    GetComputedWorldValue(viewport);
            double *xR = this->YAxisLegend->GetPosition2Coordinate()->
                    GetComputedWorldValue(viewport);
            this->YAxisLegend->SetRange(xL[1],xR[1]);

            xL = this->XAxisLegend->GetPositionCoordinate()->
                    GetComputedWorldValue(viewport);
            xR = this->XAxisLegend->GetPosition2Coordinate()->
                    GetComputedWorldValue(viewport);
            this->XAxisLegend->SetRange(xL[0],xR[0]);

            xL = this->ZAxisLegend->GetPositionCoordinate()->
                    GetComputedWorldValue(viewport);
            xR = this->ZAxisLegend->GetPosition2Coordinate()->
                    GetComputedWorldValue(viewport);
            this->ZAxisLegend->SetRange(xL[0],xR[0]);
        }

        else //distance between points
        {
            double d;

            double *xL = this->YAxisLegend->GetPositionCoordinate()->
                    GetComputedWorldValue(viewport);
            double *xR = this->YAxisLegend->GetPosition2Coordinate()->
                    GetComputedWorldValue(viewport);
            d = sqrt (vtkMath::Distance2BetweenPoints(xL,xR));
            this->YAxisLegend->SetRange(d/2.0,-d/2.0);

            xL = this->XAxisLegend->GetPositionCoordinate()->
                    GetComputedWorldValue(viewport);
            xR = this->XAxisLegend->GetPosition2Coordinate()->
                    GetComputedWorldValue(viewport);
            d = sqrt (vtkMath::Distance2BetweenPoints(xL,xR));
            this->XAxisLegend->SetRange(-d/2.0,d/2.0);

            xL = this->ZAxisLegend->GetPositionCoordinate()->
                    GetComputedWorldValue(viewport);
            xR = this->ZAxisLegend->GetPosition2Coordinate()->
                    GetComputedWorldValue(viewport);
            d = sqrt (vtkMath::Distance2BetweenPoints(xL,xR));
            this->ZAxisLegend->SetRange(-d/2.0,d/2.0);


        }

        if ( this->LegendVisibility )
        {
            // Update the position
            double x1 = 0.33333*size[0];
            double delX = x1/4.0;

            this->LegendPoints->SetPoint(0, x1,10,0);
            this->LegendPoints->SetPoint(1, x1+delX,10,0);
            this->LegendPoints->SetPoint(2, x1+2*delX,10,0);
            this->LegendPoints->SetPoint(3, x1+3*delX,10,0);
            this->LegendPoints->SetPoint(4, x1+4*delX,10,0);
            this->LegendPoints->SetPoint(5, x1,20,0);
            this->LegendPoints->SetPoint(6, x1+delX,20,0);
            this->LegendPoints->SetPoint(7, x1+2*delX,20,0);
            this->LegendPoints->SetPoint(8, x1+3*delX,20,0);
            this->LegendPoints->SetPoint(9, x1+4*delX,20,0);

            // Specify the position of the legend title
            this->LabelActors[5]->SetPosition(0.5*size[0],22);
            this->Coordinate->SetValue(0.33333*size[0],15,0.0);
            double *x = this->Coordinate->GetComputedWorldValue(viewport);
            double xL[3]; xL[0]=x[0];xL[1]=x[1];xL[2]=x[2];
            this->Coordinate->SetValue(0.66667*size[0],15,0.0);
            x = this->Coordinate->GetComputedWorldValue(viewport);
            double xR[3]; xR[0]=x[0];xR[1]=x[1];xR[2]=x[2];
            double len = sqrt(vtkMath::Distance2BetweenPoints(xL,xR));
            char buf[256];
            sprintf(buf,"Scale 1 : %g",len);
            this->LabelMappers[5]->SetInput(buf);

            // Now specify the position of the legend labels
            x = this->LegendPoints->GetPoint(0);
            this->LabelActors[0]->SetPosition(x[0],x[1]-1);
            x = this->LegendPoints->GetPoint(1);
            this->LabelActors[1]->SetPosition(x[0],x[1]-1);
            x = this->LegendPoints->GetPoint(2);
            this->LabelActors[2]->SetPosition(x[0],x[1]-1);
            x = this->LegendPoints->GetPoint(3);
            this->LabelActors[3]->SetPosition(x[0],x[1]-1);
            x = this->LegendPoints->GetPoint(4);
            this->LabelActors[4]->SetPosition(x[0],x[1]-1);
        }

        this->BuildTime.Modified();

        viewport->AddActor2D(YAxisLegend);  //LEFT
        viewport->AddActor2D(XAxisLegend);  //BOTTOM
        viewport->AddActor2D(ZAxisLegend);  //FRONT
    }

}

void vtkLeapScaleActor::PrintSelf(std::ostream &os)
{
    vtkIndent *indent = vtkIndent::New();

      os << indent << "Label Mode: ";
      if ( this->LabelMode == DISTANCE )
        {
        os << "Distance\n";
        }
      else //if ( this->LabelMode == DISTANCE )
        {
        os << "XY_Coordinates\n";
        }

      os << indent << "Legend Visibility: "
         << (this->LegendVisibility ? "On\n" : "Off\n");
      os << indent << "Corner Offset Factor: " << this->CornerOffsetFactor << "\n";

      os << indent << "Right Border Offset: " << this->RightBorderOffset << "\n";
      os << indent << "Top Border Offset: " << this->TopBorderOffset << "\n";
      os << indent << "Left Border Offset: " << this->LeftBorderOffset << "\n";
      os << indent << "Bottom Border Offset: " << this->BottomBorderOffset << "\n";

      os << indent << "Legend Title Property: ";
      if ( this->LegendTitleProperty )
        {
        os << this->LegendTitleProperty << "\n";
        }
      else
        {
        os << "(none)\n";
        }
      os << indent << "Legend Label Property: ";
      if ( this->LegendLabelProperty )
        {
        os << this->LegendLabelProperty << "\n";
        }
      else
        {
        os << "(none)\n";
        }

      os << indent << "Left Axis: ";
      if ( this->YAxisLegend )
        {
        os << this->YAxisLegend << "\n";
        }
      else
        {
        os << "(none)\n";
        }
      os << indent << "Bottom Axis: ";
      if ( this->XAxisLegend )
        {
        os << this->XAxisLegend << "\n";
        }
      else
        {
        os << "(none)\n";
        }
}
