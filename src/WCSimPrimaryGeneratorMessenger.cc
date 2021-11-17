#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4ios.hh"

WCSimPrimaryGeneratorMessenger::WCSimPrimaryGeneratorMessenger(WCSimPrimaryGeneratorAction* pointerToAction)
:myAction(pointerToAction)
{
  mydetDirectory = new G4UIdirectory("/mygen/");
  mydetDirectory->SetGuidance("WCSim detector control commands.");

  genCmd = new G4UIcmdWithAString("/mygen/generator",this);
  genCmd->SetGuidance("Select primary generator.");

  genCmd->SetGuidance(" Available generators : muline, gun, laser, gps, rootracker, radon, injector, niball");
  genCmd->SetParameterName("generator",true);
  genCmd->SetDefaultValue("muline");
  genCmd->SetCandidates("muline gun laser gps rootracker radon injector niball");

  fileNameCmd = new G4UIcmdWithAString("/mygen/vecfile",this);
  fileNameCmd->SetGuidance("Select the file of vectors.");
  fileNameCmd->SetGuidance(" Enter the file name of the vector file");
  fileNameCmd->SetParameterName("fileName",true);
  fileNameCmd->SetDefaultValue("inputvectorfile");

  genSet = false;

  //C. Vilela: Adding PMTPoisson for generating photoelectrons directly on PMTs according to a Poisson distribution.
  poisCmd = new G4UIcmdWithABool("/mygen/pmtPoisson",this);
  poisCmd->SetGuidance("Flag for generating photoelectrons directly on PMTs according to a Poisson distribution. These PE's will be generated in addition to light produce by any particles generated. Set dark rate to 0 and do not generate any particles for events with only Poisson PE's.");
  poisCmd->SetGuidance("Set poisson mean with /mygen/poissonMean");
  poisCmd->SetParameterName("pmtPoisson", true);

  poisMeanCmd = new G4UIcmdWithADouble("/mygen/poissonMean",this);
  poisMeanCmd->SetGuidance("Set Poisson mean to be used with /mygen/pmtPoisson. Defaults to 1.");
  poisMeanCmd->SetParameterName("poissonMean", true);
  poisMeanCmd->SetDefaultValue(1);


  niball_x_Cmd = new G4UIcmdWithADouble("/mygen/niball_x",this);
  niball_x_Cmd->SetGuidance("Select X position for Nickel Ball");
  niball_x_Cmd->SetParameterName("niball_x",true);
  niball_x_Cmd->SetDefaultValue(0.0);
  niball_y_Cmd = new G4UIcmdWithADouble("/mygen/niball_y",this);
  niball_y_Cmd->SetGuidance("Select Y position for Nickel Ball");
  niball_y_Cmd->SetParameterName("niball_y",true);
  niball_y_Cmd->SetDefaultValue(0.0);
  niball_z_Cmd = new G4UIcmdWithADouble("/mygen/niball_z",this);
  niball_z_Cmd->SetGuidance("Select Z position for Nickel Ball");
  niball_z_Cmd->SetParameterName("niball_z",true);
  niball_z_Cmd->SetDefaultValue(0.0);
  niball_spectrumCmd = new G4UIcmdWithAString("/mygen/niball_spectrum",this);
  niball_spectrumCmd->SetGuidance("Select the file of vectors.");
  niball_spectrumCmd->SetGuidance(" Enter the file name of the vector file");
  niball_spectrumCmd->SetParameterName("niball_spectrum",true);
  niball_spectrumCmd->SetDefaultValue("");

  radioactive_time_window_Cmd = new G4UIcmdWithADouble("/mygen/radioactive_time_window",this);
  radioactive_time_window_Cmd->SetGuidance("Select time window for radioactivity");
  radioactive_time_window_Cmd->SetParameterName("radioactive_time_window",true);
  radioactive_time_window_Cmd->SetDefaultValue(0.);

  // K.M.Tsui: options for injector events
  nPhotonsCmd = new G4UIcmdWithAnInteger("/mygen/injector_nPhotons",this);
  nPhotonsCmd->SetGuidance("Number of photons emitted for each injector event");
  nPhotonsCmd->SetParameterName("injector_nPhotons",true);
  nPhotonsCmd->SetDefaultValue(1);

  injectorOnCmd = new G4UIcmdWithAnInteger("/mygen/injector_on_index",this);
  injectorOnCmd->SetGuidance("Index of the injector to be turned on");
  injectorOnCmd->SetParameterName("injector_on_index",true);
  injectorOnCmd->SetDefaultValue(0.);

  // not really implemented yet, just a placeholder
  injectorTimeCmd = new G4UIcmdWithADouble("/mygen/injector_time_width",this);
  injectorTimeCmd->SetGuidance("Injector time width");
  injectorTimeCmd->SetParameterName("injector_time_width",true);
  injectorTimeCmd->SetDefaultValue(0.);

  openingAngleCmd = new G4UIcmdWithADouble("/mygen/injector_opening_angle",this);
  openingAngleCmd->SetGuidance("Opening angle of light injector in deg");
  openingAngleCmd->SetParameterName("injector_opening_angle",true);
  openingAngleCmd->SetDefaultValue(0.);

  injectorWavelengthCmd = new G4UIcmdWithADouble("/mygen/injector_wavelength",this);
  injectorWavelengthCmd->SetGuidance("Wavelength of the injector laser in nm");
  injectorWavelengthCmd->SetParameterName("injector_wavelength",true);
  injectorWavelengthCmd->SetDefaultValue(435.);
    
  G4UIparameter* param;
  
  radonScalingCmd = new G4UIcmdWithAString("/mygen/radon_scaling",this);
  radonScalingCmd->SetGuidance("Select scalling scenario, if scenario 0 is selected, Bi214 are generated uniformly");
  radonScalingCmd->SetGuidance("[usage] /mygen/radon SCENARIO ");
  radonScalingCmd->SetGuidance("     SCENARIO : 0, A, B");
  radonScalingCmd->SetCandidates("0 A B");
  param = new G4UIparameter("SCENARIO",'s',true);
  param->SetDefaultValue("B");
  radonScalingCmd->SetParameter(param);
  
  radonGeoSymCmd = new G4UIcmdWithAnInteger("/mygen/radon_symmetry",this);
  radonGeoSymCmd->SetGuidance("Select scalling scenario");
  radonGeoSymCmd->SetGuidance("[usage] /mygen/radon SCENARIO ");
  radonGeoSymCmd->SetGuidance("     SYMMETRY : 1 ... ");
  param = new G4UIparameter("SYMMETRY",'d',true);
  param->SetDefaultValue("1");
  radonScalingCmd->SetParameter(param);

}

