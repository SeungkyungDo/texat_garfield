#include <iostream>
#include <fstream>
#include <cstdlib>

#include <TCanvas.h>
#include <TROOT.h>
#include <TMath.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>

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
    const int nElectrons = 1;
    const double xSpacing = 0.5;
    const double xMin = 7.;
    const double xMax = 12.;

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

    // Wire radius [cm]
    const double rWire = 0.005;
    const double rWireFC = 0.005;
    const double Wirepitch = 0.5;
    const double WireFCpitch = 0.5;
    const double GGGap = 0.201;
    const double FCGap = 11.265;

    const double vMMGEM = 230.;
    const double vGG_Black = -260.;
    const double vGG_Blue = -230.;
    const double vFC = -1600.;


    // Add the wires
    const int imax = 8;
    //const int ifcmax = 15;
    const int ifcmax = 41;
    for(int i=0; i<ifcmax; i++){
        cmp.AddWire(i*WireFCpitch,-1*0.201, 2*rWireFC, vGG_Black, "GG_black");
        cmp.AddWire(i*WireFCpitch,-1*11.265, 2*rWireFC, vFC, "FC");
    }

    for(int i=0; i<imax; i++){
        //cmp.AddWire(1.75+i*Wirepitch,-1*0.201, 2*rWire, vGG_Blue, "GG_blue");
        cmp.AddWire(7.75+i*Wirepitch,-1*0.201, 2*rWire, vGG_Blue, "GG_blue");
    }
    cmp.AddPlaneY(0,vMMGEM,"MMGEM");

    // Create a sensor.
    Sensor sensor;
    sensor.AddComponent(&cmp);
    sensor.AddElectrode(&cmp, "MMGEM");
    sensor.SetArea(-5,-15,-15,25,2,5);
    // Set the bounding box if necessary.
    // cmp.SetBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);

    // 전선 위치 데이터 설정
    const int nWires = ifcmax + imax; // 전선의 총 개수
    double wireX[nWires], wireY[nWires];

    TGraph* graphBlack = new TGraph(ifcmax); // GG_black과 FC 전선 개수만큼
    for (int i=0; i < ifcmax; ++i) {
        graphBlack->SetPoint(i, i*WireFCpitch, -1*0.201); // GG_black 위치 설정
    }

    graphBlack->SetMarkerStyle(24);
    graphBlack->SetMarkerColor(kBlack);


    TGraph* graphFC = new TGraph(ifcmax); // GG_black과 FC 전선 개수만큼
    for (int i=0; i < ifcmax; ++i) {
        graphFC->SetPoint(i, i*WireFCpitch, -1*11.265); // FC 위치 설정
    }

    graphFC->SetMarkerStyle(24);
    graphFC->SetMarkerColor(kBlack);


    TGraph* graphBlue = new TGraph(imax); // GG_blue 전선 개수만큼
    for (int i=0; i < imax; ++i) {
        graphBlue->SetPoint(i, 7.75+i*Wirepitch, -1*0.201); // GG_blue 위치 설정
    }

    graphBlue->SetMarkerStyle(25);
    //graphBlue->SetMarkerColor(kBlue);

    ViewField fieldView;
    fieldView.SetComponent(&cmp);
    fieldView.SetPlane(0, 0, 1, 0, 0, 0);
    //fieldView.SetArea(-5, -15, -15, 25, 2, 5);
    //fieldView.SetVoltageRange(-1600.,200.);
    //fieldView.SetVoltageRange(-300.,200.);

    //fieldView.SetArea(5, -0.6, -15, 15, -0.1, 5);

    if (1) {
        fieldView.SetArea(0, -1, -15, 20, 0.1, 5);
        fieldView.SetVoltageRange(-300.,0.);
    }

    TCanvas* can2 = new TCanvas("can2", "", 2600, 1600);
    can2->SetLeftMargin(0.16);
    fieldView.SetCanvas(can2);
    fieldView.PlotContour();
    graphBlack->Draw("P SAME");
    //graphFC->Draw("P SAME");
    graphBlue->Draw("P SAME"); // "AP"는 축(Axis)과 점(Points)을 의미함

    can2 -> SaveAs("figure_field.png");
    can2 -> SaveAs("figure_field.root");
    app.Run(kTRUE);
    return 0;

    ///////////////////////////////////////////////////////////////////////// Drawing cell and drift. moved and modfied by S. Bae 240124
    TCanvas* can = new TCanvas("can","",1600,1600);
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

    graphBlack->Draw("P SAME");
    graphFC->Draw("P SAME");
    graphBlue->Draw("P SAME"); // "AP"는 축(Axis)과 점(Points)을 의미함
    //app.Run(kTRUE);
    //return true;

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
    TFile* fnew =new TFile("output.root","RECREATE");
    TTree* tr = new TTree("gg","");
    tr->Branch("xStart",&xStart,"xStart/D");
    tr->Branch("yStart",&yStart,"yStart/D");
    tr->Branch("zStart",&zStart,"zStart/D");
    tr->Branch("tStart",&tStart,"tStart/D");
    tr->Branch("xEnd",&xEnd,"xEnd/D");
    tr->Branch("yEnd",&yEnd,"yEnd/D");
    tr->Branch("zEnd",&zEnd,"zEnd/D");
    tr->Branch("tEnd",&tEnd,"tEnd/D");
    //////////////////////////////////////////////////////////////////////
    for (int i = 0; i < nElectrons; ++i)
    {
        for (double x0 = xMin; x0 < xMax; x0+=xSpacing)
        {
            //double x0 = floor(1.0*i/nset)*0.1+7.65, y0 = -5., z0 = 0.; // Initial position of the electron. //x0 midified by S. Bae 240129
            double y0 = -5.;
            double z0 = 0.;
            int nElectronsCollected = 0;
            // double sumDriftTime = 0.;

            double t0 = 0.; // Initial time.
            double e0 = 0.;
            double dx0=0., dy0=0., dz0=0.;

            drift.AvalancheElectron(x0, y0, z0, t0, e0, dx0, dy0, dz0);
            // Drift an electron from FC1 towards MM+GEM.


            // Get the Avalanche size (It corresponding to the number or electrons)
            int ne, ni; // Get the number of electrons and ions in the avalanche
            drift.GetAvalancheSize(ne,ni);

            int np = drift.GetNumberOfElectronEndpoints();

            //std::cout<<i<< " ne: " << ne << " ni: " << ni <<std::endl;
            //cout<<i<< " ne: " << ne << " ni: " << ni <<endl;	//commented by S. Bae 240124


            constexpr bool twod = true;
            constexpr bool drawaxis = false;	//changed to flase S. Bae 240124

            cout << i << "/" << nElectrons << " (" << x0 << "/" << xMax << ") : #AEl = " << np << endl;
            for (int n = 0; n < np; ++n) {
                //  double xStart, yStart, zStart, tStart, eStart;   //commented by S. Bae 240124
                //  double xEnd, yEnd, zEnd, tEnd, eEnd;				//commented by S. Bae 240124
                int status;

                drift.GetElectronEndpoint(n, xStart, yStart, zStart, tStart, eStart, xEnd, yEnd, zEnd, tEnd, eEnd, status);
                // drift.GetElectronEndpoint(i, x0, y0, z0, t0, e0, dx0, dy0, dz0, xEnd, yEnd, zEnd, tEnd, eEnd, dxEnd, dyEnd, dzEnd, status);

                if (std::abs(yEnd) < 0.1) {
                    nElectronsCollected++;
                }

                tr->Fill();
                //driftView.Plot(twod, drawaxis);				//Added by S. Bae 240124
                                                            //std::cout<< status << " start: " << xStart << " " << yStart << " " << zStart << " end: " << xEnd << yEnd << zEnd <<std::endl;	//commented by S. Bae
                                                            //cout<< status <<  " start: " << xStart << " " << yStart << " " << zStart << " end: " << xEnd << yEnd << zEnd <<endl;	//commented by S. Bae
            }
        }
    }

    driftView.Plot(true, false);
    can -> SaveAs("figure_drift_electon.png");

    fnew->WriteTObject(tr);	//Added by S. Bae 240124
    fnew->Print();			//Added by S. Bae 240124
    fnew->Close();			//Added by S. Bae 240124

    app.Run(kTRUE);
    return 0;
}
