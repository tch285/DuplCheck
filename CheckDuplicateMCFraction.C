#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraph.h>
// #include <TGaxis.h>
#include <TAxis.h>

// #include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITS/TrackITS.h"
// #include "Framework/Logger.h"

#endif

void CheckDuplicateMCFraction(TString output = ".", TString rootSave="", int rofStart = 0, int rofEnd = -1, int windowSize = 2, bool includeSame = 1) {
    int firstRun = 523306;
    int runs[] = {523306, 523786, 526596, 526938, 527821, 528448, 528798, 529115, 529324, 529462, 529662};

    // double_t IRs[] = {466242.3407, 475803.939, 488826, 500820, 498507, 503520, 486757, 507798, 502820, 486506, 485705};
    double_t IRskHz[] = {10, 50, 100, 250, 500, 1000, 1500};
    double_t dupls[] = {11325, 10787, 11104, 11238, 11041, 10410, 9879, 6076, 4836, 10363, 10429};
    double_t comps[] = {374216200, 360765331, 368283967, 384420831, 361141784, 350065762, 350011478, 203319642, 161117700, 332186470, 351347607};
    double_t tracks[11];
    double_t totaltracks = 0;
    int IR = 1;

    double_t fraction_comp[7] = {0};
    double_t fraction_track[7] = {0};
    
    for (int IRidx = 1; IRidx <= 7; IRidx++) {
        TFile *fin = TFile::Open(Form("saved_hists/IR_%i_proj.root", IRidx), "READ");
        std::vector<Double_t> *frac;
        fin->GetObject(Form("frac_%i", IRidx), frac); // I try to retrieve the vector

        // std::vector<Double_t> frac{(double_t) dupl, totalTracks, frac_track, (double_t) comps, frac_comp}; // I create and fill the vector to be saved.
        // std::cout << (*frac)[1];
        // std::cout << frac[1];
        std::cout << "For IR index " << IRidx << endl;
        fraction_comp[IRidx - 1] = (*frac)[4];
        fraction_track[IRidx - 1] = (*frac)[2];
        // for(std::vector<Double_t>::iterator it = frac->begin(); it != frac->end(); ++it) {
        //     std::cout << *it << '\n';
        // }
    }
    // for (int i = 0; i < 11; i++) {
    //     totaltracks = 0;
    //     // TString tracfile = Form("./data/%i/o2trac_its.root", runs[i]);
    //     TFile* file = TFile::Open(Form("./data/%i/o2trac_its.root", runs[i]));
    //     TTree* recTree = (TTree*)gFile->Get("o2sim");
    //     std::vector<o2::its::TrackITS>* recArr = nullptr;
    //     recTree->SetBranchAddress("ITSTrack", &recArr);
    //     for (int frame = 0; frame < recTree->GetEntriesFast(); frame++) { // Loop over timeframes
    //         recTree->GetEvent(frame);
    //         totaltracks += recArr->size();
    //     }
    //     tracks[i] = totaltracks;

    //     IRskHz[i] = IRs[i] / 1000;
    //     fraction_comp[i] = dupls[i] / comps[i];
    //     fraction_track[i] = dupls[i] * 2 / tracks[i];
    //     std::cout << i << " " << runs[i] << " " << fraction_comp[i] << " " << fraction_track[i] << endl;
    // }

    
    // std::cout << totaltracks << endl;

    auto c1 = new TCanvas("c1", "", 0, 0, 1300, 800);
    TMultiGraph *mg1 = new TMultiGraph();
    mg1->SetMinimum(0);
    mg1->SetMaximum(0.003);
    TGraph* gtrack = new TGraph(7, IRskHz, fraction_track);
    gtrack->SetMarkerColor(kRed);
    gtrack->SetTitle("duplicates / total tracks");

    mg1->Add(gtrack);
    mg1->SetTitle("Duplicate track efficiency (MC);interaction rate [kHz];Efficiency");
    // mg1->GetYaxis()->SetMaxDigits(2);
    // c1->SetLogy();
    // c1->BuildLegend();

    mg1->Draw("A*");
    // c1->SetLeftMargin(20);
    mg1->GetYaxis()->SetTitleOffset(1.5);

    c1->SaveAs("frac_trk_mc.png");





    
    auto c2 = new TCanvas("c2", "", 0, 0, 1300, 800);
    TMultiGraph *mg2 = new TMultiGraph();
    mg2->SetMinimum(0);
    TGraph* gcomp = new TGraph(7, IRskHz, fraction_comp);
    gcomp->SetMarkerColor(kBlue);
    gcomp->SetTitle("duplicate pairs / comparisons");

    mg2->Add(gcomp);
    mg2->SetTitle("\% duplicates (comparisons);interaction rate [kHz];Efficiency");
    mg2->Draw("A*");
    c2->SaveAs("frac_comp_mc.png");

}