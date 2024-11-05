#ifndef DETECTORCONFIGURATION_HH
#define DETECTORCONFIGURATION_HH

#ifndef NOGARFIELD
#include "Garfield/ComponentAnalyticField.hh"
using namespace Garfield;
#endif

#include <iostream>
#include "TString.h"
#include "TGraph.h"
#include "TF2.h"

using namespace std;

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
        TString Dump() {
            TString dump = Form("%s: (n,v)=(%d,%.0f) (x0,y)=(%.4f,%.4f), [x]=(%.4f,%.4f), (pitch,r)=(%.4f,%.4f)", name.Data(), n, v, x0, y, xMin, xMax, pitch, r);
            return dump;
        }

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
            graph = new TGraph();
            graph -> SetMarkerStyle(mstyle);
            graph -> SetMarkerColor(mcolor);
            xMin = x0;
            xMax = x0+(n-1)*pitch;
            cout << Dump() << endl;
        }

        //////////////////////////////////////////////////////////////////////////////
#ifndef NOGARFIELD
        void CreateWires(ComponentAnalyticField &cmp)
        {
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                cmp.AddWire(x, y, 2*r, v, name.Data());
                graph -> SetPoint(i,x,y);
            }
        }
#else
        void CreateWires(double yy=-99999)
        {
            if (yy<-99998) yy = y;
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                graph -> SetPoint(i,x,yy);
            }
        }
#endif

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraphX(TF2 *f2, int nTest=100, double x2=0.25) {
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                auto graph_vx = new TGraph();
                graph_vx -> SetLineColor(lcolor);
                for (double dx=0.; dx<x2; dx+=x2/nTest)
                    graph_vx->SetPoint(graph_vx->GetN(),dx,f2->Eval(x+dx,y));
                graph_vx -> Draw("samel");
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraphY(TF2 *f2, int nTest=400, double y1 = -12, double y2=4) {
            for(int i=0; i<n; i++) {
                auto x = x0+i*pitch;
                auto graph_vy = new TGraph();
                graph_vy -> SetLineColor(lcolor);
                for (double dy=y1; dy<y2; dy+=y2/nTest)
                    graph_vy->SetPoint(graph_vy->GetN(),dy,f2->Eval(x,y+dy));
                graph_vy -> Draw("samel");
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
        double fVMMGEM = 0.;

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
            //fVMMGEM = 0;
            //fWireFC0.SetPar(nameSet="FC0", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-1600, y=-11.265, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
            //fWireGG1.SetPar(nameSet="GG1", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-260,  y=-0.201,  r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
            //fWireGG2.SetPar(nameSet="GG2", n=n2, x0=-(n2/2-0.5)*0.5, pitch=0.5, v=-230,  y=-0.201,  r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed);
            //fVMMGEM = 0;
            //fWireFC0.SetPar(nameSet="FC0", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-1600, y=-11.-3.4, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
            //fWireGG1.SetPar(nameSet="GG1", n=n1, x0=  -(n1-1)/2*0.5, pitch=0.5, v=-260,  y=-3.4,     r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
            //fWireGG2.SetPar(nameSet="GG2", n=n2, x0=-(n2/2-0.5)*0.5, pitch=0.5, v=-230,  y=-3.4,     r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed);
            fVMMGEM = -230;
            fWireFC0.SetPar(nameSet="FC0", n=n1, x0=  -(n1-1)/2*0.5 + 0.2, pitch=0.5, v=-1600, y=-11.265, r=0.005, mstyle=26, mcolor=kBlack, lcolor=kBlack);
            fWireGG1.SetPar(nameSet="GG1", n=n1, x0=  -(n1-1)/2*0.5 + 0.2, pitch=0.5, v=-260,  y=-0.201,  r=0.005, mstyle=24, mcolor=kBlack, lcolor=kGray+1);
            fWireGG2.SetPar(nameSet="GG2", n=n2, x0=-(n2/2-0.5)*0.5 + 0.2, pitch=0.5, v=-230,  y=-0.201,  r=0.005, mstyle=25, mcolor=kRed  , lcolor=kRed);
            for (auto wireSet : {fWireFC0,fWireGG1,fWireGG2}) {
                if (xMin>wireSet.xMin) xMin = wireSet.xMin;
                if (xMax<wireSet.xMax) xMax = wireSet.xMax;
            }
        }

        //////////////////////////////////////////////////////////////////////////////
#ifndef NOGARFIELD
        void CreateComponents(ComponentAnalyticField &cmp)
        {
            fWireFC0.CreateWires(cmp);
            fWireGG1.CreateWires(cmp);
            fWireGG2.CreateWires(cmp);
            cmp.AddPlaneY(fYMM,fVMMGEM,"MMGEM");
        }
#else
        void CreateComponents(double yy=-99999)
        {
            //fWireFC0.CreateWires(yy);
            fWireGG1.CreateWires(yy);
            fWireGG2.CreateWires(yy);
        }
#endif

        TString Dump()
        {
            TString dump;
            dump = dump + fWireFC0.Dump() + "\n";
            dump = dump + fWireGG1.Dump() + "\n";
            dump = dump + fWireGG2.Dump() + "\n";
            dump = dump + "GEM: " + fYMM + " " + fVMMGEM;
            return dump;
        }

        //////////////////////////////////////////////////////////////////////////////
        void DrawGraph(TString option="psame")
        {
            if (fWireGG1.graph->GetN()>0) fWireGG1.graph -> Draw(option);
            if (fWireGG2.graph->GetN()>0) fWireGG2.graph -> Draw(option);
            if (fWireFC0.graph->GetN()>0) fWireFC0.graph -> Draw(option);
        }

        void DrawWires(TString option="psame") { DrawGraph(option); }

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraphX(TF2 *f2)
        {
            fWireGG1.CreateAndDrawFieldValueGraphX(f2);
            fWireGG2.CreateAndDrawFieldValueGraphX(f2);
        }

        //////////////////////////////////////////////////////////////////////////////
        void CreateAndDrawFieldValueGraphY(TF2 *f2)
        {
            fWireGG1.CreateAndDrawFieldValueGraphY(f2);
            fWireGG2.CreateAndDrawFieldValueGraphY(f2);
        }
};

#endif
