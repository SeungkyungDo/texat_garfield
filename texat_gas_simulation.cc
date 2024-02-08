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
    TApplication app("app", &argc, argv);

    /////////////////////////////////////////////////////////////////////////
    bool showFigures = false;
    bool saveFigures = true;
    double ySim = -5.;
    double zSim = 0.;

    /////////////////////////////////////////////////////////////////////////
    int    numElectrons = 100;
    int    numSimPoints = 100;
    double xMinSim = -10.;
    double xMaxSim = +10.;
    if (argc<3) {
        cout << "== Input should be given" << endl;
        cout << "   0) simulation index (full simulation up to (numSimPoints - 1)" << endl;
        cout << "   1) (optional, default=" << numElectrons << ") number of electrons in each point," << endl;
        cout << "   2) (optional, default=" << numSimPoints << ") number of binning through x-range" << endl;
        cout << "   3) (optional, default=" << xMinSim      << ") x-range 1," << endl;
        cout << "   4) (optional, default=" << xMaxSim      << ") x-range 2." << endl;
        cout << "   5) (optional, default=" << showFigures  << ") show simulation figure." << endl;
        cout << "   ex)" << endl;
        cout << "      ./texat_gas_simulation 0 20 1 -2.5 2.5 1" << endl;
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////
    int simulationIndex = std::atoi(argv[1]);
    if (argc>2) numElectrons = std::atoi(argv[2]);
    if (argc>2) numSimPoints = std::atoi(argv[3]);
    if (argc>4) xMinSim = std::atof(argv[4]);
    if (argc>4) xMaxSim = std::atof(argv[5]);
    if (argc>6) showFigures = bool(std::atoi(argv[6]));
    double xSpacing = ((xMaxSim - xMinSim) / numSimPoints);
    cout << "* numElectrons=" << numElectrons
         << ", numSimPoints=" << numSimPoints
         << ", xMinSim=" << xMinSim
         << ", xMaxSim=" << xMaxSim
         << ", numSimPoints=" << numSimPoints
         << ", showFigures=" << showFigures << endl;

    TString tag = Form("%d",simulationIndex);

    /////////////////////////////////////////////////////////////////////////
    int dxCvs = 1200;
    int dyCvs = 800;
    double vRange1 = -300.;
    double vRange2 = 0.;
    double y1 = -12;
    double y2 = 1;
    double z1 = -15;
    double z2 = 5;
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
    drift.SetCollisionSteps(400); // The number of steps for the simulation.

    if (showFigures) {
        cvs = new TCanvas("cvs","",1600,1200);
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
    double xStart, yStart, zStart, tStart, eStart;   
    double xEnd, yEnd, zEnd, tEnd, eEnd;				
    TFile* fnew = new TFile(Form("output_%s.root",tag.Data()),"RECREATE");
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
    auto dxSim = (xMaxSim-xMinSim)/numSimPoints;
    auto xMin = xMinSim + dxSim * (simulationIndex);
    auto xMax = xMinSim + dxSim * (simulationIndex+1);
    auto dx = (xMax-xMin)/numElectrons;
    int numCollected = 0;
    int numAbsorbed = 0;
    for (int iElPrim = 0; iElPrim < numElectrons; ++iElPrim)
    {
        double xSim = xMin + dx*iElPrim;
        double tSim = 0, eSim = 0;
        cout << iElPrim << "/" << numElectrons << ", pos=(" << xSim << "," << ySim << "," << zSim << ")" << endl;

        /// \param x,y,z,t starting point of the electron
        /// \param e initial energy of the electron
        /// \param dx,dy,dz initial direction vector of the electron
        /// If the initial direction is not specified, it is sampled randomly.
        drift.AvalancheElectron(xSim, ySim, zSim, tSim, eSim);

        int numAvElectrons, numAvIons;
        drift.GetAvalancheSize(numAvElectrons,numAvIons);
        int numAvalanche = drift.GetNumberOfElectronEndpoints();

        int numAbsorbed0 = 0;
        int numCollected0 = 0;
        for (int iAvalanche = 0; iAvalanche < numAvalanche; ++iAvalanche)
        {
            int status;
            drift.GetElectronEndpoint(iAvalanche, xStart, yStart, zStart, tStart, eStart, xEnd, yEnd, zEnd, tEnd, eEnd, status);
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

    if (showFigures) {
        driftView.Plot(true,false);
        if (saveFigures) cvs -> SaveAs(Form("figure_gas_sim_%s.png",tag.Data()));
        if (saveFigures) cvs -> SaveAs(Form("figure_gas_sim_%s.root",tag.Data()));
    }

    fnew -> WriteTObject(tree);
    fnew -> Close();

    ofstream flog(Form("log_%s.txt",tag.Data()));
    flog << simulationIndex << endl;
    flog << numElectrons << endl;
    flog << numSimPoints << endl;
    flog << xMinSim << endl;
    flog << xMaxSim << endl;
    flog << showFigures << endl;

    if (showFigures) gApplication -> Run();
    return 0;
}
