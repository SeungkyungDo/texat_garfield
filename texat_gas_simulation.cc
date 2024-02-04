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

using namespace Garfield;
using namespace std;

class WireSet
{
    public:
        TString name;
        int n=0, mstyle=20, mcolor=kBlack, lcolor=kBlack;
        double x0=0, pitch=0, v=0, y=0, r=0;
        TGraph* graph = nullptr;

        //////////////////////////////////////////////////////////////////////////////
        WireSet(TString name_, int n_, double x0_, double pitch_, double v_, double y_, double r_, int mstyle_=20, int mcolor_=kBlack, int lcolor_=kBlack)
        : name(name_), n(n_), x0(x0_), pitch(pitch_), v(v_), y(y_), r(r_), mstyle(mstyle_), mcolor(mcolor_), lcolor(lcolor_) {
            graph = new TGraph();
            graph -> SetMarkerStyle(mstyle);
            graph -> SetMarkerColor(mcolor);
        }

        ~WireSet() {}

        //////////////////////////////////////////////////////////////////////////////
        void CreateWires(ComponentAnalyticField &cmp) {
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                cmp.AddWire(x, y, 2*r, v, name.Data());
                graph -> SetPoint(i,x,y);
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraph(TF2 *f2, bool drawFrame, int nTest=100, double testRange=0.25) {
            if (drawFrame) {
                auto frame = new TH2D(name+"_v",";Offset dx from wire center [cm];Field value (V)",nTest,0,testRange,100,(v>0? -1.5*v:1.5*v),(v>0?1.5*v:-1.5*v));
                frame -> SetStats(0);
                frame -> Draw();
            }
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                auto graph1 = new TGraph();
                graph1 -> SetLineColor(lcolor);
                for (double dx=0.; dx<testRange; dx+=testRange/nTest)
                    graph1->SetPoint(graph1->GetN(),dx,f2->Eval(x+dx,y));
                graph1 -> Draw("samel");
            }
        }
};

