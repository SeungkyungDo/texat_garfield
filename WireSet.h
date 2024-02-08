#ifndef WIRESET_HH
#define WIRESET_HH

#include "Garfield/ComponentAnalyticField.hh"

using namespace Garfield;
using namespace std;

#include "TString.h"
#include "TGraph.h"
#include "TF2.h"

class WireSet
{
    public:
        TString name;
        int n=0, mstyle=20, mcolor=kBlack, lcolor=kBlack;
        double x0=0, pitch=0, v=0, y=0, r=0;
        double xMin, xMax;
        TGraph* graph = nullptr;

        //////////////////////////////////////////////////////////////////////////////
        WireSet() {}
        WireSet(TString name_, int n_, double x0_, double pitch_, double v_, double y_, double r_, int mstyle_=20, int mcolor_=kBlack, int lcolor_=kBlack) {
            SetPar(name_, n_, x0_, pitch_, v_, y_, r_, mstyle_, mcolor_, lcolor_);
        }

        ~WireSet() {}

        //////////////////////////////////////////////////////////////////////////////
        void SetPar(TString name_, int n_, double x0_, double pitch_, double v_, double y_, double r_, int mstyle_=20, int mcolor_=kBlack, int lcolor_=kBlack) {
            name = name_;
            n = n_;
            x0 = x0_;
            pitch = pitch_;
            v = v_;
            y = y_;
            r = r_;
            mstyle = mstyle_;
            mcolor = mcolor_;
            lcolor = lcolor_;
            cout << "* name = " << name
                 << ", n = " << n
                 << ", x0 = " << x0
                 << ", pitch = " << pitch
                 << ", v = " << v
                 << ", y = " << y
                 << ", r = " << r << endl;
            graph = new TGraph();
            graph -> SetMarkerStyle(mstyle);
            graph -> SetMarkerColor(mcolor);
            xMin = x0;
            xMax = x0+(n-1)*pitch;
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateWires(ComponentAnalyticField &cmp) {
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                cmp.AddWire(x, y, 2*r, v, name.Data());
                graph -> SetPoint(i,x,y);
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraph(TF2 *f2, int nTest=100, double testRange=0.25) {
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

class TexatConfiguration
{
    public:
        WireSet fWireFC0;
        WireSet fWireGG1;
        WireSet fWireGG2;
        double xMin = DBL_MAX;
        double xMax = -DBL_MAX;
        double fYMM = 0;

        TexatConfiguration() { SetParameters(); }
        ~TexatConfiguration() {};

        //////////////////////////////////////////////////////////////////////////////
        void SetParameters()
        {
            TString nameSet;
            int n, mstyle, mcolor, lcolor;
            double x0, pitch, v, y, r;
            int n1 = 101;
            int n2 = 8;
            //fWireFC0.SetPar(nameSet="FC0", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-1600, y=-11.265, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
            //fWireGG1.SetPar(nameSet="GG1", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-260,  y=-0.201,  r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
            //fWireGG2.SetPar(nameSet="GG2", n=n2, x0=-(n2/2-0.5)*0.5, pitch=0.5, v=-230,  y=-0.201,  r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed);
            fWireFC0.SetPar(nameSet="FC0", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-1600, y=-11.-3.4, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
            fWireGG1.SetPar(nameSet="GG1", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-260,  y=-3.4,     r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
            fWireGG2.SetPar(nameSet="GG2", n=n2, x0=-(n2/2-0.5)*0.5, pitch=0.5, v=-230,  y=-3.4,     r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed);
            for (auto wireSet : {fWireFC0,fWireGG1,fWireGG2}) {
                if (xMin>wireSet.xMin) xMin = wireSet.xMin;
                if (xMax<wireSet.xMax) xMax = wireSet.xMax;
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateComponents(ComponentAnalyticField &cmp) {
            fWireFC0.CreateWires(cmp);
            fWireGG1.CreateWires(cmp);
            fWireGG2.CreateWires(cmp);
            const double vMMGEM = 0.;
            cmp.AddPlaneY(fYMM,vMMGEM,"MMGEM");
        }

        //////////////////////////////////////////////////////////////////////////////
        void DrawGraph(TString option="psame")
        {
            fWireGG1.graph -> Draw(option);
            fWireGG2.graph -> Draw(option);
            fWireFC0.graph -> Draw(option);
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraph(TF2 *f2)
        {
            fWireGG1.CreateAndDrawFieldValueGraph(f2);
            fWireGG2.CreateAndDrawFieldValueGraph(f2);
        }
};

#endif
