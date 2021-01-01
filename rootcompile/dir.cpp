#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"
#include <TObject.h>
#include "TApplication.h"
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TGraph2D.h>
#include <TEllipse.h>
#include <TLine.h>
#include <stdio.h>     
#include <stdlib.h>
#include <Riostream.h>
#include <iostream>
#include <TMath.h>
#include "Math/Vector3D.h"
//#include <Math/GenVector/Dot.h>
//#include <Math/GenVector/RotationY.h>
//#include <Math/GenVector/RotationZ.h>
#include "TGraph.h"
#include <math.h>

using namespace std;
using namespace ROOT::Math;

void dir(int argc, char** argv) {

	TFile *f;
	f = new TFile("/home/t2k/geant4/WCSim_geometry1final_build/photondir.root");

	TChain  *wcsimT = new TChain("wcsimT");

	wcsimT->Add("/home/t2k/geant4/WCSim_geometry1final_build/photondir.root");
       
	WCSimRootEvent *wcsimrootevent = new WCSimRootEvent();
	wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootevent);
	wcsimT -> GetBranch("wcsimrootevent") -> SetAutoDelete(kTRUE);

	int entries =  wcsimT -> GetEntries();
	TTree *wcsimGeoT = (TTree*) f->Get("wcsimGeoT");
	int nevent = wcsimGeoT->GetEntries();

	WCSimRootGeom *wcsimrootgeom = new WCSimRootGeom();
	TBranch *branch1 = wcsimGeoT->GetBranch("wcsimrootgeom");
	branch1->SetAddress(&wcsimrootgeom);

	wcsimGeoT->SetBranchAddress("wcsimrootgeom", &wcsimrootgeom);

	wcsimGeoT->GetEntry(0);
	int numpmt = wcsimrootgeom->GetWCNumPMT();
	//cout << numpmt << endl;
	// WCSimRootEvent* wcsimrootsuperevent = new WCSimRootEvent();
	// TBranch *branch = wcsimT->GetBranch("wcsimrootevent");
	// branch->SetAddress(&wcsimrootsuperevent);
	// wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
  
		TCanvas *c1 = new TCanvas("c1", "c1",1000,1000);
       	c1 -> Divide(2,2);
	TCanvas *c2 = new TCanvas("c2", "c2",1000,1000);
       	c2 -> Divide(2,2);
      	TH1D* hist1 = new TH1D("Events", "DirectionX", 100, -1.5 ,1.5);
       	TH1D* hist2 = new TH1D("Events","DirectionY",100, -1.5, 1.5);
       	TH1D* hist3 = new TH1D("Events","DirectionZ",100, -1.5 ,1.5 );
	TH1D* hist4 = new TH1D("Events", "E-DirectionX", 100, -1.5 ,1.5);
       	TH1D* hist5 = new TH1D("Events","E-DirectionY",100, -1.5, 1.5);
       	TH1D* hist6 = new TH1D("Events","E-DirectionZ",100, -1.5 ,1.5 );
	//TH2D* hist4 = new TH2D("totalhit","timing distribution for cherenkov hit",100, 0 ,350, 12000, 0, 12000 );
	
	wcsimT->GetEntry(0);
	for (int k=0; k<entries; k++)
	  {
	    wcsimT -> GetEntry(k);
	    WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
	    int max=wcsimroottrigger->GetNcherenkovhits();
	     int ntrack = wcsimroottrigger->GetNtrack();
	      XYZVector pvector[max];
	  XYZVector evector[ntrack];
	    cout<<" "<<max<<endl;
	     for (int j=0; j<1; j++)
     {
   	      TObject *element = (wcsimroottrigger->GetTracks())->At(j);
	  
   	  WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
       double xdir = wcsimroottrack->GetDir(0);
      double ydir = wcsimroottrack->GetDir(1);
   	  double zdir = wcsimroottrack->GetDir(2);
	 evector[j].SetCoordinates(xdir, ydir, zdir);
     	 hist4->Fill(xdir);
      hist5->Fill(ydir);
   	  hist6->Fill(zdir);
	    for (int i = 0; i<max; i++)
	      {
	    	WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)
	    	  (wcsimroottrigger->GetCherenkovHits()->At(i)); 
		
	    	float pe_0 = chit -> GetTotalPe(0);
	    	float pe_1 = chit -> GetTotalPe(1);
	    	int  tubeid = chit -> GetTubeID();
	    	int  trackid = chit -> GetTrackID();
		
	    	//for (int b=pe_0; b< (pe_0 + pe_1); b++)
	      	//{
	    	WCSimRootCherenkovHitTime *hittime = (WCSimRootCherenkovHitTime*)
	    	  (wcsimroottrigger->GetCherenkovHitTimes()->At(i));
	    	  float time = hittime->GetTruetime();
	    	  double directionx = hittime->GetPhotonDir(0);
	    	  float directiony = hittime->GetPhotonDir(1);
	    	   float directionz = hittime->GetPhotonDir(2);
		   // float length1 = sqrt( directionx* directionx + directiony*directiony +directionz*directionz );
		   //float length2 = sqrt(xdir*xdir + ydir*ydir + zdir* zdir);
		   //float dot = directionx*xdir + directiony*ydir + directionz*zdir;
		   //float a = acos(dot/(length1*length2));
		   // cout<<" "<<a<<endl;
		   pvector[i].SetCoordinates(directionx, directiony, directionz);
	    	    hist1->Fill(directionx);
	    	    hist2->Fill(directiony);
	    	    hist3->Fill(directionz);
		    double_t s = acos((evector[j].Dot(pvector[i])));
		    double ss = s*180/3.14;
		    cout<<" "<<ss<<endl;
		    //cout<<" "<<directiony<<" "<<time<<endl;
		    //hist4->Fill(ss);
	      }
	   //  WCSimRootTrigger* wcsimrootevent= wcsimrootsuperevent->GetTrigger(0);
         
    	      

	    //	 cout<<" "<<s<<endl;
	  
    	  
	      }
	// TTree *tree = (TTree*)file->Get("wcsimT");
	//  int nevent = tree->GetEntries();
	   
		     
	  }	
   	
	
	
	
	cout << "plotted cd 1" << endl;
	c1 -> cd(1);
	hist1-> SetXTitle("Theta");
	//	hist1->SetYTitle("Tube #");
	hist1->Draw("colz");
		cout << "plotted cd 2" << endl;
	c1 -> cd(2);
	hist2-> SetXTitle("DirY");
	//	hist2->SetYTitle("Tube #");
	hist2->SetTitleOffset(1.5);
	hist2->Draw("colz");
	c1 -> cd(3);
	hist3-> SetXTitle("DirZ");
	//	hist3->SetYTitle("Tube #");
	hist3->SetTitleOffset(1.5);
	hist3->Draw("colz");

	c2 -> cd(1);
	hist4-> SetXTitle("Theta");
	//	hist1->SetYTitle("Tube #");
	hist4->Draw("colz");
	//	cout << "plotted cd 4" << endl;
	c2 -> cd(2);
	hist5-> SetXTitle("DirY");
	//	hist2->SetYTitle("Tube #");
	hist5->SetTitleOffset(1.5);
	hist5->Draw("colz");
	c2 -> cd(3);
	hist6-> SetXTitle("DirZ");
	//	hist3->SetYTitle("Tube #");
	hist6->SetTitleOffset(1.5);
	hist6->Draw("colz");
        // c1 -> cd(4);
	// hist4-> SetXTitle("time");
	// hist4->SetYTitle("Tube #");
	// hist4->SetTitleOffset(1.5);
	// hist4->Draw("colz")
	//return;
}
int main(int argc, char** argv) {
TApplication app("ROOT Application", &argc, argv);
dir(app.Argc(), app.Argv());
app.Run();
  
 app.Terminate(); // still doesn't quit
  
 return 0;
}
