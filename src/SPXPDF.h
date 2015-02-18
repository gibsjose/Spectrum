//************************************************************/
//
//	PDF Header
//
//	Outlines the SPXPDF class, which manages the PDF data
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		01.06.2013 - Renamed 15.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPDF_H
#define SPXPDF_H

#include <stdlib.h> // exit()
#include <sstream>  // needed for internal io
#include <iomanip>

//ROOT
#include "SPXROOT.h"

//appl_grid
#include "appl_grid/appl_grid.h"
#include "appl_grid/generic_pdf.h"

//LHAPDF
#include "LHAPDF/LHAPDF.h"
#include "SPXLHAPDF.h"

#include "SPXPDFSteeringFile.h"
#include "SPXException.h"
#include "SPXUtilities.h"
#include "SPXGrid.h"

#define DEFAULT -1

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
        void CalcTotalErrors();

	// void GetRatioToTH1(TH1D* href);
        TH1D* GetPDFRatio(int iset1, int iset2=0);
        void DrawPDFRatio(int iset1, int iset2=0);
        void DrawPDFBand();

        double GetPDFWeight(int iset1, double x1, double x2);

        double GetMaximum(int iset);
        double GetMinimum(int iset);

        //accessor methods
        bool IsDebugOn() const{return debug;};

        string GetPDFtype() const{return PDFtype;};
        string GetPDFName() const{return PDFname;};
        string GetPDFFullname(){ return default_pdf_set_name;};

        int GetNumPDFMembers() const{return n_PDFMembers;};
        int GetFillStyleCode() const{return fillStyleCode;};
        int GetFillColorCode() const{return fillColorCode;};
	int GetMarkerStyle() const{return markerStyle;};
	int GetMarkerColor() const{return fillColorCode;}; //should be marker color, using fill color as default
        string GetPDFBandType() const{return PDFBandType;};

        bool Is90PercentErrorSize()const{return f_PDFErrorSize90Percent;}; 

        bool GetDoPDFBand() const{return do_PDFBand;};
        bool GetDoAlphaS()  const{return do_AlphaS;};
        bool GetDoScale()   const{return do_Scale;};
        bool GetDoTotal()   const{return do_Total;};

        int GetNBands(){return Mapallbands.size();};
        TGraphAsymmErrors *GetBand(int i);
        string GetBandType(int i);

        int GetAlphaSmemberNumDown() const{return AlphaSmemberNumDown;};
        int GetAlphaSmemberNumUp() const{return AlphaSmemberNumUp;};
        string GetAlphaSPDFSetNameDown() const{return AlphaSPDFSetNameDown;};
        string GetAlphaSPDFSetNameUp() const{return AlphaSPDFSetNameUp;};

        TH1D * GetPdfdefault() { return hpdfdefault;};
        TH1D * GetPDFNominal() { return hpdfdefault; };

        TGraphAsymmErrors *GetTotalBand() {
         return h_Total_results;   
        }

        //mutator methods
        void SetDebug(bool _debug);
        void SetGridName(string _gridName);
        void SetSteeringFilePath(string _steeringFilePath);
        void SetSteeringFileDir(string _steeringFileDir);
        void SetSteeringFileName(string _steeringFileName);
        void SetPDFtype(string _PDFtype);
        void SetPDFname(string _PDFname);
        void SetNumPDFMembers(int _n_PDFMembers);
        void SetFillStyleCode(int _fillStyleCode);
        void SetFillColorCode(int _fillColorCode);
        void SetPDFBandType(string _PDFBandType);
        void SetPDFErrorType(string _PDFErrorType);
        //void SetPDFErrorSize(string _PDFErrorSize);

        void SetDoPDFBand(bool _doit);
        void SetDoAlphaS(bool _doit);
        void SetDoScale(bool _doit);
        void SetDoTotError(bool _doit);

        void SetAlphaSmemberNumDown(int _memberNum);
        void SetAlphaSmemberNumUp(int _memberNum);
        void SetAlphaSPDFSetNameDown(string _name);
        void SetAlphaSPDFSetNameUp(string _name);
        //void SetAlphaSPDFSetHistNameDown(string _name);
        //void SetAlphaSPDFSetHistNameUp(string _name);

        void SetAlphaS_value_worldAverage(double setalpha) {alphaS_value_worldAverage=setalpha;};
        void SetAlphaS_absUnc_worldAverage(double setalpha){alphaS_absUnc_worldAverage=setalpha;};
        void SetAlphaS_scale_worldAverage(double setalpha) {alphaS_scale_worldAverage=setalpha;};

        void SetScales(std::vector<double> aRenScales,std::vector<double> aFacScales);

        void ApplyBandCorrection(TGraphAsymmErrors *g, std::string corrlabel, bool includeinband);

    private:
        //VARIABLES
        static bool debug;

        // map of bands
	BandMap_T Mapallbands; // Map to hold uncertainty bands PDF, alphas, scale, total
        //bool correctedgrid;    // flag to indicated if Mapallbands are corrected
                               // by hadronisation, electroweak effects etc.
        // bands for individual uncertainties
        TGraphAsymmErrors *h_PDFBand_results; // PDF uncertainty
        TGraphAsymmErrors *h_AlphaS_results;  // alphas uncertainty
        TGraphAsymmErrors *h_Scale_results;   // scale uncertainty
        TGraphAsymmErrors *h_Total_results;   // total uncertainty

        //string calc_desc;
        bool applgridok;

        std::string steeringFileName;

        string default_pdf_set_name;

        int nLoops;                 // order of the theory prediction. L0=0, NLO=1, NNLO=2
        string PDFtype;             // general name for PDF, used for printing. EX: "MSTW2008nlo"
        string PDFname;             // specific name for PDF, used for looking up a particular PDF. EX: "MSTW2008nlo68cl"
        string PDFnamevar;          // for variation files like HERAPDF
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
        string PDFBandType;
        string PDFErrorType;

        bool f_PDFBandType;
        bool f_PDFErrorSize90Percent;
        string pdfSetPath;

        //PDFErrorPropagation_t ErrorPropagationType;
        int ErrorPropagationType;

        // for HERA type PDFS
        bool includeEIG;  // include experimental eigenvectors
        bool includeQUAD; // include model variations added in quadrature
        bool includeMAX;  // include parameterisation variations take maximum among those

        int AlphaSmemberNumDown; //needed if do_AlphaS is set. Can be read from steering or set by mutator
        int AlphaSmemberNumUp;
        string AlphaSPDFSetNameDown;
        string AlphaSPDFSetNameUp;

        appl::grid *my_grid;

        SPXGrid *spxgrid;

        std::vector<TH1D*> h_errors_AlphaS;
        std::vector<TH1D*> h_errors_PDFBand;
        std::vector<TH1D*> h_errors_Scale;

        TH1D *hpdfdefault;

        std::vector<double> alphaS_variations;  // the values of alphaS variations corresponding to the histograms stored in h_errors_AlphaS

        string gridName;

        //double xscale;
        bool do_PDFBand;
        bool do_AlphaS;
        bool do_Scale;
        bool do_Total;

        //METHODS
        void SetVariablesDefault();
        void SetUpParameters(SPXPDFSteeringFile *psf);
        //void SetSteeringFileNameAndDir(const string _path);
        string GetEnv( const string & var);

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

};


const string defaultPDFSetPath="PDFsets";

#endif
