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
                auto frame = new TH2D(name+"_v",";Offset dx from wire center [cm];Field value (V)",nTest,0,testRange,100,(v>0? -1.2*v:1.2*v),(v>0?1.2*v:-1.2*v));
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
