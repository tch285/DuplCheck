// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file CheckTrkAvgs.C
/// \brief Macro to look for duplicate tracks across a ROF windows

#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TStyle.h>
#include <TGaxis.h>
#include <TMultiGraph.h>
#include <TGraph.h>
// #include <TAxis.h>
#include <TLegend.h>

#include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITS/TrackITS.h"
#include "Framework/Logger.h"

#endif

void fill_avgs(std::vector<int> &runs, std::vector<double_t> &avgs, int rofEnd) {
    if (runs[0] < 100) { // this must be MC
        for (int run : runs) {
            int totalTracks{0}, totalROFs{0};
            for (int tf = 1; tf <= 5; tf++) {
                // Obtain data from files:
                TFile *file1 = TFile::Open(Form("~/alice/mc_real/data/mc/%i/o2trac_its_01_tf%i.root", run, tf));
                // TODO: possible solution is to loop through every file with this: https://stackoverflow.com/questions/67273/how-do-you-iterate-through-every-file-directory-recursively-in-standard-c
                TTree *recTree = (TTree *)gFile->Get("o2sim");
                // Extract track information
                std::vector<o2::its::TrackITS> *recArr = nullptr;
                recTree->SetBranchAddress("ITSTrack", &recArr);
                // Extract information on which ROFs contain which tracks
                std::vector<o2::itsmft::ROFRecord> *rofArr = nullptr;
                recTree->SetBranchAddress("ITSTracksROF", &rofArr);
        
                int rofFrameEnd = rofEnd; // In case frames have different numbers of ROFs, store the final ROF index separately for each frame inside rofFrameEnd
                int totalFrames = recTree->GetEntriesFast();

                for (int frame = 0; frame < totalFrames; frame++)
                { // Loop over timeframes
                    LOGP(info, "Proceeding to frame {} of {}", frame + 1, totalFrames);
                    recTree->GetEvent(frame);
                    LOGP(info, "{} ROFs in frame {}/{}", rofArr->size(), frame + 1, totalFrames);
                    if (rofEnd >= (int long)rofArr->size() || rofEnd < 0)
                    { // Check whether rofEnd is greater than the total number of ROFs in this dataset, or negative; if so, set to maximum
                        LOGP(warn, "Frame {}/{}: currently set upper ROF limit {} out of bounds; set to maximum {} instead.", frame + 1, totalFrames, rofEnd, rofArr->size() - 1);
                        rofFrameEnd = rofArr->size() - 1;
                    }
                    totalTracks += recArr->size();
                    totalROFs += rofArr->size();
                }
            }
            double_t average = (double_t) totalTracks / (double_t) totalROFs;
            std::cout << "Average for this run " << run << " (MC) is " << average << " tracks per ROF." << endl;

            avgs.push_back(average);
        }
    }
    else { // this must be data
        for (int run : runs) {
            // Obtain data from files:
            TFile *file1 = TFile::Open(Form("~/alice/mc_real/data/%i/o2trac_its.root", run));
            TTree *recTree = (TTree *)gFile->Get("o2sim");
            // Extract track information
            std::vector<o2::its::TrackITS> *recArr = nullptr;
            recTree->SetBranchAddress("ITSTrack", &recArr);
            // Extract information on which ROFs contain which tracks
            std::vector<o2::itsmft::ROFRecord> *rofArr = nullptr;
            recTree->SetBranchAddress("ITSTracksROF", &rofArr);
    
            int rofFrameEnd = rofEnd; // In case frames have different numbers of ROFs, store the final ROF index separately for each frame inside rofFrameEnd
            int totalFrames = recTree->GetEntriesFast();

            int totalTracks{0}, totalROFs{0};
            for (int frame = 0; frame < totalFrames; frame++)
            { // Loop over timeframes
                LOGP(info, "Proceeding to frame {} of {}", frame + 1, totalFrames);
                recTree->GetEvent(frame);
                LOGP(info, "{} ROFs in frame {}/{}", rofArr->size(), frame + 1, totalFrames);
                if (rofEnd >= (int long)rofArr->size() || rofEnd < 0)
                { // Check whether rofEnd is greater than the total number of ROFs in this dataset, or negative; if so, set to maximum
                    LOGP(warn, "Frame {}/{}: currently set upper ROF limit {} out of bounds; set to maximum {} instead.", frame + 1, totalFrames, rofEnd, rofArr->size() - 1);
                    rofFrameEnd = rofArr->size() - 1;
                }
                totalTracks += recArr->size();
                totalROFs += rofArr->size();
                
            }
            double_t average = (double_t) totalTracks / (double_t) totalROFs;
            std::cout << "Average for this run " << run << " (data) is " << average << " tracks per ROF." << endl;

            avgs.push_back(average);
        }
    }

}

