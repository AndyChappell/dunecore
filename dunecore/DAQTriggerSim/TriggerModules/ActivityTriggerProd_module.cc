////////////////////////////////////////////////////////////////////////
// Class:       ActivityTriggerProd
// Module Type: producer
// File:        ActivityTriggerProd_module.cc
//
// Generated at Tuesday March 29 2016 by Michael Baird using the old
// copy and paste method...
//
// Modified by Karl Warburton May 9th 2017 to reflect the move away
//  services (for better or worse...)
// --- contact information --- karlwarb@iastate.edu
//
////////////////////////////////////////////////////////////////////////

// C++ includes

// ROOT includes
#include "TH1F.h"
#include "TTree.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//// Added so that I can compile without having it in ActivityTrigger.h
#include "lardataobj/RawData/OpDetWaveform.h"

// DUNETPC specific includes
#include "dune/DAQTriggerSim/TriggerDataProducts/TriggerTypes.h"
#include "dune/DAQTriggerSim/TriggerDataProducts/BasicTrigger.h"

// The trigger classes which you're including
#include "dune/DAQTriggerSim/TriggerAlgorithms/ActivityTrigger.h"

namespace triggersim { // Declare that we are working in the triggersim namespace

  class ActivityTriggerProd : public art::EDProducer {

  public:

    explicit ActivityTriggerProd(fhicl::ParameterSet const & pset);

    // Plugins should not be copied or assigned.
    ActivityTriggerProd(ActivityTriggerProd const &) = delete;
    ActivityTriggerProd(ActivityTriggerProd &&) = delete;
    ActivityTriggerProd & operator = (ActivityTriggerProd const &) = delete;
    ActivityTriggerProd & operator = (ActivityTriggerProd &&) = delete;

    // The main guts...
    void produce(art::Event& event) override;

    void reconfigure(fhicl::ParameterSet const & pset);

    // If you want to another function...
    // void YourFunction();

    // Declare our classes as data members...
    ActivityTrigger TempTrig;

  private:

    // declare fcl input variables here
    std::string fAString;
    std::string fRawDigitLabel;
    std::string fOpDetWaveLabel;
    // Your fcl params to send to the trigger functions...  
  };

  //......................................................
  ActivityTriggerProd::ActivityTriggerProd(fhicl::ParameterSet const & pset):
    TempTrig()
  {
    // --- Declare what this module is puttting in the art event.
    produces< std::vector<triggersim::BasicTrigger> >();
    // --- Configure my trigger module.
    this->reconfigure(pset);
    // --- Configure my trigger class.
    TempTrig.Configure( pset );
  }



  //......................................................
  void ActivityTriggerProd::reconfigure(fhicl::ParameterSet const & pset)
  {
    // --- Pull the variables from the fcl file.
    fAString        = pset.get<std::string> ("AString");
    fRawDigitLabel  = pset.get<std::string> ("RawDigitLabel");
    fOpDetWaveLabel = pset.get<std::string> ("OpDetWaveLabel");

    // To convince ourselves that they have been set, print them out...
    std::cout << "\n------In my activity trigger module------\nThe fcl params have been set to :"
	      << "\n  fAString:        " << fAString
	      << "\n  fRawDigitLabel:  " << fRawDigitLabel
	      << "\n  fOpDetWaveLabel: " << fOpDetWaveLabel
	      << "\n-------------------------------\n"
	      << std::endl;
  }

