#include "MJDNeutronInducedIsotope.hh"
#include "TStyle.h"
#include "TFile.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>

using namespace std;

int main(int argc, char** argv)
{
  if(argc != 2 || atoi(argv[1]) == 0) {
    cout << "Usage: " << argv[0] << " [run]" << endl;
    return 1;
  }

  Int_t fRun = atoi(argv[1]);
 
  MJDNeutronInducedIsotope ds(fRun);
  //ds.SetEnergyName(fEnergyName);
  string fDataSet = ds.GetDataSet();
  vector<Int_t> fChannel = ds.GetChannel();
  vector<Int_t> fCryo = ds.GetCryo();
  vector<Int_t> fStr = ds.GetString();
  vector<Int_t> fDetpos = ds.GetDetPosition();

  Double_t fEnr1 = 66.7;
  Double_t fEnr2 = 10000;
  Double_t fTime = 0.5;
  string fOutputFile = Form("data_%d.txt",fRun);
  /*
  ds.SearchDelayedEvent(fEnr1,fEnr2, fTime, fOutputFile);

  ifstream fin(Form("%s",fOutputFile.c_str()));

  Int_t run,entry1,entry2,channel1,channel2;
  Double_t enr1,time1,enr2,time2;
  vector<Int_t> Entry1;
  vector<Int_t> Entry2;
  vector<Int_t> Channel1;
  vector<Int_t> Channel2;
  vector<Double_t> Enr1;
  vector<Double_t> Enr2;
  vector<Double_t> Time1;
  vector<Double_t> Time2;

  if(fin.is_open()){
    while(!fin.eof()){
      fin >> run >> entry1 >> channel1 >> enr1 >> time1 >> entry2 >> channel2 >> enr2 >> time2;
      Entry1.push_back(entry1);
      Entry2.push_back(entry2);
      Channel1.push_back(channel1);
      Channel2.push_back(channel2);
      Enr1.push_back(enr1);
      Enr2.push_back(enr2);
      Time1.push_back(time1);
      Time2.push_back(time2);  

    }
  }

  Entry1.pop_back();
  Entry2.pop_back();
  Channel1.pop_back();
  Channel2.pop_back();
  Enr1.pop_back();
  Enr2.pop_back();
  Time1.pop_back();
  Time2.pop_back();
  */

  ds.SearchEnergyEvent(fEnr1,10.,fOutputFile);

  ifstream fin(Form("%s",fOutputFile.c_str()));

  Int_t run,entry1,channel1;
  Double_t enr1;
  vector<Int_t> Entry1;
  vector<Int_t> Channel1;
  vector<Double_t> Enr1;

  if(fin.is_open()){
    while(!fin.eof()){
      fin >> run >> entry1 >> channel1 >> enr1;
      Entry1.push_back(entry1);
      Channel1.push_back(channel1);
      Enr1.push_back(enr1);
    }
  }
  if(Entry1.size()>0){
    Entry1.pop_back();
    Channel1.pop_back();
    Enr1.pop_back();
  }
  
  if(Entry1.size()>0){
    vector<Double_t> DEntry1;
    for(size_t i =0;i<Entry1.size();i++){
      DEntry1.push_back((Double_t)Entry1.at(i));
    }
    
    string fOutputWaveform = Form("waveform_%d.root",fRun);
    TFile fhist(Form("%s",fOutputWaveform.c_str()),"recreate");
    ofstream fout(Form("wf_%d.txt",fRun));
    vector<Int_t> index1 = ds.Clean(DEntry1);
    vector<Double_t> xp;
    vector<Double_t> yp;
    vector<Double_t> xp1;
    vector<Double_t> yp1;
    vector<Double_t> xp2;
    vector<Double_t> yp2;
    
    Double_t Xmin = 4000;
    Double_t Xmax = 19000;
    Double_t fResolution = 1;
    Double_t fThreshold = 0.01;
    Double_t fSigma = 5;
    
    for(size_t i=0;i<index1.size();i++){
      xp.clear();
      yp.clear();
      xp1.clear();
      yp1.clear();
      xp2.clear();
      yp2.clear();
      
      Int_t ii = index1.at(i);    
      TH1D* h = ds.GetWaveform(fRun,Entry1.at(ii),Channel1.at(ii),Enr1.at(ii));
      TH1D* h1 = ds.GetHistoSmooth(h,10);
      TH1D* h2 = ds.GetHistoDerivative(h1,10);
      TH1D* h3 = ds.GetHistoSmooth(h2,10);
      TH1D* h4 = ds.GetHistoDerivative(h3,10);
      TH1D* h5 = ds.GetHistoSmooth(h4,10);
      TH1*  hFFT = ds.GetHistoFFT(h);
      // Cut1
      Double_t maxFFT = ds.GetMax(hFFT, 50.,150.); 
      //cout << max << endl;
      // Cut2 
      Double_t A = ds.GetMax(h3, Xmin,Xmax);
      Double_t AoverE = A/Enr1.at(ii);
      
      if(maxFFT<2000 && AoverE>0.004){
	Int_t nPeak1 = ds.FindPeaks(h3, Xmin, Xmax,fResolution,fSigma, fThreshold, &xp, &yp);
	if(nPeak1>1){
	  for(Int_t ip = 0;ip<(Int_t)xp.size();ip++){
	    
	    Double_t y = ds.GetYValue(h5, xp.at(ip));
	    if(abs(y)<2e-4){
	      xp1.push_back(xp.at(ip));
	      yp1.push_back(yp.at(ip));
	    }
	  }
	  
	  if(xp1.size()>1){
	    h3->SetName(Form("waveform1_%d_%d_%d",fRun,Entry1.at(ii),Channel1.at(ii)));
	    h5->SetName(Form("waveform2_%d_%d_%d",fRun,Entry1.at(ii),Channel1.at(ii)));
	    hFFT->SetName(Form("waveformFFT_%d_%d_%d",fRun,Entry1.at(ii),Channel1.at(ii)));
	    //h->Write();
	    h1->Write();
	    h3->GetXaxis()->SetRangeUser(0,20000);
	    h3->Write();
	    h5->Write();
	    hFFT->GetXaxis()->SetRangeUser(0,1000);
	    hFFT->Write();
	    
	    vector<Int_t> Index2 = ds.Sort(yp1);
	    for(size_t ip1 = Index2.size()-2;ip1<Index2.size();ip1++){
	      Int_t ii = Index2.at(ip1);
	      xp2.push_back(xp1.at(ii));
	      yp2.push_back(yp1.at(ii));
	    }
	    fout << fRun << " " << Entry1.at(ii) << " " << Channel1.at(ii) << " "<< Enr1.at(ii)<< " " << yp2.at(1)/yp2.at(0) << " " << xp2.at(0)-xp2.at(1) << " " << maxFFT << " " << AoverE << endl;
	    
	  }
	}
      }
      
      delete h;
      delete h1;
      delete h2;
      delete h3;
      delete h4;
      delete h5;
      delete hFFT;
    }
    fhist.Close();
  }else{
    cout << "No events found in the energy window!" << endl;
  }
}
