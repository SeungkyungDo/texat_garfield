#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

#include <TCanvas.h>
#include <TROOT.h>
#include <TMath.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include "TF2.h"
#include "TH2D.h"
#include <TApplication.h>
#include <TVector3.h>

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

#include "DetectorConfiguration.h"

using namespace Garfield;
using namespace std;

int main(int argc, char * argv[])
{
    TApplication app("app", &argc, argv);

    /////////////////////////////////////////////////////////////////////////
    bool showFigures = false;
    bool saveFigures = true;
    double ySim = -5.;
    double zSim = 0.;

    /////////////////////////////////////////////////////////////////////////
    int    numElectronsPerPosition = 100;
    int    numSimPoints            = 100;
    int    numElectrons            = numSimPoints*numElectronsPerPosition;
    double xMinSim = -10.;
    double xMaxSim = +10.;
    int simulationIndex = 0;
    /*
    if (argc<3) {
        cout << "== Input should be given" << endl;
        cout << "   0) simulation index (full simulation up to (numSimPoints - 1)" << endl;
        cout << "   1) (optional, default=" << numElectrons << ") number of electrons total," << endl;
        cout << "   2) (optional, default=" << numSimPoints << ") number of binning through x-range" << endl;
        cout << "   3) (optional, default=" << xMinSim      << ") x-range 1," << endl;
        cout << "   4) (optional, default=" << xMaxSim      << ") x-range 2." << endl;
        cout << "   5) (optional, default=" << showFigures  << ") show simulation figure." << endl;
        cout << "   ex)" << endl;
        cout << "      ./texat_gas_simulation.exe 0 20 1 -2.5 2.5 1" << endl;
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////
    int simulationIndex = std::atoi(argv[1]);
    if (argc>2) numElectrons = std::atoi(argv[2]);
    if (argc>2) numSimPoints = std::atoi(argv[3]);
    if (argc>4) xMinSim = std::atof(argv[4]);
    if (argc>4) xMaxSim = std::atof(argv[5]);
    if (argc>6) showFigures = bool(std::atoi(argv[6]));
    */
    double xSpacing = ((xMaxSim - xMinSim) / numSimPoints);
    cout << "* numElectrons=" << numElectrons
         << ", numSimPoints=" << numSimPoints
         << ", xMinSim=" << xMinSim
         << ", xMaxSim=" << xMaxSim
         << ", numSimPoints=" << numSimPoints
         << ", showFigures=" << showFigures << endl;

    //TString tag = Form("%d",simulationIndex);
    TString tag = Form("%d_%d_%.2f_%.2f__%d",numElectrons,numSimPoints,xMinSim,xMaxSim,simulationIndex);

    /////////////////////////////////////////////////////////////////////////
    int dxCvs = 1200;
    int dyCvs = 800;
    double vRange1 = -300.;
    double vRange2 = 0.;
    double y1 = -12;
    double y2 = 1;
    double z1 = -5;
    double z2 = 90;
    double fieldValueHistSpacing = 0.01;

    TexatConfiguration conf;
    double x1 = xMinSim - 0.1*(xMaxSim-xMinSim); //conf.xMin;
    double x2 = xMaxSim + 0.1*(xMaxSim-xMinSim); //conf.xMax;

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

    //////////////////////////////////////////////////////////////////////////
    TCanvas* cvs = nullptr;
    ViewDrift driftView;
    ViewCell cellView;

    /////////////////////////////////////////////////////////////////////////
    AvalancheMicroscopic drift;
    drift.SetSensor(&sensor);
    drift.SetCollisionSteps(500); // The number of steps for the simulation.

    if (showFigures) {
        cvs = new TCanvas("cvs","",1000,700);
        driftView.SetCanvas(cvs);
        driftView.SetPlane(0,0,1,0,0,0);
        driftView.SetArea(x1,y1,x2,y2);
        driftView.Plot2d(true,true);

        cellView.SetCanvas(cvs);
        cellView.SetComponent(&cmp);
        cellView.SetPlane(0,0,1,0,0,0);
        cellView.SetArea(x1,y1,x2,y2);
        cellView.Plot2d();

        drift.EnablePlotting(&driftView);
    }
    // Setup the transfer function if there's a file available.
    // Assuming the transfer function is named "TexAT_v2_transfer_function.txt".
    // readTransferFunction(sensor); // Implement this function or load the transfer function file.

    ///////////////////////////////////////////////////////////////////////Added by S. Bae 240124
    double xStartVal, yStart, zStart, tStart, eStart;   
    double xEndVal, yEnd, zEnd, tEnd, eEnd;				
    TString fnewName = Form("data/output_%s.root",tag.Data());
    cout << fnewName << endl;
    TFile* fnew = new TFile(fnewName,"RECREATE");
    TTree* tree = new TTree("gg","");
    tree -> Branch("xStart",&xStartVal,"xStart/D");
    tree -> Branch("yStart",&yStart,"yStart/D");
    tree -> Branch("zStart",&zStart,"zStart/D");
    tree -> Branch("tStart",&tStart,"tStart/D");
    tree -> Branch("xEnd",&xEndVal,"xEnd/D");
    tree -> Branch("yEnd",&yEnd,"yEnd/D");
    tree -> Branch("zEnd",&zEnd,"zEnd/D");
    tree -> Branch("tEnd",&tEnd,"tEnd/D");

    //////////////////////////////////////////////////////////////////////
    auto dxSim = (xMaxSim-xMinSim)/numSimPoints;
    auto xMin = xMinSim + dxSim * (simulationIndex);
    auto xMax = xMinSim + dxSim * (simulationIndex+1);
    auto dx = (xMax-xMin)/numElectrons;
    int numCollected = 0;
    int numAbsorbed = 0;
    //int numPoints = 20;
    int numPoints = 100;
    TVector3 xStart(-0.33,-5.0,0.0);
    TVector3 xEnd(1.0,-5.0,6.80);
    TGraph* graph = new TGraph();
    for (int iSimPoint = 0; iSimPoint < numSimPoints; ++iSimPoint)
    {
        double tSim = static_cast<double>(iSimPoint)/numSimPoints;
        double xSim = xStart.x() + tSim * (xEnd.x() - xStart.x());
        double zSim = xStart.z() + tSim * (xEnd.z() - xStart.z());
        cout << iSimPoint << "/" << numSimPoints << ", pos=(" << xSim << "," << ySim << "," << zSim << ")" << endl;
        graph->SetPoint(iSimPoint,xSim,zSim);
        /// \param x,y,z,t starting point of the electron
        /// \param e initial energy of the electron
        /// \param dx,dy,dz initial direction vector of the electron
        /// If the initial direction is not specified, it is sampled randomly.

        for (int j = 0; j < numElectronsPerPosition; ++j) { // 각 위치에서 100개의 전자 발생
            double eSim = 0.0;
            drift.AvalancheElectron(xSim, ySim, zSim, tSim, eSim);

        int numAvElectrons, numAvIons;
        drift.GetAvalancheSize(numAvElectrons,numAvIons);
        int numAvalanche = drift.GetNumberOfElectronEndpoints();

        int numAbsorbed0 = 0;
        int numCollected0 = 0;
        for (int iAvalanche = 0; iAvalanche < numAvalanche; ++iAvalanche)
        {
            int status;
            drift.GetElectronEndpoint(iAvalanche, xStartVal, yStart, zStart, tStart, eStart, xEndVal, yEnd, zEnd, tEnd, eEnd, status);
            tree -> Fill();

            if (std::abs(conf.fWireGG2.y-yEnd) < conf.fWireGG2.r)
            {
                numAbsorbed0++;
                numAbsorbed++;
            }
            if (std::abs(conf.fYMM-yEnd) < 0.1)
            {
                numCollected0++;
                numCollected++;
            }
        }

        cout << "      "
             << "> #Avalanche = " << numAvalanche
             << ", #Absorbed = " << numAbsorbed << "(+" << numAbsorbed0 << ")"
             << ", #Collected = " << numCollected << "(+" << numCollected0 << ")" << endl;
        }
    }

    if (showFigures) {
        driftView.Plot(true,false);
        if (saveFigures) cvs -> SaveAs(Form("figures/figure_gas_sim_%s.png",tag.Data()));
        if (saveFigures) cvs -> SaveAs(Form("figures/figure_gas_sim_%s.root",tag.Data()));
    }

    fnew -> WriteTObject(tree);
    fnew -> Close();

    TString logFileName = Form("data/log_%s.txt",tag.Data());
    cout << logFileName << endl;
    ofstream flog(logFileName);
    flog << left << setw(18) << "simulationIndex " << simulationIndex << endl;
    flog << left << setw(18) << "numElectrons " <<    numElectrons << endl;
    flog << left << setw(18) << "numSimPoints " <<    numSimPoints << endl;
    flog << left << setw(18) << "xMinSim " <<         xMinSim << endl;
    flog << left << setw(18) << "xMaxSim " <<         xMaxSim << endl;
    flog << left << setw(18) << "showFigures " <<     showFigures << endl;
    flog << endl;
    flog << conf.Dump() << endl;

    graph -> Draw("samepl");

    if (showFigures) gApplication -> Run();
    return 0;
}