WCSimPrimaryGeneratorMessenger::~WCSimPrimaryGeneratorMessenger()
{
  delete genCmd;
  delete mydetDirectory;
  delete radonScalingCmd;
  delete radonGeoSymCmd;
  delete radioactive_time_window_Cmd;
  delete nPhotonsCmd;
  delete injectorOnCmd;
  delete injectorTimeCmd;
  delete openingAngleCmd;
  delete injectorWavelengthCmd;
}

void WCSimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command==genCmd )
  {
    genSet = true;
    if (newValue == "muline")
    {
      myAction->SetMulineEvtGenerator(true);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false
    }
    else if ( newValue == "gun")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(true);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false
    }
    else if ( newValue == "rootracker")   //M. Scott: Addition of Rootracker events
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(true);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false
    }
    else if ( newValue == "laser")   //T. Akiri: Addition of laser
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(true);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
    }
    else if ( newValue == "injector")   // addition of injector events
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(true);
    }
    else if ( newValue == "gps")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(true);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
    }
    else if ( newValue == "radon" ) //G. Pronost: Addition of Radon generator (based on F. Nova's radioactive generator but dedicated to radioactive events in water)
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(false);
      myAction->SetRadonEvtGenerator(true);
      myAction->SetInjectorEvtGenerator(false);
    }
    else if ( newValue == "niball" ) // Pablo: Addition of Ni Ball gammas generator
{
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetNiBallEvtGenerator(true);
      myAction->SetRadonEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
    }

  }

  if( command == fileNameCmd )
  {
    if(genSet){
        if(myAction->IsUsingRootrackerEvtGenerator()){
            myAction->OpenRootrackerFile(newValue);
        }
        else{
            myAction->OpenVectorFile(newValue);
        }
        G4cout << "Input vector file set to " << newValue << G4endl;
    }
    else{
        G4cout << "Generator has not been set, guessing input vector file is NOT in the Rootracker format - this will crash if you are using a Rootracker input file" << G4endl;
        G4cout << "Please put the '/mygen/generator' command above the '/mygen/vecfile' command in the mac file." << G4endl;
    }
  }

  if( command == poisCmd )
    {
      if ( poisCmd->GetNewBoolValue(newValue) ){
	myAction->SetPoissonPMT(true);
	G4cout << "Running with PoissonPMT flag. Photoelectrons will be generated directly on the PMTs according to a Poisson distribuition. Any hits resulting from physics generated elsewhere will be discarded !!!" << G4endl;
      }
      else myAction->SetPoissonPMT(false);
    }
  
  if( command == poisMeanCmd )
    {
      myAction->SetPoissonPMTMean(poisMeanCmd->GetNewDoubleValue(newValue));
      G4cout << "PoissonPMT mean set to: " << poisMeanCmd->GetNewDoubleValue(newValue) << G4endl;
    }

  if ( command==niball_x_Cmd )
    {
      myAction->SetNiBallX(StoD(newValue));
    }
  if ( command==niball_y_Cmd )
    {
      myAction->SetNiBallY(StoD(newValue));
    }
  if ( command==niball_z_Cmd )
    {
      myAction->SetNiBallZ(StoD(newValue));
    }
  if ( command==niball_spectrumCmd )
    {
      myAction->SetNiBallSpectrum(newValue);

    
  if( command==radioactive_time_window_Cmd )
    {
      myAction->SetRadioactiveTimeWindow(StoD(newValue));
    }
  
  if ( command==radonScalingCmd ) 
    {
      RadonScalingCommand(newValue);
    }
  
  if ( command==radonGeoSymCmd ) 
    {
      myAction->SetRadonSymmetry(radonGeoSymCmd->GetNewIntValue(newValue));
    }
  
  if ( command==nPhotonsCmd ) 
    {
      myAction->SetInjectorBeamPhotons(nPhotonsCmd->GetNewIntValue(newValue));
    }

  if ( command==injectorOnCmd ) 
    {
      myAction->SetInjectorOnIdx(injectorOnCmd->GetNewIntValue(newValue));
    }
  
  if ( command==injectorTimeCmd ) 
    {
      myAction->SetInjectorTimeWindow(injectorTimeCmd->GetNewDoubleValue(newValue));
    }

  if ( command==openingAngleCmd ) 
    {
      myAction->SetInjectorOpeningAngle(openingAngleCmd->GetNewDoubleValue(newValue));
    }

  if ( command== injectorWavelengthCmd )
    {
      myAction->SetInjectorWavelength(injectorWavelengthCmd->GetNewDoubleValue(newValue));
    }

}