int main(int argc, char * argv[])
{
    if (argc<3) {
        cout << "== Input should be given" << endl;
        cout << "   1) number of electrons in each point," << endl;
        cout << "   2) spacing between electron simulation starting point," << endl;
        cout << "   3) x-range 1," << endl;
        cout << "   4) x-range 2." << endl;
        cout << "   ex) ./texat_gas_simulation  10  0.5  0  20" << endl;
        return 0;
    }

    int numElSim    = std::atoi(argv[1]);
    double xSpacing = std::atof(argv[2]);
    double xMin = 0.;
    double xMax = 20.;
    if (argc>4) {
        xMin = std::atof(argv[3]);
        xMax = std::atof(argv[4]);
    }
    const int numPoints = int((xMax - xMin) / xSpacing);
    cout << numElSim << " " << xSpacing << " " << xMin << " " << xMax << " " << numPoints << endl;

    bool addGG1 = true; // 0.201 -260
    bool addGG2 = true; // 0.201 -230
    bool addFC0 = true; // 11.265 -1600.
    bool addGEM = true; // 0 230

    int dxCvs = 1600;
    int dyCvs = 1200;

    double vRange1 = -1600.;
    double vRange2 = 300.;
    double x1 = 0;
    double x2 = 20;
    double y1 = -12;
    double y2 = 1;
    double z1 = -15;
    double z2 = 5;
    double fieldValueHistSpacing = 0.01;

    double y0Sim = -5.;
    double z0Sim = 0.;

    TString nameSet;
    int n, mstyle, mcolor, lcolor;
    double x0, pitch, v, y, r;
    WireSet wireFC0(nameSet="FC0", n=41, x0=0,    pitch=0.5, v=-1600, y=-11.265, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
    WireSet wireGG1(nameSet="GG1", n=41, x0=0.00, pitch=0.5, v=-260,  y=-0.201,  r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
    WireSet wireGG2(nameSet="GG2", n= 8, x0=7.75, pitch=0.5, v=-230,  y=-0.201,  r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed  );

    const double vMMGEM = 230.;

    TApplication app("app", &argc, argv);

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

    //////////////////////////////////////////////////////////////////////////
    TCanvas* can = new TCanvas("can","",1600,1200);
    ViewCell cellView;
    cellView.SetCanvas(can);
    cellView.SetComponent(&cmp);
    cellView.SetPlane(0,0,1,0,0,0);
    cellView.SetArea(0,-12,20,1);
    ViewDrift driftView;
    driftView.SetPlane(0,0,1,0,0,0);
    driftView.SetArea(0,-12,20,1);
    driftView.SetCanvas(can);
    driftView.Plot2d(true,true);
    cellView.Plot2d();

    /////////////////////////////////////////////////////////////////////////
    AvalancheMicroscopic drift;
    drift.SetSensor(&sensor);
    drift.SetCollisionSteps(400); // The number of steps for the simulation.
    drift.EnablePlotting(&driftView);
    // Setup the transfer function if there's a file available.
    // Assuming the transfer function is named "TexAT_v2_transfer_function.txt".
    // readTransferFunction(sensor); // Implement this function or load the transfer function file.

    ///////////////////////////////////////////////////////////////////////Added by S. Bae 240124
    double xStart, yStart, zStart, tStart, eStart;   
    double xEnd, yEnd, zEnd, tEnd, eEnd;				
    TFile* fnew = new TFile(Form("output_%d_%d_%.1f_%.1f.root",numElSim,numPoints,xMin,xMax),"RECREATE");
    TTree* tree = new TTree("gg","");
    tree -> Branch("xStart",&xStart,"xStart/D");
    tree -> Branch("yStart",&yStart,"yStart/D");
    tree -> Branch("zStart",&zStart,"zStart/D");
    tree -> Branch("tStart",&tStart,"tStart/D");
    tree -> Branch("xEnd",&xEnd,"xEnd/D");
    tree -> Branch("yEnd",&yEnd,"yEnd/D");
    tree -> Branch("zEnd",&zEnd,"zEnd/D");
    tree -> Branch("tEnd",&tEnd,"tEnd/D");

    //////////////////////////////////////////////////////////////////////
    for (int i = 0; i < numElSim; ++i)
    {
        for (double x0 = xMin; x0 < xMax; x0+=xSpacing)
        {
            int nElectronsCollected = 0;
            double t0 = 0., e0 = 0., dx0=0., dy0=0., dz0=0.;
            drift.AvalancheElectron(x0, y0Sim, z0Sim, t0, e0, dx0, dy0, dz0);

            int numAvElectrons, numAvIons;
            drift.GetAvalancheSize(numAvElectrons,numAvIons);
            int numAvalanche = drift.GetNumberOfElectronEndpoints();

            cout << " == " << i << "/" << numElSim << " e/i=" << numAvElectrons << "/" << numAvIons << " (" << Form("%.1f",x0) << "/" << xMax << ") : #AEl = " << numAvalanche << endl;
            //cout << i << "/" << numElSim << " (" << x0 << "/" << xMax << ") : #AEl = " << numAvalanche << endl;
            for (int iAvalanche = 0; iAvalanche < numAvalanche; ++iAvalanche) {
                int status;

                drift.GetElectronEndpoint(iAvalanche, xStart, yStart, zStart, tStart, eStart, xEnd, yEnd, zEnd, tEnd, eEnd, status);

                if (std::abs(yEnd) < 0.1)
                    nElectronsCollected++;

                tree -> Fill();
            }
        }
    }

    driftView.Plot(true, false);
    can -> SaveAs("figure_drift_electron.png");

    fnew -> WriteTObject(tree);	//Added by S. Bae 240124
    fnew -> Print();			//Added by S. Bae 240124
    fnew -> Close();			//Added by S. Bae 240124

    app.Run(kTRUE);
    return 0;
}
