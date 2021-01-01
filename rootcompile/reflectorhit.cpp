#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"
#include "TApplication.h"
#include <TObject.h>
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
using namespace std;


void reflectorhit(int argc, char** argv) {

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
	TBranch *branch = wcsimGeoT->GetBranch("wcsimrootgeom");
	branch->SetAddress(&wcsimrootgeom);

	wcsimGeoT->SetBranchAddress("wcsimrootgeom", &wcsimrootgeom);

	wcsimGeoT->GetEntry(0);
	int numpmt = wcsimrootgeom->GetWCNumPMT();
	cout << numpmt << endl;

		TCanvas *c1 = new TCanvas("c1", "c1",1000,1000);
       	c1 -> Divide(2,2);

      	TH2D* hist1 = new TH2D("CherenkovHits_barrelpart","Total Cherenkov hit", 1000, 0 ,329 , 1000, -329.0, +329.0);
       	TH2D* hist2 = new TH2D("reflectorhit","same reflectorid & tubeid - all hits ", 1000, 0 ,329 , 1000, -329.0, +329.0);
       	TH2D* hist3 = new TH2D("reflectorhit","different reflectorid and tubeid-all hits", 1000, 0, 329.0 , 1000, -329, +329);
	TH2D* hist4 = new TH2D("relfectorhit","first reflectorhit for plot3", 1000, 0 , 329.0 , 1000, -329.0, 329.0);
	
	wcsimT->GetEntry(0);
	for (int k=0; k<entries; k++)
	  {
	    wcsimT -> GetEntry(k);
	    WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
	    int max=wcsimroottrigger->GetNcherenkovhits();
	    for (int i = 0; i<max; i++)
	      {
		WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)
		  (wcsimroottrigger->GetCherenkovHits()->At(i)); 
		
		float pe_0 = chit -> GetTotalPe(0);
		float pe_1 = chit -> GetTotalPe(1);
		int  tubeid = chit -> GetTubeID();
		int  trackid = chit -> GetTrackID();
		const int ref = wcsimroottrigger->GetNreflectorhits();  
	       	for (int j=0; j<ref; j++)
	         {
		   
		   WCSimRootReflectorHit *rhit = (WCSimRootReflectorHit*)
		     (wcsimroottrigger->GetReflectorHits()->At(j));
		   int reflectorid = rhit -> GetReflectorID();
		   int reflectortrackid = rhit -> GetRefTrackID();
		   double x_ref = rhit->GetReflectorPos(0)*0.1;
		   double y_ref = rhit->GetReflectorPos(1)*0.1;
		   double z_ref = rhit->GetReflectorPos(2)*0.1;	  
		   double theta_r =atan(z_ref/x_ref)*180/3.14159;
		   double thetaa_r = 0;	 
		   if (z_ref<0 && x_ref>0) {
		     thetaa_r = theta_r+360;
		   }
		   else if (x_ref<0 && z_ref>0){
		     thetaa_r = theta_r+180;
		   }
		   else if (x_ref<0 && z_ref<0){
		     thetaa_r = theta_r + 180;
		   }
		   else if (x_ref>0 && z_ref>0){
		     thetaa_r = theta_r;
		   }							
		    int iHR = chit -> GetIsHitReflector();
		    WCSimRootPMT pmt = wcsimrootgeom ->GetPMT(tubeid-1);
		    double X = pmt.GetPosition(0);
		    double Y = pmt.GetPosition(1);
		    double Z = pmt.GetPosition(2);
		    double theta =atan(Z/X)*180/3.14159;
		    double thetaa = 0;
		    if (Z<0 && X>0) {
		      thetaa = theta+360;
		    }
		    else if (X<0 && Z>0){
		      thetaa = theta+180;
		    }
		    else if (X<0 && Z<0){
		      thetaa = theta + 180;
		    }
		    else if (X>0 && Z>0){
		      thetaa = theta;
		    }
		    int ch, ch1;
		    //=======================Cherenkovhit========================================//
		    
		    if (Y >= -280.0 && Y <= 280.0) //barrel
		      //  if (Y >= 280.0) //topcap
		      //if (Y <= -280.0) //bottom
		      {
			ch = trackid;
			if(j==0)
			  {
			    ch1 = ch;
			    double charge1 =chit -> GetTotalPe(1); 
			    hist1->Fill(thetaa, Y, charge1);
			  }
			if(ch1 != ch)
			  {
			    ch1 = ch;
			    double charge1 =chit -> GetTotalPe(1); 
			    hist1->Fill(thetaa, Y, charge1);
			  }
		      }
		    //=================reflectorhit direct light all counts=========================================//
		    
		    if (Y >= -280.0 && Y <= 280.0) //barrel
		      // if (Y >= 280.0) //top
		      //if (Y <= -280.0) //bottom
		      {
			if (reflectortrackid == trackid)
			  {
			    if (reflectorid == tubeid)
			      {
				double charge =chit -> GetTotalPe(1); 
				hist2->Fill(thetaa_r, y_ref, charge);
			      }
			  }
		      }
		    int temp, temp1;
		    
		    if (Y >= -280.0 && Y <= 280.0) //barrel
		    // if (Y >= 280.0) //top
		      //if (Y <= -280.0) //bottom
		      {
			if (reflectortrackid == trackid)
			  {
			    if (reflectorid != tubeid)
			      {
				temp = trackid;
				double charge =chit -> GetTotalPe(1); 
				hist3->Fill(thetaa_r, y_ref);
			      }
			    if(j==0)
			      {
				temp1=temp;
				double charge1 =chit -> GetTotalPe(1); 
				hist4->Fill(thetaa_r, y_ref, charge1);
			      }
			    if(temp1!=temp)
			      {
				temp1=temp;
				double charge1 =chit -> GetTotalPe(1); 
				hist4->Fill(thetaa_r, y_ref, charge1);
			      }
			  }
		      }
		    
		 }
		
	      }
	  }
	
	
	
	
	
	cout << "plotted cd 1" << endl;
	c1 -> cd(1);
	hist1-> SetXTitle("Theta");
	hist1->SetYTitle("Y coordinate in cm");
	hist1->Draw("colz");
		cout << "plotted cd 2" << endl;
	c1 -> cd(2);
	hist2-> SetXTitle("theta");
	hist2->SetYTitle("Y coordinate in cm");
	hist2->SetTitleOffset(1.5);
	hist2->Draw("colz");
	c1 -> cd(3);
	hist3-> SetXTitle("theta");
	hist3->SetYTitle("Y coordinate in cm");
	hist3->SetTitleOffset(1.5);
	hist3->Draw("colz");
        c1 -> cd(4);
	hist4-> SetXTitle("theta");
	hist4->SetYTitle("Y coordinate in cm");
	hist4->SetTitleOffset(1.5);
	hist4->Draw("colz");
	return;
}
int main(int argc, char** argv) {
  TApplication app("ROOT Application", &argc, argv);
  reflectorhit(app.Argc(), app.Argv());
  app.Run();
  
  // app.Terminate(); // still doesn't quit
  
  return 0;
}