G4String WCSimPrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4String cv;
  
  if( command==genCmd )
  {
    if(myAction->IsUsingMulineEvtGenerator())
      { cv = "muline"; }
    else if(myAction->IsUsingGunEvtGenerator())
      { cv = "gun"; }
    else if(myAction->IsUsingLaserEvtGenerator())
      { cv = "laser"; }   //T. Akiri: Addition of laser
    else if(myAction->IsUsingInjectorEvtGenerator())
      { cv = "injector"; }   
    else if(myAction->IsUsingGPSEvtGenerator())
      { cv = "gps"; }
    else if(myAction->IsUsingRootrackerEvtGenerator())
      { cv = "rootracker"; }   //M. Scott: Addition of Rootracker events
    else if(myAction->IsUsingNiBallEvtGenerator())
      { cv = "niball"; } // Pablo: Addition of Ni ball generator
    else if(myAction->IsUsingRadonEvtGenerator())
      { cv = "radon"; } // G. Pronost: Addition of Radon generator
  }
  
  return cv;
}

void WCSimPrimaryGeneratorMessenger::RadonScalingCommand(G4String newValue)
{
  G4Tokenizer next( newValue );

  G4String scenario = next();
  G4int iScenario = 0;
   
  if ( scenario == "A" ) iScenario = 1; // Relative scaling with respect to full ID volume (Pessimistic)
  if ( scenario == "B" ) iScenario = 2; // Relative scaling with respect to fiducial volume
   
  myAction->SetRadonScenario(iScenario);
}


