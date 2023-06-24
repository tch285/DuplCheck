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
#include "Framework/Logger.h"

#endif



using namespace std;
// 523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662

void CheckDuplMCComb(int IR = 7, int windowSize = 2, int rofStart = 0, int rofEnd = -1, bool includeSame = 1) {
    fair::Logger::SetFileSeverity("info");
    gErrorIgnoreLevel = kWarning;
    // Specify plot parameters
    int phiBins{101}, etaBins{101}; // number of bins for phi and eta axes
    double_t phiMin{-6.5}, phiMax{6.5}, etaMin{-4}, etaMax{4}; // phi and eta axes limits
    double_t phiLim{0.001}, etaLim{0.001}; // limits for determining duplicate status
    auto h1 = new TH2D("h1", "", phiBins, phiMin, phiMax, etaBins, etaMin, etaMax); //
    auto h2 = new TH2D("h2", "", phiBins, 0, phiMax, etaBins, 0, etaMax); // Distribution of all comparisons over dPhi and dEta
    auto h3 = new TH2D("dupl", "Duplicate candidates; avg. #phi; avg. #eta", 100, -1*phiLim, phiLim, 100, -1*etaLim, etaLim); // Distribution of duplicates over dPhi and dEta
    auto h4 = new TH2D("h4", "Duplicate candidates; avg. #phi; avg. #eta", 50, 0, phiMax, 50, etaMin, etaMax); // Distribution of duplicate pairs over avg. phi and avg. eta

    int comps{0}, dupl{0}, zeroes{0}, fakedupls{0}, totalROFs{0};
    double_t eta1, eta2, phi1, phi2, dPhi, dEta, avgPhi, avgEta;
    int rofLow, rofHigh, rofWHighMin, rofWHighMax, rof1Low, rof1High, rof2Low, rof2High;
    int rofWLow = rofStart;
    int rofWHigh = rofStart + windowSize - 1;
    int rofFrameEnd = rofEnd;
    int totalFrames;

    int tfs[] = {1, 2, 3, 4, 5};
    // int tfs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    TString tracfile;
    double_t totalTracks{0};
    for (int tf : tfs) {
        // Obtain data from files:
        // if (tf < 6) {tracfile = TString::Format("./data/mc/%i/o2trac_its_02_tf%i.root", IR, tf); std::cout << tf << endl;}
        // else {tracfile = TString::Format("./data/mc/%i/o2trac_its_01_tf%i.root", IR, tf); std::cout << tf << endl;}
        tracfile = TString::Format("./data/mc/%i/o2trac_its_01_tf%i.root", IR, tf);

        TFile* file1 = TFile::Open(tracfile);
        TTree* recTree = (TTree*)gFile->Get("o2sim");
        // Extract track information
        std::vector<o2::its::TrackITS>* recArr = nullptr;
        recTree->SetBranchAddress("ITSTrack", &recArr);
        // Extract information on which ROFs contain which tracks
        std::vector<o2::itsmft::ROFRecord>* rofArr = nullptr;
        recTree->SetBranchAddress("ITSTracksROF", &rofArr);

        totalFrames = recTree->GetEntriesFast();
        for (int frame = 0; frame < totalFrames; frame++) { // Loop over timeframes
            LOGP(info, "Proceeding to frame {} of {}", frame + 1, totalFrames);
            recTree->GetEvent(frame);
            LOGP(info, "{} ROFs in frame {}/{}", rofArr->size(), frame + 1, totalFrames);
            totalROFs += rofArr->size();
            totalTracks += recArr->size();
            // Check whether rofEnd is greater than the total number of ROFs in this dataset, or negative; if so, set to maximum
            if (rofEnd >= (int long) rofArr->size() || rofEnd < 0) { 
                LOGP(warn, "Frame {}/{}: currently set upper ROF limit {} out of bounds; set to maximum {} instead.", frame + 1, totalFrames, rofEnd, rofArr->size() - 1);
                rofFrameEnd = rofArr->size() - 1;
            }

            // First loop: compare tracks in every ROF pair in the first window
            for (int rof1 = rofStart; rof1 < rofStart + windowSize - 1; rof1++) {
                for (int rof2 = rof1 + 1; rof2 <= rofStart + windowSize - 1; rof2++) {
                    rof1Low  = rofArr->at(rof1).getEntry().getFirstEntry();
                    rof1High = rofArr->at(rof1).getEntry().getEntriesBound();
                    rof2Low = rofArr->at(rof2).getEntry().getFirstEntry();
                    rof2High = rofArr->at(rof2).getEntry().getEntriesBound();
                    for (int iTrack1 = rof1Low; iTrack1 < rof1High; ++iTrack1) {
                        for (int iTrack2 = rof2Low; iTrack2 < rof2High; ++iTrack2) {
                            phi1 = recArr->at(iTrack1).getPhi();
                            phi2 = recArr->at(iTrack2).getPhi();
                            eta1 = recArr->at(iTrack1).getEta();
                            eta2 = recArr->at(iTrack2).getEta();

                            dPhi = phi1 - phi2;
                            dEta = eta1 - eta2;
                            avgPhi = (phi1 + phi2) / 2;
                            avgEta = (eta1 + eta2) / 2;
                            h1->Fill(dPhi, dEta);
                            h2->Fill(abs(dPhi), abs(dEta));

                            if (abs(dPhi) < phiLim && abs(dEta) < etaLim) {
                                if (dPhi == 0 && dEta == 0) {
                                    LOGP(warning, "Zero found; ROFs: {}, {}. Tracks: {}, {}. dPhi=dEta=0. Not included in plots.", rof1, rof2, iTrack1, iTrack2);
                                    zeroes++;
                                }
                                else {
                                    LOGP(info, "Possible duplicate found. ROFs: {}, {}. Tracks: {}, {}. dPhi={}, dEta={}.", rof1, rof2, iTrack1, iTrack2, dPhi, dEta);
                                    dupl++;
                                    h3->Fill(dPhi, dEta);
                                    h4->Fill(avgPhi, avgEta);
                                }
                            }
                            ++comps;
                        }
                    }
                }
            }
            LOGP(info, "Frame {}/{}: initial ROF window {}-{} complete. {} possible duplicates out of {} total comparisons.", frame + 1, totalFrames, rofStart, rofStart + windowSize - 1, dupl, comps);

            // Next loops: for each new window, pair every ROF except the last ROF with the last ROF
            for (int rofWLow = rofStart + 1; rofWLow <= rofFrameEnd - windowSize + 1; rofWLow++) {
                int rof2 = rofWLow + windowSize - 1;
                for (int rof1 = rofWLow; rof1 < rof2; rof1++) {
                    rof1Low  = rofArr->at(rof1).getEntry().getFirstEntry();
                    rof1High = rofArr->at(rof1).getEntry().getEntriesBound();
                    rof2Low = rofArr->at(rof2).getEntry().getFirstEntry();
                    rof2High = rofArr->at(rof2).getEntry().getEntriesBound();
                    for (int iTrack1 = rof1Low; iTrack1 < rof1High; ++iTrack1) {
                        for (int iTrack2 = rof2Low; iTrack2 < rof2High; ++iTrack2) {
                            phi1 = recArr->at(iTrack1).getPhi();
                            phi2 = recArr->at(iTrack2).getPhi();
                            eta1 = recArr->at(iTrack1).getEta();
                            eta2 = recArr->at(iTrack2).getEta();

                            dPhi = phi1 - phi2;
                            dEta = eta1 - eta2;
                            avgPhi = (phi1 + phi2) / 2;
                            avgEta = (eta1 + eta2) / 2;
                            h1->Fill(dPhi, dEta);
                            h2->Fill(abs(dPhi), abs(dEta));

                            if (abs(dPhi) < phiLim && abs(dEta) < etaLim) {
                                if (dPhi == 0 && dEta == 0) {
                                    LOGP(warning, "Zero found; ROFs: {}, {}. Tracks: {}, {}. dPhi=dEta=0. Not included in plots.", rof1, rof2, iTrack1, iTrack2);
                                    zeroes++;
                                }
                                else {
                                    LOGP(info, "Possible duplicate found. ROFs: {}, {}. Tracks: {}, {}. dPhi={}, dEta={}.", rof1, rof2, iTrack1, iTrack2, dPhi, dEta);
                                    dupl++;
                                    h3->Fill(dPhi, dEta);
                                    h4->Fill(avgPhi, avgEta);
                                }
                            }
                            ++comps;
                        }
                    }
                }
            }
            LOGP(info, "Frame {}/{}: inter-ROF analysis complete. {} possible duplicates out of {} total comparisons.", frame + 1, totalFrames, dupl, comps);
            
            if (includeSame) {
                for (int rof = rofStart; rof <= rofFrameEnd; rof++) {
                    rofLow  = rofArr->at(rof).getEntry().getFirstEntry();
                    rofHigh = rofArr->at(rof).getEntry().getEntriesBound();
                    for (int iTrack1 = rofLow; iTrack1 < rofHigh; ++iTrack1) {
                        for (int iTrack2 = iTrack1 + 1; iTrack2 < rofHigh; ++iTrack2) {
                            phi1 = recArr->at(iTrack1).getPhi();
                            phi2 = recArr->at(iTrack2).getPhi();
                            eta1 = recArr->at(iTrack1).getEta();
                            eta2 = recArr->at(iTrack2).getEta();

                            dPhi = phi1 - phi2;
                            dEta = eta1 - eta2;
                            avgPhi = (phi1 + phi2) / 2;
                            avgEta = (eta1 + eta2) / 2;
                            h1->Fill(dPhi, dEta);
                            h2->Fill(abs(dPhi), abs(dEta));

                            if (abs(dPhi) < phiLim && abs(dEta) < etaLim) {
                                if (dPhi == 0 && dEta == 0) {
                                    LOGP(warning, "Zero found; ROFs: {}, {}. Tracks: {}, {}. dPhi=dEta=0. Not included in plots.", rof, rof, iTrack1, iTrack2);
                                    zeroes++;
                                }
                                else {
                                    LOGP(info, "Possible duplicate found. ROFs: {}, {}. Tracks: {}, {}. dPhi={}, dEta={}.", rof, rof, iTrack1, iTrack2, dPhi, dEta);
                                    dupl++;
                                    h3->Fill(dPhi, dEta);
                                    h4->Fill(avgPhi, avgEta);
                                }
                            }
                            ++comps;
                        }
                    }
                }
                LOGP(info, "Frame {}/{}: intra-ROF analysis complete. {} possible duplicates out of {} total comparisons.", frame + 1, totalFrames, dupl, comps);
            }
            else LOGP(info, "Frame {}/{}: intra-ROF analysis not included as per parameters (includeSame={})", frame + 1, totalFrames, includeSame);
        }
    }
    std::cout << totalFrames << endl;

    TString prefix = Form("IR%i", IR); // insert any more information you'd like to see on the plot title here, like run numbers
    h1->SetTitle(Form("%s/%i ROFs/window size %i;#Delta#phi [rad];#Delta#eta", prefix.Data(), totalROFs, windowSize));

    TString title2 = "All pairs; |#Delta#phi| [rad]; |#Delta#eta|";
    h2->SetTitle(title2);
    TString title3 = "Duplicate candidates; #Delta#phi [rad]; #Delta#eta";
    h3->SetTitle(title3);

    gStyle->SetOptStat("ne");
    auto c1 = new TCanvas("c1", "Signed track difference histogram", 0, 0, 1200, 800);
    h1->SetStats(1);
    h1->Draw("lego");
    // h1->Draw("COLZ");

    auto c2 = new TCanvas("c2", "Unsigned track difference histogram", 0, 0, 1200, 800);
    h2->SetStats(0);
    // h1->Draw("lego");
    h2->Draw("COLZ");

    auto c3 = new TCanvas("c3", "", 0, 0, 1200, 800);
    h3->SetStats(1);
    h3->GetXaxis()->SetMaxDigits(1);
    h3->GetYaxis()->SetMaxDigits(1);
    h3->Draw("COLZ");

    auto c4 = new TCanvas("c4", "", 0, 0, 1200, 800);
    h4->SetStats(0);
    h4->Draw("COLZ");

    auto c5 = new TCanvas("c5", "Phi projection", 0, 0, 1200, 800);
    TH1D* h5 = h4->ProjectionX("pPhi");
    h5->SetTitle("Phi projection");
    c5->SetLogy();
    h5->SetStats(0);
    h5->Draw();
    
    auto c6 = new TCanvas("c6", "Phi projection", 0, 0, 1200, 800);
    TH1D* h6 = h4->ProjectionY("pEta");
    h6->SetTitle("Eta projection");
    c6->SetLogy();
    h6->SetStats(0);
    h6->Draw();


    TFile output(Form("saved_hists/IR_%i_proj%i_001.root", IR, windowSize), "RECREATE");
    h5->Write();
    h6->Write();
    output.Close();

    TFile *fout = TFile::Open(Form("saved_hists/IR_%i_proj%i_001.root", IR, windowSize), "RECREATE");
    double_t frac_comp = (double_t) dupl / (double_t) comps;
    double_t frac_track = (double_t) dupl * 2 / (double_t) totalTracks;
    std::vector<Double_t> frac{(double_t) dupl, totalTracks, frac_track, (double_t) comps, frac_comp}; // I create and fill the vector to be saved.
    LOGP(info, "Efficiencies: trk {} comp {}", frac_track, frac_comp);

    fout->WriteObject(&frac, Form("frac_%i", IR)); // I store the vector in the TFile
    fout->Close();

    

    TString filename = TString::Format("plots/IR_%i_%i_inclS_%i.pdf", IR, windowSize, includeSame);
    c1->Print(filename+"[");
    c1->Print(filename);
    c2->Print(filename);
    c3->Print(filename);
    c4->Print(filename);
    c5->Print(filename);
    c6->Print(filename);
    c6->Print(filename+"]");
    LOGP(info, "Summary:");
    LOGP(info, "Zeroes: {}; duplicates: {}; total comparisons: {}; total tracks: {}.", zeroes, dupl, comps, totalTracks);
}