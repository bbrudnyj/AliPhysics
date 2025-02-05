#include "AliAnalysisTaskDeuteronCoalescence.h"
#include "AliInputEventHandler.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisUtils.h"
#include "AliMultSelection.h"
#include "AliMultEstimator.h"
#include "AliAnalysisTask.h"
#include "TLorentzVector.h"
#include "AliMCParticle.h"
#include "AliCentrality.h"
#include "AliEventCuts.h"
#include "TDatabasePDG.h"
#include "AliESDEvent.h"
#include "AliMCEvent.h"
#include "TObjArray.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "TRandom.h"
#include "TChain.h"
#include "TMath.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
ClassImp(AliAnalysisTaskDeuteronCoalescence)

//____________________________________________________________________________________________________________________________________________________
AliAnalysisTaskDeuteronCoalescence::AliAnalysisTaskDeuteronCoalescence():
AliAnalysisTaskSE(),
fESDeventCuts(),
fESDevent(nullptr),
fMCEvent(nullptr),
fMCstack(nullptr),
fMCEventHandler(nullptr),
fOutputList(nullptr),
fQAList(nullptr),
fAverage_Nch_Transv(7.2),
hProtonWeights(nullptr),
fDeuteronWF(nullptr),
hSourceSize(nullptr),
hNumberOfEvents(nullptr),
hTransverseMult(nullptr),
hRtDistribution(nullptr),
hProtonsINELgtZERO(nullptr),
hProtonsINELgtZERO_reshaped(nullptr),
hProtons_Toward(nullptr),
hProtons_Transv(nullptr),
hProtons_Away(nullptr),
hNeutronsINELgtZERO(nullptr),
hNeutronsINELgtZERO_reshaped(nullptr),
hNeutrons_Toward(nullptr),
hNeutrons_Transv(nullptr),
hNeutrons_Away(nullptr),
hRparticles(nullptr),
hRapidityProtons(nullptr),
hRapidityNeutrons(nullptr),
hDeltaP(nullptr),
hSourceRadius_Prot(nullptr),
hSourceRadius_Neut(nullptr),
hDistanceLab(nullptr),
hDistanceDeut(nullptr),
hDistanceDiff(nullptr)
{}
//____________________________________________________________________________________________________________________________________________________
AliAnalysisTaskDeuteronCoalescence::AliAnalysisTaskDeuteronCoalescence(const char *name):
AliAnalysisTaskSE(name),
fESDeventCuts(),
fESDevent(nullptr),
fMCEvent(nullptr),
fMCstack(nullptr),
fMCEventHandler(nullptr),
fOutputList(nullptr),
fQAList(nullptr),
fAverage_Nch_Transv(7.2),
hProtonWeights(nullptr),
fDeuteronWF(nullptr),
hSourceSize(nullptr),
hNumberOfEvents(nullptr),
hTransverseMult(nullptr),
hRtDistribution(nullptr),
hProtonsINELgtZERO(nullptr),
hProtonsINELgtZERO_reshaped(nullptr),
hProtons_Toward(nullptr),
hProtons_Transv(nullptr),
hProtons_Away(nullptr),
hNeutronsINELgtZERO(nullptr),
hNeutronsINELgtZERO_reshaped(nullptr),
hNeutrons_Toward(nullptr),
hNeutrons_Transv(nullptr),
hNeutrons_Away(nullptr),
hRparticles(nullptr),
hRapidityProtons(nullptr),
hRapidityNeutrons(nullptr),
hDeltaP(nullptr),
hSourceRadius_Prot(nullptr),
hSourceRadius_Neut(nullptr),
hDistanceLab(nullptr),
hDistanceDeut(nullptr),
hDistanceDiff(nullptr)
{
    DefineInput (0, TChain::Class());
    DefineOutput(1, TList::Class());
    DefineOutput(2, TList::Class());
}
//____________________________________________________________________________________________________________________________________________________
AliAnalysisTaskDeuteronCoalescence::~AliAnalysisTaskDeuteronCoalescence()  {
    
    fOutputList->Clear();
    delete fESDevent;
    delete fMCEvent;
    delete fMCstack;
    delete fMCEventHandler;
    delete fOutputList;
    delete fQAList;
    delete hProtonWeights;
    delete fDeuteronWF;
    delete hSourceSize;
    delete hNumberOfEvents;
    delete hTransverseMult;
    delete hRtDistribution;
    delete hProtonsINELgtZERO;
    delete hProtonsINELgtZERO_reshaped;
    delete hProtons_Toward;
    delete hProtons_Transv;
    delete hProtons_Away;
    delete hNeutronsINELgtZERO;
    delete hNeutronsINELgtZERO_reshaped;
    delete hNeutrons_Toward;
    delete hNeutrons_Transv;
    delete hNeutrons_Away;
    delete hRparticles;
    delete hRapidityProtons;
    delete hRapidityNeutrons;
    delete hDeltaP;
    delete hSourceRadius_Prot;
    delete hSourceRadius_Neut;
    delete hDistanceLab;
    delete hDistanceDeut;
    delete hDistanceDiff;

}
//____________________________________________________________________________________________________________________________________________________
void AliAnalysisTaskDeuteronCoalescence::UserCreateOutputObjects()  {
    
    //Create Output List
    fOutputList = new TList();
    fQAList     = new TList();
    fOutputList -> SetOwner();
    fQAList     -> SetOwner();
    
    //Event Selection
    fESDeventCuts.AddQAplotsToList(fQAList);
    fESDeventCuts.SetManualMode();
    fESDeventCuts.fRequireTrackVertex = kTRUE;
    fESDeventCuts.fMinVtz = -10.f;
    fESDeventCuts.fMaxVtz = +10.f;
    fESDeventCuts.fMaxDeltaSpdTrackAbsolute = 0.5f;
    fESDeventCuts.fMaxResolutionSPDvertex = 0.25f;
    fESDeventCuts.fTriggerMask = (AliVEvent::kINT7);
    fESDeventCuts.fRejectDAQincomplete = kTRUE;
    fESDeventCuts.fSPDpileupMinContributors = 3;
    fESDeventCuts.fSPDpileupMinZdist = 0.8;
    fESDeventCuts.fSPDpileupNsigmaZdist = 3.0;
    fESDeventCuts.fSPDpileupNsigmaDiamXY = 2.0;
    fESDeventCuts.fSPDpileupNsigmaDiamZ = 5.0;
    fESDeventCuts.fTrackletBGcut = kTRUE;
    
    //Event Counter
    hNumberOfEvents = new TH1D ("hNumberOfEvents","",20,0,20);
    hNumberOfEvents -> Sumw2();
    fOutputList -> Add(hNumberOfEvents);
    
    //Transverse Multiplicity
    hTransverseMult = new TH1D ("hTransverseMult","",200,0,200);
    hTransverseMult -> Sumw2();
    fOutputList -> Add(hTransverseMult);
   
    //R_{T} Distribution
    hRtDistribution = new TH1D ("hRtDistribution","",1000,0,10);
    hRtDistribution -> Sumw2();
    fOutputList -> Add(hRtDistribution);

    //p_{T} Intervals
    Double_t pt_proton[] = {0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.1,1.2,1.3,1.4,1.5,
        1.6,1.7,1.8,1.9,2.0,2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0,4.5,5.0,5.5,6.0,6.5,7.0,8.0,10.0};
    Double_t pt_deuteron[]={0.7,0.8,0.9,1.0,1.1,1.2,1.4,1.6,1.8,2.0,2.2,2.6,3.0,3.4,3.8};
    const Int_t nPtProton   = sizeof(pt_proton)/sizeof(Double_t)-1;
    const Int_t nPtDeuteron = sizeof(pt_deuteron)/sizeof(Double_t)-1;
    
    //p_{T} Spectra in Events INEL>0
    hProtonsINELgtZERO            = new TH1D ("hProtonsINELgtZERO","",nPtProton,pt_proton);
    hProtonsINELgtZERO_reshaped   = new TH1D ("hProtonsINELgtZERO_reshaped","",nPtProton,pt_proton);
    hNeutronsINELgtZERO           = new TH1D ("hNeutronsINELgtZERO","",nPtProton,pt_proton);
    hNeutronsINELgtZERO_reshaped  = new TH1D ("hNeutronsINELgtZERO_reshaped","",nPtProton,pt_proton);
    hProtonsINELgtZERO            -> Sumw2();
    hNeutronsINELgtZERO           -> Sumw2();
    hProtonsINELgtZERO_reshaped   -> Sumw2();
    hNeutronsINELgtZERO_reshaped  -> Sumw2();
    fOutputList -> Add(hProtonsINELgtZERO);
    fOutputList -> Add(hNeutronsINELgtZERO);
    fOutputList -> Add(hProtonsINELgtZERO_reshaped);
    fOutputList -> Add(hNeutronsINELgtZERO_reshaped);

    
    //p_{T} Spectra Deuterons
    for (Int_t i=0 ; i<50 ; i++)  {
        
        //p_{T} Spectra: Deuterons (Simple Coalescence)
        hDeuteronsINELgtZERO_simpleCoal[i] = new TH1D (Form("hDeuteronsINELgtZERO_simpleCoal[%d]",i),"",nPtDeuteron,pt_deuteron);
        hDeuterons_Toward_simpleCoal[i]    = new TH1D (Form("hDeuterons_Toward_simpleCoal[%d]",i),"",400,0,4);
        hDeuterons_Transv_simpleCoal[i]    = new TH1D (Form("hDeuterons_Transv_simpleCoal[%d]",i),"",400,0,4);
        hDeuteronsINELgtZERO_simpleCoal[i] -> Sumw2();
        hDeuterons_Toward_simpleCoal[i]    -> Sumw2();
        hDeuterons_Transv_simpleCoal[i]    -> Sumw2();
        fOutputList -> Add(hDeuteronsINELgtZERO_simpleCoal[i]);
        fOutputList -> Add(hDeuterons_Toward_simpleCoal[i]);
        fOutputList -> Add(hDeuterons_Transv_simpleCoal[i]);
           
        //p_{T} Spectra: Deuterons (Wigner Gaussian)
        hDeuteronsINELgtZERO_wignerGaus[i] = new TH1D (Form("hDeuteronsINELgtZERO_wignerGaus[%d]",i),"",nPtDeuteron,pt_deuteron);
        hDeuterons_Toward_wignerGaus[i]    = new TH1D (Form("hDeuterons_Toward_wignerGaus[%d]",i),"",400,0,4);
        hDeuterons_Transv_wignerGaus[i]    = new TH1D (Form("hDeuterons_Transv_wignerGaus[%d]",i),"",400,0,4);
        hDeuteronsINELgtZERO_wignerGaus[i] -> Sumw2();
        hDeuterons_Toward_wignerGaus[i]    -> Sumw2();
        hDeuterons_Transv_wignerGaus[i]    -> Sumw2();
        fOutputList -> Add(hDeuteronsINELgtZERO_wignerGaus[i]);
        fOutputList -> Add(hDeuterons_Toward_wignerGaus[i]);
        fOutputList -> Add(hDeuterons_Transv_wignerGaus[i]);

        //p_{T} Spectra: Deuterons (Wigner Argonne)
        hDeuteronsINELgtZERO_wignerArg[i] = new TH1D (Form("hDeuteronsINELgtZERO_wignerArg[%d]",i),"",nPtDeuteron,pt_deuteron);
        hDeuterons_Toward_wignerArg[i]    = new TH1D (Form("hDeuterons_Toward_wignerArg[%d]",i),"",400,0,4);
        hDeuterons_Transv_wignerArg[i]    = new TH1D (Form("hDeuterons_Transv_wignerArg[%d]",i),"",400,0,4);
        hDeuteronsINELgtZERO_wignerArg[i] -> Sumw2();
        hDeuterons_Toward_wignerArg[i]    -> Sumw2();
        hDeuterons_Transv_wignerArg[i]    -> Sumw2();
        fOutputList -> Add(hDeuteronsINELgtZERO_wignerArg[i]);
        fOutputList -> Add(hDeuterons_Toward_wignerArg[i]);
        fOutputList -> Add(hDeuterons_Transv_wignerArg[i]);
    }
    
    
    //R_{T} Intervals
    Double_t Rt_Intervals[]={0.0,0.5,1.5,5.0};
    Int_t nRtIntervals=sizeof(Rt_Intervals)/sizeof(Double_t)-1;
    
    //Proton Spectra in the Azimuthal Regions
    hProtons_Toward   = new TH1D ("hProtons_Toward","",400,0,4);
    hProtons_Transv   = new TH1D ("hProtons_Transv","",400,0,4);
    hProtons_Away     = new TH1D ("hProtons_Away","",400,0,4);
    hProtons_Toward -> Sumw2();
    hProtons_Transv -> Sumw2();
    hProtons_Away   -> Sumw2();
    fOutputList -> Add(hProtons_Toward);
    fOutputList -> Add(hProtons_Transv);
    fOutputList -> Add(hProtons_Away);
    
    //Neutron Spectra in the Azimuthal Regions
    hNeutrons_Toward   = new TH1D ("hNeutrons_Toward","",400,0,4);
    hNeutrons_Transv   = new TH1D ("hNeutrons_Transv","",400,0,4);
    hNeutrons_Away     = new TH1D ("hNeutrons_Away","",400,0,4);
    hNeutrons_Toward -> Sumw2();
    hNeutrons_Transv -> Sumw2();
    hNeutrons_Away   -> Sumw2();
    //fOutputList -> Add(hNeutrons_Toward);
    //fOutputList -> Add(hNeutrons_Transv);
    //fOutputList -> Add(hNeutrons_Away);

    
    //QA Histograms & Debug
    hRparticles = new TH1D ("hRparticles","",1000,0,1);
    hRparticles -> Sumw2();
    fOutputList -> Add(hRparticles);
    
    //Rapidity Distributions of Protons & Neutrons that form Deuteron in |y|<0.5
    hRapidityProtons  = new TH1D ("hRapidityProtons","",200,-2,2);
    hRapidityNeutrons = new TH1D ("hRapidityNeutrons","",200,-2,2);
    hRapidityProtons  -> Sumw2();
    hRapidityNeutrons -> Sumw2();
    fOutputList -> Add(hRapidityProtons);
    fOutputList -> Add(hRapidityNeutrons);
    
    //DeltaP Distribution
    hDeltaP = new TH1D ("hDeltaP","",1000,0,1);
    hDeltaP -> Sumw2();
    fOutputList -> Add(hDeltaP);
    
    //Source radii
    hSourceRadius_Prot = new TH1D ("hSourceRadius_Prot","",500,0,5);
    hSourceRadius_Neut = new TH1D ("hSourceRadius_Neut","",500,0,5);
    hSourceRadius_Prot -> Sumw2();
    hSourceRadius_Neut -> Sumw2();
    fOutputList -> Add(hSourceRadius_Prot);
    fOutputList -> Add(hSourceRadius_Neut);

    //Control Histograms
    hDistanceLab  = new TH1D ("hDistanceLab","",500,0,5);
    hDistanceDeut = new TH1D ("hDistanceDeut","",500,0,5);
    hDistanceDiff = new TH1D ("hDistanceDiff","",500,-5,5);
    hDistanceLab  -> Sumw2();
    hDistanceDeut -> Sumw2();
    hDistanceDiff -> Sumw2();
    fOutputList->Add(hDistanceLab);
    fOutputList->Add(hDistanceDeut);
    fOutputList->Add(hDistanceDiff);

    PostData(1, fOutputList);
    PostData(2, fQAList);
}
//____________________________________________________________________________________________________________________________________________________
void AliAnalysisTaskDeuteronCoalescence::UserExec(Option_t *)  {
    
    //Seed Random Number
    gRandom->SetSeed(0);
    
    //Coalescence Momentum
    Double_t p0[]={0.200,0.210,0.220,0.230,0.240,0.245,0.250,0.255,0.260,0.265,0.270,0.275,0.280,0.285,0.290,0.295,0.300,0.305,0.310,0.315,0.320,0.325,0.330,0.335,0.340,0.345,0.350,0.360,0.370,0.380,0.390,0.400};
    const Int_t nTrials = sizeof(p0)/sizeof(Double_t);
    
    
    //Particle Masses in GeV
    Double_t mp = 0.93827208816;//Proton
    Double_t mn = 0.93956542052;//Neutron
    Double_t md = 1.87561294257;//Deuteron
    
    
    //Get Input Event (INEL>0 Selection)
    if (!GetEvent()) return;
   
    //Protons and Neutrons IDs
    vector<Int_t> proton_ID;
    vector<Int_t> neutron_ID;
    vector<Int_t> neutron_status;

    //Loop over Generated Particles
    for ( Int_t i=0; i<fMCEvent->GetNumberOfTracks(); i++ )  {

        //Get Primary Particle
        AliMCParticle *particle = (AliMCParticle*) fMCEvent->GetTrack(i);
        if (!particle) continue;
        if (!particle->IsPhysicalPrimary()) continue;
        if (IsInjectedParticle(particle))   continue;
        if (TMath::Abs(particle->Y())>1.0)  continue;
        
        //Variables
        Double_t pt = particle->Pt();
        Double_t protWeight = GetProtonWeight (pt);
        
        //Store Protons and Neutrons (no rapidity cut)
        if (particle->PdgCode()==-2212) { proton_ID.push_back(i); }
        if (particle->PdgCode()==-2112) { neutron_ID.push_back(i); neutron_status.push_back(0);}
        
        //Protons and Neutrons p_{T} Spectra from Pythia
        if (TMath::Abs(particle->Y())>0.5) continue;
        if (particle->PdgCode()==-2212) {hProtonsINELgtZERO  -> Fill(pt); hProtonsINELgtZERO_reshaped  -> Fill(pt,protWeight);}
        if (particle->PdgCode()==-2112) {hNeutronsINELgtZERO -> Fill(pt); hNeutronsINELgtZERO_reshaped -> Fill(pt,protWeight);}
    }
    
    
    //Save Deuteron Properties (Simple Coalescence)
    Double_t pt_deuteron_simpleCoal     [nTrials][20];
    Double_t phi_deuteron_simpleCoal    [nTrials][20];
    Double_t weight_deuteron_simpleCoal [nTrials][20];
    Int_t nDeuterons_simpleCoal[nTrials];
    
    //Save Deuteron Properties (Wigner Gaus)
    Double_t pt_deuteron_wignerGaus     [nTrials][20];
    Double_t phi_deuteron_wignerGaus    [nTrials][20];
    Double_t weight_deuteron_wignerGaus [nTrials][20];
    Int_t nDeuterons_wignerGaus[nTrials];

    //Save Deuteron Properties (Wigner Argonne18)
    Double_t pt_deuteron_wignerArg     [nTrials][20];
    Double_t phi_deuteron_wignerArg    [nTrials][20];
    Double_t weight_deuteron_wignerArg [nTrials][20];
    Int_t nDeuterons_wignerArg[nTrials];
    
    
    //Initialize Deuteron Counters
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++) {
        
        nDeuterons_simpleCoal[iTrial]=0;
        nDeuterons_wignerGaus[iTrial]=0;
        nDeuterons_wignerArg[iTrial]=0;
    }
    
    
    //************************************************** SIMPLE COALESCENCE **************************************************//
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++)  {
        
        //Reset Neutron Status
        for (Int_t in=0 ; in<(Int_t)neutron_ID.size() ; in++)  {neutron_status[in]=0;}
        
        for (Int_t ip=0 ; ip<(Int_t)proton_ID.size() ; ip++)  {
            
            //Proton 4-Momentum
            AliMCParticle *proton = (AliMCParticle*) fMCEvent->GetTrack(proton_ID[ip]);
            TLorentzVector p_proton;
            p_proton.SetXYZM(proton->Px(),proton->Py(),proton->Pz(),mp);

            for (Int_t in=0 ; in<(Int_t)neutron_ID.size() ; in++)  {

                //Neutron 4-Momentum
                AliMCParticle *neutron = (AliMCParticle*) fMCEvent->GetTrack(neutron_ID[in]);
                TLorentzVector p_neutron;
                p_neutron.SetXYZM(neutron->Px(),neutron->Py(),neutron->Pz(),mn);
                
                //Deuteron 4-Momentum
                TLorentzVector p_deuteron;
                p_deuteron.SetXYZM(proton->Px()+neutron->Px(),proton->Py()+neutron->Py(),proton->Pz()+neutron->Pz(),md);
                Double_t beta_x = p_deuteron.Px()/p_deuteron.E();
                Double_t beta_y = p_deuteron.Py()/p_deuteron.E();
                Double_t beta_z = p_deuteron.Pz()/p_deuteron.E();
                TVector3 beta (beta_x,beta_y,beta_z);

                //Lorentz Transformations (from Lab to Deuteron Frame)
                TLorentzVector p_proton_prime  = LorentzTransform (p_proton,beta);
                TLorentzVector p_neutron_prime = LorentzTransform (p_neutron,beta);
                Double_t deltaP = (p_proton_prime-p_neutron_prime).P();
                Double_t deutWeight = GetDeuteronWeight (p_proton.Pt(),p_neutron.Pt());
                
                //Fill DeltaP Distribution
                if (iTrial==0) hDeltaP->Fill(deltaP);

                if (neutron_status[in]==1) continue;//Skip already used neutrons

                //Simple Coalescence Condition
                if (deltaP<p0[iTrial]) {
                    
                    neutron_status[in]=1;
                    Double_t y = p_deuteron.Rapidity();
                    if (TMath::Abs(y)<0.5) {
                        
                        hDeuteronsINELgtZERO_simpleCoal[iTrial] -> Fill(p_deuteron.Pt(),deutWeight);
                        
                        //Store Deuteron Properties
                        phi_deuteron_simpleCoal[iTrial][nDeuterons_simpleCoal[iTrial]]    = TVector2::Phi_0_2pi(p_deuteron.Phi());
                        pt_deuteron_simpleCoal[iTrial][nDeuterons_simpleCoal[iTrial]]     = p_deuteron.Pt();
                        weight_deuteron_simpleCoal[iTrial][nDeuterons_simpleCoal[iTrial]] = deutWeight;
                        nDeuterons_simpleCoal[iTrial]++;
                        
                        //Rapidity Distributions of Protons and Neutrons
                        hRapidityProtons  -> Fill (proton->Y());
                        hRapidityNeutrons -> Fill (neutron->Y());
                    }
                    break;
                }
            }
        }
    }
    //******************************************************************************************************************************//

    
    //************************************************** COALESCENCE: WIGNER GAUS **************************************************//
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++)  {
        
        //Reset Neutron Status
        for (Int_t in=0 ; in<(Int_t)neutron_ID.size() ; in++)  {neutron_status[in]=0;}
        
        for (Int_t ip=0 ; ip<(Int_t)proton_ID.size() ; ip++)  {
            
            //Proton 4-Momentum
            AliMCParticle *proton = (AliMCParticle*) fMCEvent->GetTrack(proton_ID[ip]);
            TLorentzVector p_proton;
            p_proton.SetXYZM(proton->Px(),proton->Py(),proton->Pz(),mp);

            for (Int_t in=0 ; in<(Int_t)neutron_ID.size() ; in++)  {

                //Neutron 4-Momentum
                AliMCParticle *neutron = (AliMCParticle*) fMCEvent->GetTrack(neutron_ID[in]);
                TLorentzVector p_neutron;
                p_neutron.SetXYZM(neutron->Px(),neutron->Py(),neutron->Pz(),mn);

                //Deuteron 4-Momentum
                TLorentzVector p_deuteron;
                p_deuteron.SetXYZM(proton->Px()+neutron->Px(),proton->Py()+neutron->Py(),proton->Pz()+neutron->Pz(),md);
                Double_t beta_x = p_deuteron.Px()/p_deuteron.E();
                Double_t beta_y = p_deuteron.Py()/p_deuteron.E();
                Double_t beta_z = p_deuteron.Pz()/p_deuteron.E();
                TVector3 beta (beta_x,beta_y,beta_z);

                //Lorentz Transformation
                TLorentzVector p_proton_prime  = LorentzTransform (p_proton,beta);
                TLorentzVector p_neutron_prime = LorentzTransform (p_neutron,beta);
                
                Double_t deltaX = GetSpatialDistance (p_proton,p_neutron,beta);
                Double_t deltaP = (p_proton_prime-p_neutron_prime).P();
                Double_t deutWeight = GetDeuteronWeight (p_proton.Pt(),p_neutron.Pt());

                if (neutron_status[in]==1) continue;//Skip already used neutrons

                //Coalescence Condition
                if (DoCoalescence(deltaX,deltaP,p0[iTrial],"Gaus"))  {
                    
                    neutron_status[in]=1;
                    Double_t y = p_deuteron.Rapidity();
                    if (TMath::Abs(y)<0.5) {
                        
                        hDeuteronsINELgtZERO_wignerGaus[iTrial] -> Fill(p_deuteron.Pt(),deutWeight);
                        
                        //Store Deuteron Properties
                        phi_deuteron_wignerGaus[iTrial][nDeuterons_wignerGaus[iTrial]]    = TVector2::Phi_0_2pi(p_deuteron.Phi());
                        pt_deuteron_wignerGaus[iTrial][nDeuterons_wignerGaus[iTrial]]     = p_deuteron.Pt();
                        weight_deuteron_wignerGaus[iTrial][nDeuterons_wignerGaus[iTrial]] = deutWeight;
                        nDeuterons_wignerGaus[iTrial]++;
                    }
                    break;
                }
            }
        }
    }
    //*********************************************************************************************************************************//

    
    //************************************************** COALESCENCE: WIGNER ARGONNE **************************************************//
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++)  {
        
        //Reset Neutron Status
        for (Int_t in=0 ; in<(Int_t)neutron_ID.size() ; in++)  {neutron_status[in]=0;}
        
        for (Int_t ip=0 ; ip<(Int_t)proton_ID.size() ; ip++)  {
            
            //Proton 4-Momentum
            AliMCParticle *proton = (AliMCParticle*) fMCEvent->GetTrack(proton_ID[ip]);
            TLorentzVector p_proton;
            p_proton.SetXYZM(proton->Px(),proton->Py(),proton->Pz(),mp);

            for (Int_t in=0 ; in<(Int_t)neutron_ID.size() ; in++)  {

                //Neutron 4-Momentum
                AliMCParticle *neutron = (AliMCParticle*) fMCEvent->GetTrack(neutron_ID[in]);
                TLorentzVector p_neutron;
                p_neutron.SetXYZM(neutron->Px(),neutron->Py(),neutron->Pz(),mn);

                //Deuteron 4-Momentum
                TLorentzVector p_deuteron;
                p_deuteron.SetXYZM(proton->Px()+neutron->Px(),proton->Py()+neutron->Py(),proton->Pz()+neutron->Pz(),md);
                Double_t beta_x = p_deuteron.Px()/p_deuteron.E();
                Double_t beta_y = p_deuteron.Py()/p_deuteron.E();
                Double_t beta_z = p_deuteron.Pz()/p_deuteron.E();
                TVector3 beta (beta_x,beta_y,beta_z);

                //Lorentz Transformation
                TLorentzVector p_proton_prime  = LorentzTransform (p_proton,beta);
                TLorentzVector p_neutron_prime = LorentzTransform (p_neutron,beta);
                
                Double_t deltaX = GetSpatialDistance (p_proton,p_neutron,beta);
                Double_t deltaP = (p_proton_prime-p_neutron_prime).P();
                Double_t deutWeight = GetDeuteronWeight (p_proton.Pt(),p_neutron.Pt());

                if (neutron_status[in]==1) continue;//Skip already used neutrons

                //Coalescence Condition
                if (DoCoalescence(deltaX,deltaP,p0[iTrial],"Arg"))  {
                    
                    neutron_status[in]=1;
                    Double_t y = p_deuteron.Rapidity();
                    if (TMath::Abs(y)<0.5) {
                        
                        hDeuteronsINELgtZERO_wignerArg[iTrial] -> Fill(p_deuteron.Pt(),deutWeight);
                        
                        //Store Deuteron Properties
                        phi_deuteron_wignerArg[iTrial][nDeuterons_wignerArg[iTrial]]    = TVector2::Phi_0_2pi(p_deuteron.Phi());
                        pt_deuteron_wignerArg[iTrial][nDeuterons_wignerArg[iTrial]]     = p_deuteron.Pt();
                        weight_deuteron_wignerArg[iTrial][nDeuterons_wignerArg[iTrial]] = deutWeight;
                        nDeuterons_wignerArg[iTrial]++;
                    }
                    break;
                }
            }
        }
    }
    //*********************************************************************************************************************************//










    //*********************************************************************************************************************************//

    
    //Selection of Leading Particle
    Int_t lp = GetLeadingParticle();
    AliMCParticle *leading_particle = (AliMCParticle*) fMCEvent->GetTrack(lp);
    Double_t pt_leading  = leading_particle->Pt();
    Double_t phi_leading = TVector2::Phi_0_2pi(leading_particle->Phi());
    if (pt_leading<5.0) return;
    hNumberOfEvents -> Fill(17.5);

    //Transverse Multiplicity
    Int_t nParticles_Transv = GetTransverseMultiplicity(lp);
    hTransverseMult -> Fill(nParticles_Transv);
    
    //R_{T}
    Double_t Rt = static_cast<Double_t>(nParticles_Transv)/fAverage_Nch_Transv;
    hRtDistribution -> Fill (Rt);
    
    
    //Fill Proton Spectra in Azimuthal Region
    for (Int_t i=0 ; i<(Int_t)proton_ID.size() ; i++)  {
        
        //Get Particle
        AliMCParticle *particle = (AliMCParticle*) fMCEvent->GetTrack(proton_ID[i]);
        if (TMath::Abs(particle->Y())>0.5) continue;
        
        //Variables
        Double_t pt = particle->Pt();
        Double_t phi_particle = TVector2::Phi_0_2pi(particle->Phi());
        Double_t protWeight = GetProtonWeight (pt);

        //Fill p_{T} Spectra Protons
        if (IsParticleInTowardRegion(phi_particle,phi_leading))     hProtons_Toward->Fill(pt,protWeight);
        if (IsParticleInTransverseRegion(phi_particle,phi_leading)) hProtons_Transv->Fill(pt,protWeight);
        if (IsParticleInAwayRegion(phi_particle,phi_leading))       hProtons_Away->Fill(pt,protWeight);
    }
    
    //Fill Deuteron Spectra in Azimuthal Regions: Simple Coalescence
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++)  {
        for (Int_t i=0 ; i<nDeuterons_simpleCoal[iTrial] ; i++)  {

            //Variables
            Double_t pt           = pt_deuteron_simpleCoal[iTrial][i];
            Double_t phi_particle = phi_deuteron_simpleCoal[iTrial][i];
            Double_t deutWeight   = weight_deuteron_simpleCoal[iTrial][i];
                  
            //Fill p_{T} Spectra Deuterons
            if (IsParticleInTowardRegion(phi_particle,phi_leading))     hDeuterons_Toward_simpleCoal[iTrial]->Fill(pt,deutWeight);
            if (IsParticleInTransverseRegion(phi_particle,phi_leading)) hDeuterons_Transv_simpleCoal[iTrial]->Fill(pt,deutWeight);
        }
    }
    
    //Fill Deuteron Spectra in Azimuthal Regions: Wigner Gaus
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++)  {
        for (Int_t i=0 ; i<nDeuterons_wignerGaus[iTrial] ; i++)  {

            //Variables
            Double_t pt           = pt_deuteron_wignerGaus[iTrial][i];
            Double_t phi_particle = phi_deuteron_wignerGaus[iTrial][i];
            Double_t deutWeight   = weight_deuteron_wignerGaus[iTrial][i];
                  
            //Fill p_{T} Spectra Deuterons
            if (IsParticleInTowardRegion(phi_particle,phi_leading))     hDeuterons_Toward_wignerGaus[iTrial]->Fill(pt,deutWeight);
            if (IsParticleInTransverseRegion(phi_particle,phi_leading)) hDeuterons_Transv_wignerGaus[iTrial]->Fill(pt,deutWeight);
        }
    }

    //Fill Deuteron Spectra in Azimuthal Regions: Wigner Argonne
    for (Int_t iTrial=0 ; iTrial<nTrials ; iTrial++)  {
        for (Int_t i=0 ; i<nDeuterons_wignerArg[iTrial] ; i++)  {

            //Variables
            Double_t pt           = pt_deuteron_wignerArg[iTrial][i];
            Double_t phi_particle = phi_deuteron_wignerArg[iTrial][i];
            Double_t deutWeight   = weight_deuteron_wignerArg[iTrial][i];
                  
            //Fill p_{T} Spectra Deuterons
            if (IsParticleInTowardRegion(phi_particle,phi_leading))     hDeuterons_Toward_wignerArg[iTrial]->Fill(pt,deutWeight);
            if (IsParticleInTransverseRegion(phi_particle,phi_leading)) hDeuterons_Transv_wignerArg[iTrial]->Fill(pt,deutWeight);
        }
    }
    
    
    //Post Output Data
    PostData(1, fOutputList);
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::GetEvent ()  {
    
    //Get ESD Event
    fESDevent = dynamic_cast <AliESDEvent*>(InputEvent());
    if (!fESDevent) return kFALSE;
    hNumberOfEvents -> Fill(0.5);

    //Get MC Event
    fMCEvent = MCEvent();
    if (!fMCEvent) return kFALSE;
    hNumberOfEvents -> Fill(1.5);
       
    //Get MC Event Handler
    fMCEventHandler = dynamic_cast<AliMCEventHandler*>(AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler());
    if(!fMCEventHandler) return kFALSE;
    hNumberOfEvents -> Fill(2.5);

    //Standard Event Cuts
    if (!fESDeventCuts.AcceptEvent(fESDevent)) {
        PostData(2, fQAList);
        return kFALSE;
    }
    hNumberOfEvents -> Fill(3.5);
       
    //Reject Events with Incomplete DAQ
    if (fESDevent->IsIncompleteDAQ()) return kFALSE;
    hNumberOfEvents -> Fill(4.5);
       
    //V0 Timing Decision
    AliVVZERO *vzeroData = fESDevent->GetVZEROData();
    if (!(vzeroData->GetV0ADecision()) || !(vzeroData->GetV0CDecision())) return kFALSE;
    hNumberOfEvents -> Fill(5.5);
       
    //Pileup Rejection
    Int_t nClustersLayer0 = fESDevent->GetNumberOfITSClusters(0);
    Int_t nClustersLayer1 = fESDevent->GetNumberOfITSClusters(1);
    Int_t nTracklets      = fESDevent->GetMultiplicity()->GetNumberOfTracklets();
    if ((nClustersLayer0 + nClustersLayer1) > 65.0 + (Double_t)nTracklets*4.0) return kFALSE;
    hNumberOfEvents -> Fill(6.5);

    //Primary Vertex Tracks
    AliESDVertex *vertex_tracks = (AliESDVertex*) fESDevent->GetPrimaryVertexTracks();
    if (!vertex_tracks) return kFALSE;
    hNumberOfEvents -> Fill(7.5);
       
    //Vertex Contributors Tracks
    if ( vertex_tracks->GetNContributors() < 1 ) return kFALSE;
    hNumberOfEvents -> Fill(8.5);
       
    //Primary Vertex SPD
    AliESDVertex *vertex_SPD = (AliESDVertex*) fESDevent->GetPrimaryVertexSPD();
    if (!vertex_SPD) return kFALSE;
    hNumberOfEvents -> Fill(9.5);
       
    //Vertex Contributors SPD
    if (vertex_SPD->GetNContributors() < 1 ) return kFALSE;
    hNumberOfEvents -> Fill(10.5);
       
    //SPD Pile-up in Mult Bins
    if (fESDevent->IsPileupFromSPDInMultBins()) return kFALSE;
    hNumberOfEvents -> Fill(11.5);
       
    //Cut on Z-Vertex Resolution
    if (TMath::Abs(vertex_SPD->GetZ() - vertex_tracks->GetZ()) > 0.3) return kFALSE;
    hNumberOfEvents -> Fill(12.5);

    //Primary Vertex Selection
    if (vertex_tracks->GetZ() < -10.0) return kFALSE;
    if (vertex_tracks->GetZ() > +10.0) return kFALSE;
    hNumberOfEvents -> Fill(13.5);
              
    //Multiplicity
    AliMultSelection *multiplicitySelection = (AliMultSelection*) fESDevent->FindListObject("MultSelection");
    if(!multiplicitySelection) return kFALSE;
    hNumberOfEvents -> Fill(14.5);
       
    //Selection of Multiplicity Range
    Double_t mult_percentile = multiplicitySelection->GetMultiplicityPercentile("V0M");
    if (mult_percentile <  0.0)   return kFALSE;
    if (mult_percentile >  100.0) return kFALSE;
    hNumberOfEvents -> Fill(15.5);
    
    //Selection of INEL>0 Events
    if (!IsINELgtZERO ()) return kFALSE;
    hNumberOfEvents -> Fill(16.5);
         
    return kTRUE;
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::IsINELgtZERO ()  {
    
    //Initialization
    Bool_t isEventINELgtZERO = (kFALSE);
       
    //Loop over Generated Particles
    for ( Int_t i=0; i<fMCEvent->GetNumberOfTracks(); i++ )  {

        AliMCParticle *particle = (AliMCParticle*) fMCEvent->GetTrack(i);
        if (!particle) continue;
        if (particle->Charge()==0)        continue;
        if (IsInjectedParticle(particle)) continue;
        if (particle->MCStatusCode()<=0)  continue;

        Bool_t isFSParticle = (kFALSE);
        if (TMath::Abs(particle->PdgCode())==211)  isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==321)  isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==2212) isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==11)   isFSParticle=kTRUE;
        if (isFSParticle && TMath::Abs(particle->Eta())<1.0) {isEventINELgtZERO = kTRUE; break;}
    }
    
    return isEventINELgtZERO;
}
//____________________________________________________________________________________________________________________________________________________
Double_t AliAnalysisTaskDeuteronCoalescence::GetProtonWeight (Double_t pt)  {
    
    //Initialization
    Double_t w(1);
    
    Int_t ibin = hProtonWeights->FindBin(pt);
    w = hProtonWeights->GetBinContent(ibin);
    return w;
}
//____________________________________________________________________________________________________________________________________________________
Double_t AliAnalysisTaskDeuteronCoalescence::GetDeuteronWeight (Double_t pt_prot, Double_t pt_neut)  {
    
    //Initialization
    Double_t w(1);
    
    Double_t wp = hProtonWeights->GetBinContent(hProtonWeights->FindBin(pt_prot));
    Double_t wn = hProtonWeights->GetBinContent(hProtonWeights->FindBin(pt_neut));
    Double_t spin_factor    = 3.0/4.0;
    Double_t isospin_factor = 1.0/2.0;
    w = spin_factor*isospin_factor*wp*wn;

    return w;
}
//____________________________________________________________________________________________________________________________________________________
Double_t AliAnalysisTaskDeuteronCoalescence::GetSpatialDistance (TLorentzVector p_proton, TLorentzVector p_neutron, TVector3 beta_vect)  {
    
    //Initialization
    Double_t deltaX(0);
    
    //Source Radius: m_{T} dependence
    Double_t r_source_proton  = hSourceSize->GetBinContent(hSourceSize->FindBin(p_proton.Mt()));
    Double_t r_source_neutron = hSourceSize->GetBinContent(hSourceSize->FindBin(p_neutron.Mt()));

    //Proton Coordinates in the Lab
    Double_t rp      = TMath::Abs(gRandom->Gaus(0.0,r_source_proton));
    Double_t theta_p = p_proton.Theta();
    Double_t phi_p   = p_proton.Phi();
    Double_t xp = rp*TMath::Sin(theta_p)*TMath::Cos(phi_p);
    Double_t yp = rp*TMath::Sin(theta_p)*TMath::Sin(phi_p);
    Double_t zp = rp*TMath::Cos(theta_p);
    TLorentzVector r_proton_lab;
    r_proton_lab.SetXYZT(xp,yp,zp,0);
    
    //Neutron Coordinates in the Lab
    Double_t rn      = TMath::Abs(gRandom->Gaus(0.0,r_source_neutron));
    Double_t theta_n = p_neutron.Theta();
    Double_t phi_n   = p_neutron.Phi();
    Double_t xn = rn*TMath::Sin(theta_n)*TMath::Cos(phi_n);
    Double_t yn = rn*TMath::Sin(theta_n)*TMath::Sin(phi_n);
    Double_t zn = rn*TMath::Cos(theta_n);
    TLorentzVector r_neutron_lab;
    r_neutron_lab.SetXYZT(xn,yn,zn,0);
    
    //Fill Control Histograms
    hSourceRadius_Prot -> Fill (rp);
    hSourceRadius_Neut -> Fill (rn);
    
    //Distance in the Deuteron Frame
    TLorentzVector r_proton_prime  = LorentzTransform (r_proton_lab,beta_vect);
    TLorentzVector r_neutron_prime = LorentzTransform (r_neutron_lab,beta_vect);
    deltaX = (r_proton_prime-r_neutron_prime).P();
    
    //Control Histograms
    Double_t distance_lab  = (r_proton_lab-r_neutron_lab).P();
    Double_t distance_deut = (r_proton_prime-r_neutron_prime).P();
    hDistanceLab  -> Fill(distance_lab);
    hDistanceDeut -> Fill(distance_deut);
    hDistanceDiff -> Fill(distance_lab-distance_deut);
    
    return deltaX;
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::DoCoalescence (Double_t deltaX, Double_t deltaP, Double_t sigma_p, const char *func)  {
    
    //Initialization
    Bool_t doCoalescence=(kFALSE);
    
    //Constants
    Double_t sigma_x = 3.2;//(fm) Parameter related to deuteron radius (arxiv.org/abs/1812.05175)
    Double_t max = fDeuteronWF->GetMaximum();
    
    //Wave Function in Momentum Space
    Double_t Psi_momentum = TMath::Exp(-(deltaP*deltaP)/(sigma_p*sigma_p));

    //Wave Function in Coordinate Space
    Double_t Psi_space(0);
    if (strcmp(func,"Gaus")==0) Psi_space = TMath::Exp(-(deltaX*deltaX)/(sigma_x*sigma_x));
    if (strcmp(func,"Arg")==0)  Psi_space = (1.0/max)*fDeuteronWF->Eval(deltaX);

    //Wigner Density
    Double_t wigner = Psi_space*Psi_momentum;
    
    //Coalescence Probability
    Double_t rndm = gRandom->Uniform(0.0,1.0);
    if (rndm<wigner) doCoalescence=kTRUE;
    
    return doCoalescence;
}
//____________________________________________________________________________________________________________________________________________________
Int_t AliAnalysisTaskDeuteronCoalescence::GetLeadingParticle ()  {
    
    //Initialization
    Int_t ID_leading_particle(0);
    Double_t pt_max(0);
    
    //Loop over Generated Particles
    for ( Int_t i=0; i<fMCEvent->GetNumberOfTracks(); i++ )  {

        //Get Particle
        AliMCParticle *particle = (AliMCParticle*) fMCEvent->GetTrack(i);
        if (!particle) continue;
        if (particle->Charge()==0) continue;
        if (IsInjectedParticle(particle))    continue;
        if (TMath::Abs(particle->Eta())>0.8) continue;
        if (particle->MCStatusCode()<=0)  continue;
        if (particle->Pt()<0.15) continue;
        
        //Primary Particle
        Double_t x = particle->Xv();
        Double_t y = particle->Yv();
        Double_t z = particle->Zv();
        Double_t r = TMath::Sqrt(x*x+y*y+z*z);
        if (particle->IsPhysicalPrimary()) hRparticles -> Fill(r);
        //if (r>1e-10) continue;
        
        Bool_t isFSParticle = (kFALSE);
        if (TMath::Abs(particle->PdgCode())==211)  isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==321)  isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==2212) isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==11)   isFSParticle=kTRUE;
        if (!isFSParticle) continue;
        
        if (particle->Pt() > pt_max)  {
            
            pt_max = particle->Pt();
            ID_leading_particle = i;
        }
    }
    
    return ID_leading_particle;
}
//____________________________________________________________________________________________________________________________________________________
Int_t AliAnalysisTaskDeuteronCoalescence::GetTransverseMultiplicity (Int_t leading_particle_ID)  {
    
    //Initialization
    Int_t mult_Transverse(0);
    
    //Leading Particle
    AliMCParticle *leading_particle = (AliMCParticle*) fMCEvent->GetTrack(leading_particle_ID);
    
    //Loop over Generated Particles
    for ( Int_t i=0; i<fMCEvent->GetNumberOfTracks(); i++ )  {

        //Get Particle
        AliMCParticle *particle = (AliMCParticle*) fMCEvent->GetTrack(i);
        if (!particle) continue;
        if (particle->Charge()==0) continue;
        if (IsInjectedParticle(particle))    continue;
        if (TMath::Abs(particle->Eta())>0.8) continue;
        if (particle->MCStatusCode()<=0)     continue;
        if (particle->Pt()<0.15) continue;
               
        //Primary Particle
        Double_t x = particle->Xv();
        Double_t y = particle->Yv();
        Double_t z = particle->Zv();
        Double_t r = TMath::Sqrt(x*x+y*y+z*z);
        //if (r>1e-10) continue;
        
        Bool_t isFSParticle = (kFALSE);
        if (TMath::Abs(particle->PdgCode())==211)  isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==321)  isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==2212) isFSParticle=kTRUE;
        if (TMath::Abs(particle->PdgCode())==11)   isFSParticle=kTRUE;
        if (!isFSParticle) continue;
                   
        Double_t phi_particle = TVector2::Phi_0_2pi(particle->Phi());
        Double_t phi_leading  = TVector2::Phi_0_2pi(leading_particle->Phi());

        if (IsParticleInTransverseRegion (phi_particle,phi_leading)) mult_Transverse++;
    }
    
    return mult_Transverse;
    
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::IsParticleInTowardRegion (Double_t phi, Double_t phi_leading)  {
    
    //Initialization
    Bool_t isInTowardRegion = kFALSE;
              
    //DeltaPhi
    Double_t delta_phi = (180.0/TMath::Pi())*TVector2::Phi_0_2pi (phi-phi_leading);
       
    if (delta_phi>=0.0   && delta_phi<60.0)   isInTowardRegion=kTRUE;
    if (delta_phi>=300.0 && delta_phi<=360.0) isInTowardRegion=kTRUE;

    return isInTowardRegion;
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::IsParticleInTransverseRegion (Double_t phi, Double_t phi_leading)  {
    
    //Initialization
    Bool_t isInTransverseRegion = kFALSE;
              
    //DeltaPhi
    Double_t delta_phi = (180.0/TMath::Pi())*TVector2::Phi_0_2pi (phi-phi_leading);
       
    if (delta_phi>=60.0  && delta_phi<120.0) isInTransverseRegion=kTRUE;
    if (delta_phi>=240.0 && delta_phi<300.0) isInTransverseRegion=kTRUE;

    return isInTransverseRegion;
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::IsParticleInAwayRegion (Double_t phi, Double_t phi_leading)  {
    
    //Initialization
    Bool_t isInAwayRegion = kFALSE;
              
    //DeltaPhi
    Double_t delta_phi = (180.0/TMath::Pi())*TVector2::Phi_0_2pi (phi-phi_leading);
    if (delta_phi>=120.0 && delta_phi<240.0) isInAwayRegion=kTRUE;

    return isInAwayRegion;
}
//____________________________________________________________________________________________________________________________________________________
Bool_t AliAnalysisTaskDeuteronCoalescence::IsInjectedParticle (AliMCParticle *particle)  {
    
    //Initialization
    Bool_t isInjected=kFALSE;
    
    if (TMath::Abs(particle->PdgCode())==1000010020) isInjected=kTRUE;//Deuteron
    if (TMath::Abs(particle->PdgCode())==1000020030) isInjected=kTRUE;//Helium-3
    if (TMath::Abs(particle->PdgCode())==1000010030) isInjected=kTRUE;//Triton
    if (TMath::Abs(particle->PdgCode())==1000020040) isInjected=kTRUE;//Helium-4
    if (TMath::Abs(particle->PdgCode())==1010010030) isInjected=kTRUE;//Hypertriton

    return isInjected;
}
//____________________________________________________________________________________________________________________________________________________
TLorentzVector AliAnalysisTaskDeuteronCoalescence::LorentzTransform (TLorentzVector R, TVector3 beta_vect)  {
    
    //Inizialization
    TLorentzVector R_prime (0.0, 0.0, 0.0, 0.0);
    
    //Beta Components
    Double_t Bx = beta_vect.X();
    Double_t By = beta_vect.Y();
    Double_t Bz = beta_vect.Z();
    
    //Beta & Gamma
    Double_t beta  = TMath::Sqrt(Bx*Bx + By*By + Bz*Bz);
    if (beta>=1.0) { return R_prime; }
    Double_t gamma = 1.0/TMath::Sqrt(1.0-(beta*beta));
    
    //Coordinates in the Lab System
    Double_t t = R.T();
    Double_t x = R.X();
    Double_t y = R.Y();
    Double_t z = R.Z();
    
    //Coordinates in the Deuteron Frame
    Double_t t_prime = gamma*t - gamma*Bx*x - gamma*By*y - gamma*Bz*z;
    Double_t x_prime = -gamma*Bx*t + (1.0+(gamma-1.0)*Bx*Bx/(beta*beta))*x + (gamma-1.0)*(Bx*By/(beta*beta))*y + (gamma-1.0)*(Bx*Bz/(beta*beta))*z;
    Double_t y_prime = -gamma*By*t + (gamma-1.0)*(Bx*By/(beta*beta))*x + (1.0+(gamma-1.0)*By*By/(beta*beta))*y + (gamma-1.0)*(By*Bz/(beta*beta))*z;
    Double_t z_prime = -gamma*Bz*t + (gamma-1.0)*(Bx*Bz/(beta*beta))*x + (gamma-1.0)*(By*Bz/(beta*beta))*y + (1.0+(gamma-1.0)*Bz*Bz/(beta*beta))*z;

    //Set Coordinates
    R_prime.SetXYZT(x_prime,y_prime,z_prime,t_prime);
    
    return R_prime;
}
//____________________________________________________________________________________________________________________________________________________
void AliAnalysisTaskDeuteronCoalescence::Terminate(Option_t *)  {
    
    fOutputList = dynamic_cast<TList*> (GetOutputData(1));
    if (!fOutputList) return;
}
//____________________________________________________________________________________________________________________________________________________
