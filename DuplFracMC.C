#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraph.h>
// #include <TGaxis.h>
#include <TAxis.h>
#include <TLegend.h>

// #include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITS/TrackITS.h"
// #include "Framework/Logger.h"

#endif

void DuplFracMC(int rofStart = 0, int rofEnd = -1, int windowSize = 2, bool includeSame = 1) {
    // int firstRun = 523306;
    int runs[] = {523306, 523786, 526596, 526938, 527821, 528448, 528798, 529115, 529324, 529462, 529662};
    // int run;
    double_t IRs[] = {466242.3407, 475803.939, 488826, 500820, 498507, 503520, 486757, 507798, 502820, 486506, 485705};
    double_t IRskHz[] = {10, 50, 100, 250, 500, 1000, 1500};
    double_t fraction3[7];
    double_t fraction2[7];

        // TFile *fin3 = TFile::Open(Form("saved_hists/IR_%i_proj3.root", 1), "READ");
        // std::cout << Form("saved_hists/IR_%i_proj3.root", 1) << endl;
        // std::vector<Double_t> *frac3;
        // fin3->GetObject(Form("frac_%i", 1), frac3); // I try to retrieve the vector
        // std::cout << (*frac3)[2];

    // for (int i = 0; i < 11; i++) {
    for (int i = 1; i <= 7; i++) {

        // TFile *fin3 = TFile::Open(Form("saved_hists_3_01/%i.root", runs[i]), "READ");
        TFile *fin3 = TFile::Open(Form("saved_hists/IR_%i_proj3.root", i), "READ");
        std::cout << Form("saved_hists/IR_%i_proj3.root", i) << endl;
        std::vector<Double_t> *frac3;
        // fin3->GetObject(Form("frac_%i", runs[i]), frac3); // I try to retrieve the vector
        fin3->GetObject(Form("frac_%i", i), frac3); // I try to retrieve the vector
        // std::cout << (*frac3)[2];
        // fraction3[i] = (*frac3)[2];
        fraction3[i-1] = (*frac3)[2];

        // TFile *fin2 = TFile::Open(Form("saved_hists_2_01/%i.root", runs[i]), "READ");
        TFile *fin2 = TFile::Open(Form("saved_hists/IR_%i_proj2.root", i), "READ");
        std::vector<Double_t> *frac2;
        // fin2->GetObject(Form("frac_%i", runs[i]), frac2); // I try to retrieve the vector
        fin2->GetObject(Form("frac_%i", i), frac2); // I try to retrieve the vector
        // fraction2[i] = (*frac2)[2];
        fraction2[i-1] = (*frac2)[2];


        // IRskHz[i] = IRs[i] / 1000;
        // std::cout << i << " " << runs[i] << " " << fraction3[i] << " " << fraction2[i] << endl;
        std::cout << i << " " << fraction3[i-1] << " " << fraction2[i-1] << endl;
    }

    
    auto c1 = new TCanvas("c1", "", 0, 0, 1300, 800);
    TMultiGraph *mg1 = new TMultiGraph();
    mg1->SetMinimum(0);
    // mg1->SetMaximum(0.002);
    TGraph* gtrack3 = new TGraph(7, IRskHz, fraction3);
    gtrack3->SetMarkerColor(kRed);
    gtrack3->SetLineColor(kRed);
    gtrack3->SetTitle("window size 3");
    TGraph* gtrack2 = new TGraph(7, IRskHz, fraction2);
    gtrack2->SetMarkerColor(kBlue);
    gtrack2->SetLineColor(kBlue);
    gtrack2->SetTitle("window size 2");

    mg1->Add(gtrack3);
    mg1->Add(gtrack2);
    // mg1->SetTitle("Duplicate track efficiency (data);interaction rate [kHz];Efficiency");
    mg1->SetTitle("Duplicate track efficiency (MC);interaction rate [kHz];Efficiency");
    // mg1->GetYaxis()->SetMaxDigits(2);
    // c1->SetLogy();

    mg1->Draw("A*");
    mg1->GetYaxis()->SetTitleOffset(1.5);
    TLegend *legend = new TLegend(0.4, 0.2, 0.6, 0.3);
    legend->AddEntry(gtrack3,"window size 3","l");
    legend->AddEntry(gtrack2,"window size 2","l");
    legend->Draw();

    // c1->SaveAs("frac_trk_data.png");
    c1->SaveAs("frac_trk_mc.png");

}