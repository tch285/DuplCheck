#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TGaxis.h>
#include <TAxis.h>
#include <TLegend.h>

// #include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITS/TrackITS.h"
// #include "Framework/Logger.h"

#endif

void DuplFracData(int rofStart = 0, int rofEnd = -1, int windowSize = 2, bool includeSame = 1) {
    int runs[] = {523306, 523786, 526596, 526938, 527821, 528448, 528798, 529115, 529324, 529462, 529662};
    double_t IRs[] = {466242.3407, 475803.939, 488826, 500820, 498507, 503520, 486757, 507798, 502820, 486506, 485705};
    double_t IRskHz[11];
    double_t fraction3[11];
    double_t fraction2[11];

    int runs_range[] = {536340, 536402, 536490, 536608, 536899, 536908};
    double_t IRs_range[] = {10.53854743, 53.017, 98.557, 238.661, 972.625, 735.5717224};
    double_t fraction3_range[6];
    double_t fraction2_range[6];

    for (int i = 0; i < 11; i++) {

        TFile *fin3 = TFile::Open(Form("saved_hists_3_001/%i.root", runs[i]), "READ");
        std::vector<Double_t> *frac3;
        fin3->GetObject(Form("frac_%i", runs[i]), frac3); // I try to retrieve the vector
        fraction3[i] = (*frac3)[2];

        TFile *fin2 = TFile::Open(Form("saved_hists_2_001/%i.root", runs[i]), "READ");
        std::vector<Double_t> *frac2;
        fin2->GetObject(Form("frac_%i", runs[i]), frac2); // I try to retrieve the vector
        fraction2[i] = (*frac2)[2];

        IRskHz[i] = IRs[i] / 1000;
        std::cout << i << " " << runs[i] << " " << fraction3[i] << " " << fraction2[i] << endl;
    }
    
    for (int i = 0; i < 6; i++) {

        TFile *fin3_range = TFile::Open(Form("saved_hists_3_001/%i.root", runs_range[i]), "READ");
        std::vector<Double_t> *frac3_range;
        fin3_range->GetObject(Form("frac_%i", runs_range[i]), frac3_range); // I try to retrieve the vector
        fraction3_range[i] = (*frac3_range)[2];

        TFile *fin2_range = TFile::Open(Form("saved_hists_2_001/%i.root", runs_range[i]), "READ");
        std::vector<Double_t> *frac2_range;
        fin2_range->GetObject(Form("frac_%i", runs_range[i]), frac2_range); // I try to retrieve the vector
        fraction2_range[i] = (*frac2_range)[2];

        std::cout << i << " " << runs_range[i] << " " << fraction3_range[i] << " " << fraction2_range[i] << endl;
    }

    
    auto c1 = new TCanvas("c1", "", 0, 0, 1300, 800);
    TMultiGraph *mg1 = new TMultiGraph();
    mg1->SetMinimum(0);
    // mg1->SetMaximum(0.01);
    TGraph* gtrack3 = new TGraph(11, IRskHz, fraction3);
    gtrack3->SetMarkerColor(kRed);
    gtrack3->SetLineColor(kRed);
    gtrack3->SetTitle("window size 3");
    TGraph* gtrack2 = new TGraph(11, IRskHz, fraction2);
    gtrack2->SetMarkerColor(kBlue);
    gtrack2->SetLineColor(kBlue);
    gtrack2->SetTitle("window size 2");
    TGraph* gtrack3_range = new TGraph(6, IRs_range, fraction3_range);
    gtrack3_range->SetMarkerColor(kGreen);
    gtrack3_range->SetLineColor(kGreen);
    gtrack3_range->SetTitle("window size 3");
    TGraph* gtrack2_range = new TGraph(6, IRs_range, fraction2_range);
    gtrack2_range->SetMarkerColor(kBlack);
    gtrack2_range->SetLineColor(kBlack);
    gtrack2_range->SetTitle("window size 2");

    mg1->Add(gtrack3);
    mg1->Add(gtrack2);
    mg1->Add(gtrack3_range);
    mg1->Add(gtrack2_range);
    mg1->SetTitle("Duplicate track efficiency (data);interaction rate [kHz];Efficiency");
    // mg1->GetYaxis()->SetMaxDigits(2);
    // c1->SetLogy();

    mg1->Draw("A*");
    mg1->GetYaxis()->SetTitleOffset(1.5);
    TLegend *legend = new TLegend(0.4, 0.2, 0.6, 0.3);
    legend->AddEntry(gtrack3,"2022 w=3","p");
    legend->AddEntry(gtrack2,"2022 w=2","p");
    legend->AddEntry(gtrack3_range,"2023 w=3","p");
    legend->AddEntry(gtrack2_range,"2023 w=2","p");
    legend->Draw();

    c1->SaveAs("frac_trk_data_001.png");

}