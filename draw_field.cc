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

using namespace Garfield;
using namespace std;

int main(int argc, char * argv[])
{
    bool zoom1 = false;

    bool addGG1 = true; // 0.201 -260
    bool addGG2 = true; // 0.201 -230
    bool addFC0 = true; // 11.265 -1600.
    bool addGEM = true; // 0 230
    //double vRange1 = -1600.;
    double vRange1 = -300.;
    double vRange2 = 300.;
    double x1 = 0;
    double x2 = 20;
    double y1 = -12;
    double y2 = 1;
    double z1 = -15;
    double z2 = 5;
    double zoomx1 = x1;
    double zoomx2 = x2;
    double zoomy1 = y1;
    double zoomy2 = y2;

    const double vMMGEM = 230.;

    const int numFC0 = 41;
    const double vFC0 = -1600.;
    const double yFC0 = -11.265;
    const double offFC0 = 0;
    const double rWireFC0 = 0.005;
    const double wirePitchFC0 = 0.5;

    const int numGG1 = 41;
    const double vGG1 = -260.;
    const double yGG1 = -0.201;
    const double offGG1 = 0.00;
    const double rWireGG1 = 0.005;
    const double wirePitchGG1 = 0.5;

    const int numGG2 = 8;
    const double vGG2 = -230.;
    const double yGG2 = -0.201;
    const double offGG2 = 7.75;
    const double rWireGG2 = 0.005;
    const double wirePitchGG2 = 0.5;

    if (zoom1) {
        vRange1 = -300;
        vRange2 = 300;
        y1 = -1;
        y2 = 0.1;
        zoomx1 = 6;
        zoomx2 = 11;
        zoomy1 = y1;
        zoomy2 = y2;
    }

    TApplication app("app", &argc, argv);

    MediumMagboltz gas("he4", 90., "co2", 10.);
    gas.SetTemperature(293.15);
    gas.SetPressure(760.*0.39);

    ComponentAnalyticField cmp;
    cmp.SetMedium(&gas);

    TGraph* graphFC0 = new TGraph(numFC0);
    graphFC0 -> SetMarkerStyle(26);
    graphFC0 -> SetMarkerColor(kBlack);
    TGraph* graphGG1 = new TGraph(numGG1);
    graphGG1 -> SetMarkerStyle(24);
    graphGG1 -> SetMarkerColor(kBlack);
    TGraph* graphGG2 = new TGraph(numGG2);
    graphGG2 -> SetMarkerStyle(25);
    graphGG2 -> SetMarkerColor(kRed);

    if (addFC0) {
        for(int i=0; i<numFC0; i++) {
            auto x = offFC0+i*wirePitchFC0;
            cmp.AddWire(x,yFC0, offFC0+2*rWireFC0, vFC0, "FC0");
            graphFC0 -> SetPoint(i,x,yFC0);
        }
    }

    if (addGG1) {
        for(int i=0; i<numGG1; i++) {
            auto x = offGG1+i*wirePitchGG1;
            cmp.AddWire(x,yGG1, 2*rWireGG1, vGG1, "GG1");
            graphGG1 -> SetPoint(i,x,yGG1);
        }
    }

    if (addGG2) {
        for(int i=0; i<numGG2; i++) {
            auto x = offGG2+i*wirePitchGG2;
            cmp.AddWire(x,yGG2, 2*rWireGG2, vGG2, "GG2");
            graphGG2 -> SetPoint(i,x,yGG2);
        }
    }

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

    TCanvas* cvsField = new TCanvas("field", "", 1600, 1200);
    cvsField -> SetLeftMargin(0.16);
    fieldView.SetCanvas(cvsField);
    fieldView.PlotContour();
    graphGG1 -> Draw("P SAME");
    if (addGG2) graphGG2 -> Draw("P SAME");
    if (addFC0) graphFC0 -> Draw("P SAME");

    TString name = "field";
    if (addGEM) name = name + "_GEM";
    if (addGG1) name = name + "_GG1";
    if (addGG2) name = name + "_GG2";
    if (addFC0) name = name + "_FC0";
    TString nameRoot = TString("rcvs_") + name;
    TString nameCvs = TString("cvs_") + name;
    TString nameHist = name;

    auto f2 = (TF2*) cvsField -> GetListOfPrimitives() -> FindObject("f2D_0");
    auto hist = f2 -> GetHistogram();
    hist -> GetXaxis() -> SetRangeUser(zoomx1,zoomx2);
    hist -> GetYaxis() -> SetRangeUser(zoomy1,zoomy2);
    cvsField -> SaveAs(nameCvs+".png");
    cvsField -> SaveAs(nameRoot+".root");

    app.Run(kTRUE);
    return 0;
}
