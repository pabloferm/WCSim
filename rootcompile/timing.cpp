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
using namespace std;


void timing(int argc, char** argv) {

 

	TFile *f;
	f = new TFile("/home/t2k/geant4/WCSim_geometry2final_build/1_geometry2.root");

	TChain  *wcsimT = new TChain("wcsimT");

	wcsimT->Add("/home/t2k/geant4/WCSim_geometry2final_build/1_geometry2.root");
       
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

      	TH1D* hist1 = new TH1D("reflectorhit_barrelpart","timing imformation for reflector hit", 100, 0 ,350);
       	TH1D* hist2 = new TH1D("reflectorhit","Direct light timing distribution",100, 0 ,350);
       	TH1D* hist3 = new TH1D("reflectorhit","Indirect light timing distribution",100, 0 ,350 );
	TH2D* hist4 = new TH2D("totalhit","timing distribution for cherenkov hit",100, 0 ,350, 12000, 0, 12000 );
	
	wcsimT->GetEntry(0);
	for (int k=0; k<entries; k++)
	  {
	    wcsimT -> GetEntry(k);
	    WCSimRootTrigger *wcsimroottrigger = wcsimrootevent->GetTrigger(0);
	    int max=wcsimroottrigger->GetNcherenkovhits();
	    cout<<" "<<max<<endl;
	    for (int i = 0; i<max; i++)
	      {
		WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)
		  (wcsimroottrigger->GetCherenkovHits()->At(i)); 
		
		float pe_0 = chit -> GetTotalPe(0);
		float pe_1 = chit -> GetTotalPe(1);
		int  tubeid = chit -> GetTubeID();
		int  trackid = chit -> GetTrackID();
		
		//for (int b=pe_0; b< (pe_0 + pe_1); b++)
		//	{
		WCSimRootCherenkovHitTime *hittime = (WCSimRootCherenkovHitTime*)
		  (wcsimroottrigger->GetCherenkovHitTimes()->At(i));
		  float time = hittime->GetTruetime();
		  float directionx = hittime->GetPhotonDir(0);
		  float directiony = hittime->GetPhotonDir(1);
		   float directionz = hittime->GetPhotonDir(2);
		    hist1->Fill(directionx);
		   cout<<" "<<directiony<<" "<<time<<endl;
		  // hist4->Fill(time, tubeid);
		
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

		    //cout<<"ref == "<<ref<<endl;
									
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
		    
		    // if (Y >= -280.0 && Y <= 280.0) //barrel
		    //  if (Y >= 280.0) //topcap
		      //if (Y <= -280.0) //bottom
			 // {
		    	if (reflectortrackid == trackid)
			  {
			    float time = hittime->GetTruetime();
			    //double charge =chit -> GetTotalPe(1); 
			   
			  }  
			
			// }
		    // if (Y >= -280.0 && Y <= 280.0) //barrel
		    // if (Y >= 280.0) //top
		      //if (Y <= -280.0) //bottom
		      //{
			if (reflectortrackid == trackid)
			  {
			    if (reflectorid == tubeid)
			      {
				float time = hittime->GetTruetime();
				double charge =chit -> GetTotalPe(1);
				hist2->Fill(time);
				//cout<< ""<<arrayrefid[j]<<" "<<tubeid<<endl;
			      }
			  }
			// }
		    int temp, temp1;
		    
		    //if (Y >= -280.0 && Y <= 280.0) //barrel
		    // if (Y >= 280.0) //top
		      //if (Y <= -280.0) //bottom
			// {
			if (reflectortrackid == trackid)
			  {
			    if (reflectorid != tubeid)
			      {
				temp = trackid;
				float time = hittime->GetTruetime();
				// double charge =chit -> GetTotalPe(1); 
				 hist3->Fill(time);
			      }
			   	// if(j==0)
				//   {
				//     temp1=temp;
				//     float time = hittime->GetTruetime();
				//     // double charge1 =chit -> GetTotalPe(1); 
				//     hist4->Fill(time, tubeid);
				//   }
				// 	if(temp1!=temp)
				//   {
				//     temp1=temp;
				//     float time = hittime->GetTruetime();
				//     //double charge1 =chit -> GetTotalPe(1); 
				//     hist4->Fill(time, tubeid);
				//     //	cout<< ""<<arrayrefid[j]<<" "<<tubeid<<endl;
				//   }
			  }
			// }
		 }
	      	}   
	   
	    // }
	  }
	
	
	
	
	
	cout << "plotted cd 1" << endl;
	c1 -> cd(1);
	hist1-> SetXTitle("Time");
	//	hist1->SetYTitle("Tube #");
	hist1->Draw("colz");
		cout << "plotted cd 2" << endl;
	c1 -> cd(2);
	hist2-> SetXTitle("time");
	//	hist2->SetYTitle("Tube #");
	hist2->SetTitleOffset(1.5);
	hist2->Draw("colz");
	c1 -> cd(3);
	hist3-> SetXTitle("time");
	//	hist3->SetYTitle("Tube #");
	hist3->SetTitleOffset(1.5);
	hist3->Draw("colz");
        // c1 -> cd(4);
	// hist4-> SetXTitle("time");
	// hist4->SetYTitle("Tube #");
	// hist4->SetTitleOffset(1.5);
	// hist4->Draw("colz");
	return;
}
int main(int argc, char** argv) {
  TApplication app("ROOT Application", &argc, argv);
  timing(app.Argc(), app.Argv());
  app.Run();
  
  // app.Terminate(); // still doesn't quit
  
  return 0;
}
