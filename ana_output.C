#define NOGARFIELD
#include "DetectorConfiguration.h"

void ana_output()
{
    TexatConfiguration conf;
    double userYPosition = 10;
    conf.CreateComponents(userYPosition);

    //auto file = new TFile("data/output_1000_1_-3.00_3.00__0.root");
    //auto file = new TFile("data/output_1000_1_-3.50_3.50__0.root");
    //auto file = new TFile("data/output_2000_1_-5.00_5.00__0.root");
    //auto file = new TFile("data/output_200_1_-5.00_5.00__0.root");
    auto file = new TFile("data/output_10000_100_-10.00_10.00__0.root");
    auto tree = (TTree*) file -> Get("gg");

    double padGapChain = 0;
    double padSizeChain = .167;
    double padGapCenter = 0;
    double padSizeCenter = .342;

    int numChains1 = 25;
    int numCenter1 = 3;
    int numChains = 2 * numChains1;
    int numCenter = 2 * numCenter1;
    int numBins = numChains + numCenter;

    double x1 = - (numCenter1 * padSizeCenter) - (numChains1 * padSizeChain);
    double x2 = - (numCenter1 * padSizeCenter);
    double x3 = 0;
    double x4 = + (numCenter1 * padSizeCenter);
    double x5 = + (numCenter1 * padSizeCenter) + (numChains1 * padSizeChain);
    cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 << endl;

    int countBin = 0;
    double xbins[100];
    for (auto iBin=0; iBin<numChains1; ++iBin) xbins[countBin++] = x1 + padSizeChain*iBin;
    for (auto iBin=0; iBin<numCenter1; ++iBin) xbins[countBin++] = x2 + padSizeCenter*iBin;
    for (auto iBin=0; iBin<numCenter1; ++iBin) xbins[countBin++] = x3 + padSizeCenter*iBin;
    for (auto iBin=0; iBin<numChains1; ++iBin) xbins[countBin++] = x4 + padSizeChain*iBin;
    xbins[countBin++] = x5;

    //TH1D (const char *name, const char *title, Int_t nbinsx, const Double_t *xbins)
    auto histStart0    = new TH1D("histStart0" ,"(All) Equal size binning;x (cm)",40,x1,x5);
    auto histStart     = new TH1D("histStart" ,"(All) Real pad size binning;x (cm)",numBins,xbins);
    auto histAbsorbed  = new TH1D("histAbsorbed" ,"(Absorbed by wires);x (cm)",100,x1,x5);
    auto histCollected = new TH1D("histCollected","(Collected by pads) Real pad size binning;x (cm)",numBins,xbins);
    histStart0    -> SetMinimum(0);
    histStart     -> SetMinimum(0);
    histAbsorbed  -> SetMinimum(0);
    histCollected -> SetMinimum(0);
    histStart0    -> SetFillColor(kGray);
    histStart     -> SetFillColor(kGray);
    histAbsorbed  -> SetFillColor(kGray);
    histCollected -> SetFillColor(kGray);

    auto DrawPadBoundary = [](TH1 *hist) {
        auto histLColor = hist -> GetLineColor();
        auto numBins = hist -> GetXaxis() -> GetNbins();
        for (auto iBin=1; iBin<=numBins; ++iBin) {
            auto y = hist -> GetBinContent(iBin);
            auto x = hist -> GetXaxis() -> GetBinLowEdge(iBin);
            auto line = new TLine(x,0,x,y);
            line -> SetLineColor(histLColor);
            line -> SetLineStyle(2);
            line -> Draw("samel");
        }
    };

    auto cvs = new TCanvas("cvs","",1500,1200);
    cvs -> Divide(2,2);
    cvs -> cd(1); tree -> Draw("xStart>>histStart0" ,""       ); conf.DrawWires();
    cvs -> cd(2); tree -> Draw("xStart>>histStart"  ,""       ); DrawPadBoundary(histStart); conf.DrawWires();
    cvs -> cd(3); tree -> Draw("xEnd>>histAbsorbed" ,"yEnd<-1"); conf.DrawWires();
    cvs -> cd(4); tree -> Draw("xEnd>>histCollected","yEnd>-1"); DrawPadBoundary(histCollected); conf.DrawWires();

    cvs -> SaveAs("figures/figure_e_collected_and_absorbed.png");

    auto cvs2 = new TCanvas("cvs2","",800,600);
    //auto histY = new TH1D("histY",Form("xStart>%.4f && xStart<%.4f;y_{End};count",x2,x4),40,-4,1.0);
    auto histY = new TH1D("histY",Form("xStart>%.4f && xStart<%.4f;y_{End};count",x2,x4),40,-0.3,0.1);
    tree -> Draw("yEnd>>histY",Form("xStart>%f&&xStart<%f",x2,x4));


    auto cvs3 = new TCanvas("cvs3","",800,600);
    auto histCollected1 = (TH1D*) histCollected -> Clone("histCollected1");
    histCollected1 -> SetTitle("histCollected1 / histStart");
    histCollected1 -> Divide(histStart);
    histCollected1 -> Draw();
    DrawPadBoundary(histCollected1);
    conf.DrawWires();

    for (auto iBin=1; iBin<=numBins; ++iBin)
    {
        auto value = histCollected1 -> GetBinContent(iBin);
        auto center = histCollected1 -> GetBinCenter(iBin);
        cout << iBin << " " << center << " " << value << endl;
    }
}
