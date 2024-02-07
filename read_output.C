void read_output()
{
    gStyle -> SetOptStat(0);

    ifstream file("list_root");
    TChain *chain = new TChain("gg");
    TString fileName;
    while (file >> fileName)
        chain -> AddFile(fileName);

    cout << chain -> GetEntries() << endl;

    TCanvas *cvs = new TCanvas("cvsAll","",3000,1000);
    cvs -> Divide(2,1);

    cvs -> cd(1);
    auto hist1 = new TH1D("hist1","start;x;count;",240,-10,10);
    hist1 -> SetMinimum(0);
    hist1 -> SetFillColor(kGray);
    chain -> Draw("xStart>>hist1");

    cvs -> cd(2);
    auto hist2 = new TH1D("hist2","end;x;count;",240,-10,10);
    hist2 -> SetFillColor(kGray);
    chain -> Draw("xEnd>>hist2","yEnd>-0.2");

    //cvs -> cd(3);
    //auto hist3 = new TH2D("hist3",";x;y;",240,-10,10,100,-0.2,0.1);
    //chain -> Draw("yEnd:xEnd>>hist3","","colz");

    //cvs -> cd(4);
    //auto hist4 = new TH2D("hist4",";x0;x1-x0;",240,-10,10,100,-2,2);
    //chain -> Draw("xEnd-xStart:xStart>>hist4","yEnd>-0.2","colz");
}
