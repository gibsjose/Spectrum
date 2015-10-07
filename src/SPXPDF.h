//************************************************************/
//
//	PDF Header
//
//	Outlines the SPXPDF class, which manages the PDF data
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		01.06.2013 - Renamed 15.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPDF_H
#define SPXPDF_H

// if set timer is on
//#define TIMER

#include <stdlib.h> // exit()
#include <sstream>  // needed for internal io
#include <iomanip>


//ROOT
#include "SPXROOT.h"
//
//appl_grid
//
#include "appl_grid/appl_grid.h"
#include "appl_grid/generic_pdf.h"
#ifdef TIMER
#include "appl_grid/appl_timer.h"
#endif
//LHAPDF
#include "LHAPDF/LHAPDF.h"

#if defined LHAPDF_MAJOR_VERSION && LHAPDF_MAJOR_VERSION == 6
#else
#include "SPXLHAPDF.h"
#endif

#include "SPXPDFSteeringFile.h"
#include "SPXException.h"
#include "SPXUtilities.h"
#include "SPXGrid.h"

//#define DEFAULT -1

//using namespace std;
typedef std::map<std::string,  TGraphAsymmErrors*>  BandMap_T;

class SPXPDF {

    public:

        //METHODS
        SPXPDF() {};
        SPXPDF(SPXPDFSteeringFile *psf, int ifl, double q2, TH1D *h1);
        SPXPDF(SPXPDFSteeringFile *psf, const std::string &_gridName);
        SPXPDF(SPXPDFSteeringFile *psf, SPXGrid *grid);

        virtual ~SPXPDF() { CleanUpSPXPDF(); }; //destructor

        void PrintMap(BandMap_T &m);

        // method for PDF standalone plots

        TH1D * FillPdfHisto();

        void CleanUpSPXPDF();
        void Initialize();
        void Print();
        void ReadPDFSteeringFile(SPXPDFSteeringFile *psf);

        void InitializeErrorGraphs();

        void CalcSystErrors();
        void CalcPDFBandErrors();
        void CalcAlphaSErrors();
        void CalcScaleErrors();
        void CalcAlternativeScaleChoiceErrors();
        void CalcTotalErrors();
        void CalcBeamEnergyErrors();

	// void GetRatioToTH1(TH1D* href);
        TH1D* GetPDFRatio(int iset1, int iset2=0);
        void DrawPDFRatio(int iset1, int iset2=0);
        void DrawPDFBand();

        double GetPDFWeight(int iset1, double x1, double x2);

        double GetMaximum(int iset);
        double GetMinimum(int iset);

        //accessor methods
        bool IsDebugOn() const{return debug;};

	std::string GetPDFtype() const{return PDFtype;};
	std::string GetPDFName() const{return PDFname;};
	std::string GetPDFFullname(){ return default_pdf_set_name;};

        int GetNumPDFMembers() const{return n_PDFMembers;};
        int GetFillStyleCode() const{return fillStyleCode;};
        int GetFillColorCode() const{return fillColorCode;};
	int GetMarkerStyle() const{return markerStyle;};
	int GetMarkerColor() const{return fillColorCode;}; //should be marker color, using fill color as default
	std::string GetPDFBandType() const{return PDFBandType;};

        bool Is90PercentErrorSize()const{return f_PDFErrorSize90Percent;}; 

        bool GetDoPDFBand() const{return do_PDFBand;};
        bool GetDoAlphaS()  const{return do_AlphaS;};
        bool GetDoScale()   const{return do_Scale;};
        bool GetDoAlternativeScaleChoice() const{return do_AlternativeScaleChoice;};
        bool GetDoTotal()   const{return do_Total;};
        bool GetDoBeamEnergyUncertainty() const{return do_Escale;};

        int GetNBands(){return Mapallbands.size();};
        TGraphAsymmErrors *GetBand(int i);
	std::string GetBandType(int i);

        TGraphAsymmErrors *GetTotalBand() {return h_Total_results;}

        int GetAlphaSmemberNumDown() const{return AlphaSmemberNumDown;};
        int GetAlphaSmemberNumUp() const{return AlphaSmemberNumUp;};
	std::string GetAlphaSPDFSetNameDown() const{return AlphaSPDFSetNameDown;};
	std::string GetAlphaSPDFSetNameUp() const{return AlphaSPDFSetNameUp;};