void CheckTrkAvgs(TString tracfile = "data/523306/o2trac_its.root", TString output = ".", TString rootSave = "", int rofStart = 0, int rofEnd = -1, int windowSize = 2, bool includeSame = 1)
{
    fair::Logger::SetFileSeverity("error");
    gErrorIgnoreLevel = kWarning;

    std::vector<int> runs_500{523306, 523786, 526596, 526938, 527821, 528448, 528798, 529115, 529324, 529462, 529662};
    std::vector<double_t> IRs_500{466.2423407, 475.803939, 488.826, 500.820, 498.507, 503.520, 486.757, 507.798, 502.820, 486.506, 485.705};
    std::vector<double_t> avgs_500;

    std::vector<int> runs_range{536340, 536402, 536490, 536608, 536899, 536908};
    std::vector<double_t> IRs_range{10.53854743, 53.017, 98.557, 238.661, 972.625, 735.5717224};
    std::vector<double_t> avgs_range;

    std::vector<int> runs_mc{1, 2, 3, 4, 5, 6, 7}; // indicates that this is MC and thus has no run number
    std::vector<double_t> IRs_mc{10, 50, 100, 250, 500, 1000, 1500};
    std::vector<double_t> avgs_mc;

    fill_avgs(runs_500, avgs_500, rofEnd);
    fill_avgs(runs_range, avgs_range, rofEnd);
    fill_avgs(runs_mc, avgs_mc, rofEnd);



    
    auto c1 = new TCanvas("c1", "", 0, 0, 1300, 800);
    TMultiGraph *mg1 = new TMultiGraph();
    mg1->SetMinimum(0);
    mg1->SetTitle("Average tracks / ROF;interaction rate [kHz];tracks per ROF");
    // mg1->SetMaximum(0.002);

    TGraph* g_500 = new TGraph(IRs_500.size(), &IRs_500[0], &avgs_500[0]);
    g_500->SetMarkerColor(kRed);
    g_500->SetLineColor(kRed);
    g_500->SetTitle("Data (IR=500 kHz)");
    mg1->Add(g_500);

    TGraph* g_range = new TGraph(IRs_range.size(), &IRs_range[0], &avgs_range[0]);
    g_range->SetMarkerColor(kBlue);
    g_range->SetLineColor(kBlue);
    g_range->SetTitle("Data (full range)");
    mg1->Add(g_range);

    TGraph* g_mc = new TGraph(IRs_mc.size(), &IRs_mc[0], &avgs_mc[0]);
    g_mc->SetMarkerColor(kGreen);
    g_mc->SetLineColor(kGreen);
    g_mc->SetTitle("Monte Carlo");
    mg1->Add(g_mc);


    mg1->Draw("A*");
    mg1->GetYaxis()->SetTitleOffset(1.5);
    TLegend *legend = new TLegend(0.4, 0.2, 0.6, 0.3);
    legend->AddEntry(g_500,"Data (IR#approx500 kHz)","p");
    legend->AddEntry(g_range,"Data (full IR range)","p");
    legend->AddEntry(g_mc,"MC","p");
    legend->Draw();

    c1->SaveAs("trk_avgs.png");

}