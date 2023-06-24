/// \file CheckDuplicatesWindow.C
/// \brief Macro to look for duplicate tracks across a ROF window

#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <THStack.h>
#include <TLegend.h>
#include <TPad.h>

#include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITS/TrackITS.h"
#include "fairlogger/Logger.h"

#endif



using namespace std;
// 523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662

void CheckDuplFile(int rofStart = 0, int rofEnd = -1, int windowSize = 2, bool includeSame = 1) {
    fair::Logger::SetFileSeverity("warn");
    gErrorIgnoreLevel = kWarning;

    int firstRun = 523306;
    int runs[] = {523786, 526596, 526938, 527821, 528448, 528798, 529115, 529324, 529462, 529662};

    TFile* input = new TFile(Form("./saved_hists_01/%i.root", firstRun), "READ");
    TH1D *hphi = (TH1D*)input->Get("pPhi");
    TH1D *heta = (TH1D*)input->Get("pEta");

    for (int run : runs) {
        TFile* input = new TFile(Form("./saved_hists_01/%i.root", run), "READ");
        TH1D *h1 = (TH1D*)input->Get("pPhi");
        TH1D *h2 = (TH1D*)input->Get("pEta");
        hphi->Add(h1);
        heta->Add(h2);
    }

    auto cphi = new TCanvas("cphi", "Duplicates avg. #phi", 0, 0, 1200, 800);
    cphi->SetLogy();
    hphi->SetTitle("Duplicates;avg. #phi");
    hphi->SetStats(1);
    hphi->Draw();

    auto ceta = new TCanvas("ceta", "Duplicates avg. #eta", 0, 0, 1200, 800);
    ceta->SetLogy();
    heta->SetTitle("Duplicates;avg. #eta");
    heta->SetStats(1);
    heta->Draw();

    cphi->SaveAs("plots/combined_phi_01.png");
    ceta->SaveAs("plots/combined_eta_01.png");
    TString filename = TString::Format("plots/runs_combined_01.pdf");
    cphi->Print(filename+"[");
    cphi->Print(filename);
    ceta->Print(filename);
    ceta->Print(filename+"]");
    // input->Close();
}