        TH1D * GetPdfdefault() { return hpdfdefault;};
        TH1D * GetPDFNominal() { return hpdfdefault; };

	int GetNumberOfIndividualPDFComponents() {return h_errors_PDF.size();}
	TH1D* GetIndividualPDFComponent(int ipdf);
	void SetIndividualPDFComponent(int ipdf, TH1D *h); 

	int GetNumberOfIndividualScaleVariations() {return h_errors_Scale.size();}
	TH1D* GetIndividualScaleVariation(int iscale);
	void  SetIndividualScaleVariation(int iscale, TH1D *h); 

	int GetNumberOfIndividualAlternativeScaleChoiceVariations() {return h_errors_AlternativeScaleChoice.size();}
	TH1D* GetIndividualAlternativeScaleChoiceVariation(int iscale);
	void  SetIndividualAlternativeScaleChoiceVariation(int iscale, TH1D *h); 

	int GetNumberOfIndividualAlphaSVariations() {return h_errors_AlphaS.size();}
	TH1D* GetIndividualAlphaSVariation(int ialphas);
	void  SetIndividualAlphaSVariation(int iscale, TH1D *h); 

	int GetNumberOfBeamUncertaintyVariationsVariations() {return h_errors_BeamUncertainty.size();}

        //mutator methods
        void SetDebug(bool _debug);
        void SetGridName(std::string _gridName);
        void SetSteeringFilePath(std::string _steeringFilePath);
        void SetSteeringFileDir(std::string _steeringFileDir);
        void SetSteeringFileName(std::string _steeringFileName);
        void SetPDFtype(std::string _PDFtype);
        void SetPDFname(std::string _PDFname);
        void SetNumPDFMembers(int _n_PDFMembers);
        void SetFillStyleCode(int _fillStyleCode);
        void SetFillColorCode(int _fillColorCode);
        void SetPDFBandType(std::string _PDFBandType);
        void SetPDFErrorType(std::string _PDFErrorType);
        //void SetPDFErrorSize(string _PDFErrorSize);

        void SetDoPDFBand(bool _doit);
        void SetDoAlphaS(bool _doit);
        void SetDoScale(bool _doit);
        void SetDoAlternativeScaleChoice(bool _doit);
        void SetDoTotError(bool _doit);

        void SetAlphaSmemberNumDown(int _memberNum);
        void SetAlphaSmemberNumUp(int _memberNum);
        void SetAlphaSPDFSetNameDown(std::string _name);
        void SetAlphaSPDFSetNameUp(std::string _name);

        void SetAlphaS_value_worldAverage(double setalpha) {alphaS_value_worldAverage=setalpha;};
        void SetAlphaS_absUnc_worldAverage(double setalpha){alphaS_absUnc_worldAverage=setalpha;};
        void SetAlphaS_scale_worldAverage(double setalpha) {alphaS_scale_worldAverage=setalpha;};

        void SetScales(std::vector<double> aRenScales,std::vector<double> aFacScales);

        void ApplyBandCorrection(TGraphAsymmErrors *g, std::string corrlabel, bool includeinband);

        bool BandsHaveDifferentProperties();
        bool BandsHaveDifferentMarker();
        bool BandsHaveDifferentFillStyle();

        bool HasDetailedBands();
        bool HasBandofType(std::string bandtype);

        double GetChangeSqrtS() { return Escale; };
        void SetChangeSqrtS(double fEbeam) { 
         // fEbeam is uncertainty on proton beam energy
         // Escale is scale of on CMS energy = sqrt(s)
         //
         Escale=fEbeam; 
	 if (debug) std::cout<<"SPXPDF::SetChangeSqrtS: Change beam energy to fEbeam= "<<fEbeam<<" Escale= "<<Escale<<std::endl;
         return; 
        };

    private:
        //VARIABLES
        static bool debug;

        int DEFAULT;

