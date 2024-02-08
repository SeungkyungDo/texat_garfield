#include <iostream>
#include <fstream>
#include <cstdlib>

#include <TCanvas.h>
#include <TROOT.h>
#include <TMath.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include "TF2.h"
#include "TH2D.h"
#include <TApplication.h>

#include "Garfield/ViewCell.hh"
#include "Garfield/ViewDrift.hh"
#include "Garfield/ViewSignal.hh"
#include "Garfield/ViewField.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/DriftLineRKF.hh"
#include "Garfield/TrackHeed.hh"
#include "Garfield/TrackSrim.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/AvalancheMC.hh"

#include "WireSet.h"

using namespace Garfield;
using namespace std;

int main(int argc, char * argv[])
{
    TString tag = "0208";

    TexatConfiguration conf;

    bool saveFigures = false;
    int dxCvs = 1200;
    int dyCvs = 800;
    //double vRange1 = -400;//conf.fWireGG1.v;
    //double vRange2 = -50;//conf.fWireGG2.v;
    double vRange1 = conf.fWireGG1.v-50;
    double vRange2 = conf.fWireGG2.v+10;
    double x1 = -11;
    double x2 = 11;
    double y1 = -12;
    double y2 = 1;
    double z1 = -15;
    double z2 = 5;
    //double zoomx1 = x1;
    //double zoomx2 = x2;
    //double zoomy1 = y1;
    //double zoomy2 = y2;
    double zoomx1 = -6;
    double zoomx2 = 2;
    double zoomy1 = conf.fWireGG2.y-0.5;
    double zoomy2 = conf.fWireGG2.y+0.5;
    double fieldValueHistSpacing = 0.01;

    TApplication app("app", &argc, argv);

    /////////////////////////////////////////////////////////////////////////
    // Setup the gas mixture and the corresponding file.
    MediumMagboltz gas("he4", 90., "co2", 10.);
    gas.SetTemperature(293.15);
    gas.SetPressure(760.*0.39);
    // This file needs to be created with the correct gas mixture and conditions.
    //gas.LoadGasFile("he4_co2_p39_r10_Drift.gas");

    // Setup the component for the TexAT_v2 structure.
    // Assuming the field configuration is already set in a file named "TexAT_v2_field.els"
    // and the geometry in "TexAT_v2_geometry.txt".
    ComponentAnalyticField cmp;
    cmp.SetMedium(&gas);
    // Load the field map.
    // cmp.LoadElectricField("TexAT_v2_field.els", "mm");

    /////////////////////////////////////////////////////////////////////////
    conf.CreateComponents(cmp);

    Sensor sensor;
    sensor.AddComponent(&cmp);
    sensor.AddElectrode(&cmp, "MMGEM");
    sensor.SetArea(x1,y1,z1,x2,y2,z2);

    ViewField fieldView;
    fieldView.SetComponent(&cmp);
    fieldView.SetPlane(0, 0, 1, 0, 0, 0);
    fieldView.SetArea(x1,y1,z1,x2,y2,z2);
    fieldView.SetVoltageRange(vRange1,vRange2);

    //////////////////////////////////////////////////////////////////////////
    TString name0 = "field_contour";
    auto cvs = new TCanvas(name0, "", dxCvs, dyCvs);
    cvs -> SetLeftMargin(0.16);
    fieldView.SetCanvas(cvs);
    fieldView.PlotContour();
    conf.DrawGraph("p same");
    auto f2 = (TF2*) cvs -> GetListOfPrimitives() -> FindObject("f2D_0");
    auto hist = f2 -> GetHistogram();
    hist -> GetXaxis() -> SetRangeUser(zoomx1,zoomx2);
    hist -> GetYaxis() -> SetRangeUser(zoomy1,zoomy2);
    if (saveFigures) cvs -> SaveAs(Form("figure_%s%s.png", name0.Data(),tag.Data()));
    if (saveFigures) cvs -> SaveAs(Form("figure_%s%s.root",name0.Data(),tag.Data()));

    //////////////////////////////////////////////////////////////////////////
    name0 = "field_value";
    cvs = new TCanvas(name0, "", dxCvs, dyCvs);
    int nTest = 100;
    double testRange = 0.25;
    auto frame = new TH2D("hist_v",";Offset dx from wire center [cm];Field value (V)",nTest,0,testRange,100,vRange1,vRange2);
    frame -> SetStats(0);
    frame -> Draw();
    conf.CreateAndDrawFieldValueGraph(f2);
    if (saveFigures) cvs -> SaveAs(Form("figure_%s%s.png", name0.Data(),tag.Data()));
    if (saveFigures) cvs -> SaveAs(Form("figure_%s%s.root",name0.Data(),tag.Data()));

    //////////////////////////////////////////////////////////////////////////
    name0 = "field_voltage2D";
    cvs = new TCanvas(name0, "", dxCvs, dyCvs);
    int nx = (zoomx2-zoomx1)/fieldValueHistSpacing;
    int ny = (zoomy2-zoomy1)/fieldValueHistSpacing;
    cout << "nxy: " << nx << ", " << ny << endl;
    cvs -> SetMargin(0.12,0.16,0.12,0.06);
    auto histVoltage2D = new TH2D("histVoltage2D",";x [cm];y [cm]; Field value",nx,zoomx1,zoomx2,ny,zoomy1,zoomy2);
    histVoltage2D -> SetStats(0);
    histVoltage2D -> GetZaxis() -> SetTitleOffset(1.25);
    histVoltage2D -> SetMinimum(vRange1);
    histVoltage2D -> SetMaximum(vRange2);
    for (double xBin=zoomx1; xBin<zoomx2; xBin+=fieldValueHistSpacing)
        for (double yBin=zoomy1; yBin<zoomy2; yBin+=fieldValueHistSpacing)
            histVoltage2D -> Fill(xBin,yBin,f2->Eval(xBin,yBin));
    histVoltage2D -> Draw("colz");
    conf.DrawGraph("p same");
    if (saveFigures) cvs -> SaveAs(Form("figure_%s_%s.png", name0.Data(),tag.Data()));
    if (saveFigures) cvs -> SaveAs(Form("figure_%s_%s.root",name0.Data(),tag.Data()));

    app.Run(kTRUE);
    return 0;
}
