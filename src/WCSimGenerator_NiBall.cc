/*********************************************************************************/
/**     WCSimGenerator_NiBall                                                   **/
/**     Author: Pablo                                                           **/
/**     Date: March 28th 2021                                                   **/
/**     Desc: Get the energy, mode, branching ration and multiplicity of gammas **/
/**     produced by Ni+n-->Ni+gamma reaction of the 252Cf+Ni calibration source **/
/*********************************************************************************/

#include "WCSimGenerator_NiBall.hh"
#include <array>

// Setting parameters

std::vector<double> WCSimGenerator_NiBall::NiBallBR; // Cumulative branching ratio for the Ni ball source
std::vector<int>    WCSimGenerator_NiBall::NiBallMulti; // Multiplicities for each mode
std::vector<std::vector<double>> WCSimGenerator_NiBall::NiBallEnergy; // Gamma energies for each mode

WCSimGenerator_NiBall::WCSimGenerator_NiBall(WCSimDetectorConstruction* myDC) {
        myDetector = myDC;
//        this->Initialize(niball_spectrum);
}

WCSimGenerator_NiBall::~WCSimGenerator_NiBall() {
}

void WCSimGenerator_NiBall::Initialize(G4String file) {
//	std::ifstream input( "NiSpectrum.dat" );
	std::ifstream input( file );
	std::cout<<"Entering NiBall Init"<<std::endl;
	int modes;
	int num;
	double br, energy;
	int i = 0;
	int i0 = 0;
	std::vector<double> Energy;
	for( std::string eachLine; getline( input, eachLine ); )
	{
        std::istringstream strm(eachLine);
        std::string splitedLines;
        while ( strm >> splitedLines )
        {
        	std::stringstream geek(splitedLines);
            	if (i==1){
                    geek >>modes;
            	}
            	else if (i==3){
                    geek >>br;
                    NiBallBR.push_back (br);
          	 }
            	else if (i==5){
                    geek >>num;
                    NiBallMulti.push_back (num);
            	}
            	else if (i>6){
                    if (i>i0 && i>7){
                            NiBallEnergy.push_back (Energy);
                            Energy.clear();
                            i0 = i;
                    }
                    geek >>energy;
                    Energy.push_back (energy);
            	}
            }
        i=i+1;
        }
        NiBallEnergy.push_back (Energy);
	if (modes!=NiBallBR.size()) std::cout<<"WARNING: Quoted and actual number of Ni-CF source modes is not the same, please check."<<std::endl;

}




void WCSimGenerator_NiBall::SettingNiBall(double fNiBallPosition[3], double rn[4]) {
	int i = 0;
	while (rn[0] > NiBallBR[i]) i++;
	fNiGammaMode = i;
	fNiGammaMultiplicity = NiBallMulti[fNiGammaMode];
	for (int j=0; j<4; j++) fNiGammaEnergy[j] = NiBallEnergy[j][fNiGammaMode];
        double radius, theta, z; // Match the geometry of Ni ball (taken from SK)
        radius = rn[1] * 2.5;//in cm
        theta  = rn[2] * 2.0*3.14159265;
        z      = rn[3] * 12.0 + 5.0;//in cm
        fNiGammaPosition[0] = fNiBallPosition[0] + radius*cos(theta);
        fNiGammaPosition[1] = fNiBallPosition[1] + radius*sin(theta);
        fNiGammaPosition[2] = fNiBallPosition[2] + z - 8.5;

}