  //......................................................
  void ActivityTriggerProd::produce(art::Event& event)
  {
    // --- Make the vector of BasicTrigger objects:
    std::unique_ptr< std::vector<triggersim::BasicTrigger> > triggers(new std::vector<triggersim::BasicTrigger>);

    // --- Lift out the TPC raw digits:
    art::Handle< std::vector<raw::RawDigit> > DigitsHandle;
    std::vector< art::Ptr<raw::RawDigit> > rawdigits;
    if (event.getByLabel( fRawDigitLabel, DigitsHandle ) ) {
      art::fill_ptr_vector( rawdigits , DigitsHandle );
    } else {
      // --- If the RawDigitLabel was incorrect...
      mf::LogError("ActivityTrigger_Producer") << "The raw::RawDigit you gave me " << fRawDigitLabel << " is not in the event..." << std::endl;
    }

    // --- Lift out the Photon Detector OpDetWaveforms:
    art::Handle< std::vector<raw::OpDetWaveform> > WaveformHandle;
    std::vector< art::Ptr<raw::OpDetWaveform> > waveforms;
    if (event.getByLabel( fOpDetWaveLabel, WaveformHandle ) ) {
      art::fill_ptr_vector( waveforms , WaveformHandle );
    } else {
      // --- If the RawDigitLabel was incorrect...
      mf::LogError("ActivityTrigger_Producer") << "The raw::OpDetWaveform you gave me " << fOpDetWaveLabel << " is not in the event..." << std::endl;
    }
    /*
    // **************************************************
    // *********** Trigger on the whole event ***********
    // **************************************************
    std::cout << "\nLet's trigger on the whole event...." << std::endl;

    // --- Now call the class...
    bool EvTrigDec = TempTrig.TriggerOnWholeEvent( event );

    // --- Make my basic trigger and push it back
    triggersim::BasicTrigger my_ev_trig( EvTrigDec , triggersim::kNu, triggersim::kNuBeam);
    triggers->push_back( my_ev_trig );

    std::cout << "I have now left my trigger, it's decision was " << EvTrigDec << ".\n" << std::endl;
    */
    // **************************************************
    // ************* Trigger on the TPC info ************
    // **************************************************
    std::cout << "\nLet's trigger on the TPC info....Activ Trigger" << std::endl;

    // --- Now call the class...
    bool TPCTrigDec = TempTrig.TriggerOnTPC( rawdigits );
  
    // --- Make my basic trigger and push it back
    triggersim::BasicTrigger my_tpc_trig( TPCTrigDec , triggersim::kNu, triggersim::kNuBeam);
    triggers->push_back( my_tpc_trig );

    std::cout << "I have now left my trigger, it's decision was " << TPCTrigDec << ".\n" << std::endl;
    /*
    // **************************************************
    // ******** Trigger on the OpDetWaveform info *******
    // **************************************************
    std::cout << "\nLet's trigger on the OpDetWaveform info...." << std::endl;

    // --- Now call the class...
    bool PDTrigDec = TempTrig.TriggerOnPD( waveforms );
  
    // --- Make my basic trigger and push it back
    triggersim::BasicTrigger my_pd_trig( PDTrigDec , triggersim::kNu, triggersim::kNuBeam);
    triggers->push_back( my_pd_trig );

    std::cout << "I have now left my trigger, it's decision was " << PDTrigDec << ".\n" << std::endl;


    // **************************************************
    // *** Trigger on the OpDetWaveform and TPC info ****
    // **************************************************
    std::cout << "\nLet's trigger on the TPC and OpDetWaveform info...." << std::endl;

    // --- Now call the class...
    bool TPC_PD_TrigDec = TempTrig.TriggerOnTPC_PD( rawdigits, waveforms );
  
    // --- Make my basic trigger and push it back
    triggersim::BasicTrigger my_tpc_pd_trig( TPC_PD_TrigDec , triggersim::kNu, triggersim::kNuBeam);
    triggers->push_back( my_tpc_pd_trig );

    std::cout << "I have now left my trigger, it's decision was " << TPC_PD_TrigDec << ".\n" << std::endl;
    */
    // **************************************************
    // **** Trigger some of the triggers we've made *****
    // **************************************************
    std::cout << "\nLet's trigger on the triggers I've just made....ActivTrigger" << std::endl;

    // --- Now call the class...
    bool Trig_TrigDec = TempTrig.TriggerOnTriggers( *triggers );
  
    // --- Make my basic trigger and push it back
    triggersim::BasicTrigger my_trig_trig( Trig_TrigDec , triggersim::kNu, triggersim::kNuBeam);
    triggers->push_back( my_trig_trig );

    std::cout << "I have now left my trigger, it's decision was " << Trig_TrigDec << ".\n" << std::endl;

    // **************************************************
    // ******* Now put all of that into the event *******
    // **************************************************
    event.put(std::move(triggers));

  } // ActivityTriggerProd::produce()
  //......................................................
  // If you want to another function...

  //......................................................
  DEFINE_ART_MODULE(ActivityTriggerProd)
} // namespace triggersim