        // map of bands
	BandMap_T Mapallbands; // Map to hold uncertainty bands PDF, alphas, scale, total
        //bool correctedgrid;    // flag to indicated if Mapallbands are corrected
                               // by hadronisation, electroweak effects etc.
        // bands for individual uncertainties
        TGraphAsymmErrors *h_PDF_results; // PDF uncertainty
        TGraphAsymmErrors *h_AlphaS_results;  // alphas uncertainty
        TGraphAsymmErrors *h_Scale_results;   // scale uncertainty
        TGraphAsymmErrors *h_AlternativeScaleChoice_results;   // alternative scale choice uncertainty
        TGraphAsymmErrors *h_BeamUncertainty_results;   // scale uncertainty
        TGraphAsymmErrors *h_Total_results;   // total uncertainty

        //string calc_desc;
        bool applgridok;

        std::string steeringFileName;

	std::string default_pdf_set_name;

        int nLoops;                 // order of the theory prediction. L0=0, NLO=1, NNLO=2
	std::string PDFtype;             // general name for PDF, used for printing. EX: "MSTW2008nlo"
	std::string PDFname;             // specific name for PDF, used for looking up a particular PDF. EX: "MSTW2008nlo68cl"
	std::string PDFnamevar;          // for variation files like HERAPDF
        int n_PDFMembers;
        int defaultpdfid;           // set number of default PDF
        int defaultpdfidvar;        // set number of default PDF in variation file
        int firsteig;               // first eigenvector component
        int lasteig;                // last eigenvector component
        int firstquadvar;           // first eigenvector in variation sample to be added in quadrature
        int lastquadvar;            // last eigenvector in variation sample to be added in quadrature
        int firstmaxvar;            // first eigenvector to look maximum
        int lastmaxvar;             // last eigenvector  to look for maximum

        int fillStyleCode;          
        int fillColorCode;
	int markerStyle;            // optional marker style
	std::string PDFBandType;
	std::string PDFErrorType;

        bool f_PDFBandType;
        bool f_PDFErrorSize90Percent;
	std::string pdfSetPath;

        //PDFErrorPropagation_t ErrorPropagationType;
        int ErrorPropagationType;

        // for HERA type PDFS
        bool includeEIG;  // include experimental eigenvectors
        bool includeQUAD; // include model variations added in quadrature
        bool includeMAX;  // include parameterisation variations take maximum among those

        int AlphaSmemberNumDown; //needed if do_AlphaS is set. Can be read from steering or set by mutator
        int AlphaSmemberNumUp;
	std::string AlphaSPDFSetNameDown;
	std::string AlphaSPDFSetNameUp;

        appl::grid *my_grid;
        appl::grid *my_gridAlternativeScaleChoice;
        int ngrid;

        SPXGrid *spxgrid;

        std::vector<TH1D*> h_errors_AlphaS;
        std::vector<TH1D*> h_errors_PDF;
        std::vector<TH1D*> h_errors_Scale;
        std::vector<TH1D*> h_errors_AlternativeScaleChoice;
        std::vector<TH1D*> h_errors_BeamUncertainty;

        TH1D *hpdfdefault;
        std::vector<double> alphaS_variations;  // the values of alphaS variations corresponding to the histograms stored in h_errors_AlphaS

	std::string gridName;

        //double xscale;
        bool do_PDFBand;
        bool do_AlphaS;
        bool do_Scale;
        bool do_AlternativeScaleChoice;
        bool do_Escale;
        bool do_Total;

        //METHODS

        TH1D *GetHisto(double renscale=1, double facscale=1); // Get histogram from Grid

	void SetLHAPDFPDFset(std::string pdfname, int id); // interface to set PDF for LHAPDF5 and LHAPDF6

        void SetVariablesDefault();
        void SetUpParameters(SPXPDFSteeringFile *psf);
        //void SetSteeringFileNameAndDir(const string _path);
	std::string GetEnv( const std::string & var);

        // variable for PDF plotting
        int ifl; // flavour as from LHAPDF
        double Q2; // Q2 value used in LHAPDF
        TH1D * hpdf; //histo to store PDS PDF vs x

        std::vector<double> RenScales;
        std::vector<double> FacScales;

        double alphaS_value_worldAverage;
        double alphaS_absUnc_worldAverage;
        double alphaS_relUnc_worldAverage;
        double alphaS_scale_worldAverage;

        double Escale; // Changed cms energy needs to be positive Escale is relative energy scale (old/new),

};


const std::string defaultPDFSetPath="PDFsets";

#endif
