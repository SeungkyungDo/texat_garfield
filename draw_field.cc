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
    bool zoom_option1 = true;
    bool zoom_option2 = false;

    bool addGG1 = true; // 0.201 -260
    bool addGG2 = true; // 0.201 -230
    bool addFC0 = true; // 11.265 -1600.
    bool addGEM = true; // 0 230

    int dxCvs = 1600;
    int dyCvs = 1200;

    double vRange1 = -1600.;
    double vRange2 = 300.;
    double x1 = -11;
    double x2 = 11;
    double y1 = -12;
    double y2 = 1;
    double z1 = -15;
    double z2 = 5;
    double zoomx1 = x1;
    double zoomx2 = x2;
    double zoomy1 = y1;
    double zoomy2 = y2;
    double fieldValueHistSpacing = 0.01;

    TString nameSet;
    int n, mstyle, mcolor, lcolor;
    double x0, pitch, v, y, r;
    int n1 = 101;
    int n2 = 8;
    WireSet wireFC0(nameSet="FC0", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-1600, y=-11.265, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
    WireSet wireGG1(nameSet="GG1", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-260,  y=-0.201,  r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
    WireSet wireGG2(nameSet="GG2", n=n2, x0=-(n2/2-0.5)*0.5, pitch=0.5, v=-230,  y=-0.201,  r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed  );

    //const double vMMGEM = 230.;
    const double vMMGEM = 0.;

    if (zoom_option1) {
        vRange1 = -300;
        vRange2 = 0;
        y1 = -1;
        y2 = 0.1;
        zoomx1 = -5;
        zoomx2 = 0;
        zoomy1 = y1;
        zoomy2 = y2;
    }
    else if (zoom_option2) {
        vRange1 = -300;
        vRange2 = 0;
        y1 = -0.5;
        y2 = 0.1;
        zoomx1 = 6;
        zoomx2 = 9;
        zoomy1 = y1;
        zoomy2 = y2;
    }

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
    if (addFC0) wireFC0.CreateWires(cmp);
    if (addGG1) wireGG1.CreateWires(cmp);
    if (addGG2) wireGG2.CreateWires(cmp);
    if (addGEM) cmp.AddPlaneY(0,vMMGEM,"MMGEM");

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
    TCanvas *cvs = nullptr;
    TString name0 = "";
    TString nameTag = "";
    if (zoom_option1) nameTag = nameTag + "_zoom1";
    else if (zoom_option2) nameTag = nameTag + "_zoom2";
    if (addGEM && addGG1 && addGG2 && addFC0) ;
    else {
        if (addGEM) nameTag = nameTag + "_GEM";
        if (addGG1) nameTag = nameTag + "_GG1";
        if (addGG2) nameTag = nameTag + "_GG2";
        if (addFC0) nameTag = nameTag + "_FC0";
    }

    //////////////////////////////////////////////////////////////////////////
    name0 = "field_contour";
    cvs = new TCanvas(name0, "", dxCvs, dyCvs);
    cvs -> SetLeftMargin(0.16);
    fieldView.SetCanvas(cvs);
    fieldView.PlotContour();
    if (addGG1) wireGG1.graph -> Draw("P SAME");
    if (addGG2) wireGG2.graph -> Draw("P SAME");
    if (addFC0) wireFC0.graph -> Draw("P SAME");
    auto f2 = (TF2*) cvs -> GetListOfPrimitives() -> FindObject("f2D_0");
    auto hist = f2 -> GetHistogram();
    hist -> GetXaxis() -> SetRangeUser(zoomx1,zoomx2);
    hist -> GetYaxis() -> SetRangeUser(zoomy1,zoomy2);
    cvs -> SaveAs(Form("figure_%s%s.png", name0.Data(),nameTag.Data()));
    cvs -> SaveAs(Form("figure_%s%s.root",name0.Data(),nameTag.Data()));

    //////////////////////////////////////////////////////////////////////////
    name0 = "field_value";
    cvs = new TCanvas(name0, "", dxCvs, dyCvs);
    wireGG1.CreateAndDrawFieldValueGraph(f2,true);
    wireGG2.CreateAndDrawFieldValueGraph(f2,false);
    cvs -> SaveAs(Form("figure_%s%s.png", name0.Data(),nameTag.Data()));
    cvs -> SaveAs(Form("figure_%s%s.root",name0.Data(),nameTag.Data()));

    //////////////////////////////////////////////////////////////////////////
    name0 = "field_voltage2D";
    cvs = new TCanvas(name0, "", dxCvs, dyCvs);
    int nx = (zoomx2-zoomx1)/fieldValueHistSpacing;
    int ny = (zoomy2-zoomy1)/fieldValueHistSpacing;
    cout << "nxy: " << nx << ", " << ny << endl;
    cvs -> SetMargin(0.12,0.16,0.12,0.06);
    //cvs -> SetGrid(0,0);
    auto histVoltage2D = new TH2D("histVoltage2D",";x [cm];y [cm]; Field value",nx,zoomx1,zoomx2,ny,zoomy1,zoomy2);
    histVoltage2D -> SetStats(0);
    histVoltage2D -> GetZaxis() -> SetTitleOffset(1.25);
    histVoltage2D -> SetMinimum(vRange1);
    histVoltage2D -> SetMaximum(vRange2);
    for (double xBin=zoomx1; xBin<zoomx2; xBin+=fieldValueHistSpacing)
        for (double yBin=zoomy1; yBin<zoomy2; yBin+=fieldValueHistSpacing)
            histVoltage2D -> Fill(xBin,yBin,f2->Eval(xBin,yBin));
    histVoltage2D -> Draw("colz");
    if (addGG1) wireGG1.graph -> Draw("P SAME");
    if (addGG2) wireGG2.graph -> Draw("P SAME");
    if (addFC0) wireFC0.graph -> Draw("P SAME");
    cvs -> SaveAs(Form("figure_%s%s.png", name0.Data(),nameTag.Data()));
    cvs -> SaveAs(Form("figure_%s%s.root",name0.Data(),nameTag.Data()));

    app.Run(kTRUE);
    return 0;
}
