// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/ZFinder.hh"

#include "mcgrid/mcgrid.hh"

namespace Rivet {

  using namespace Cuts;


  /// @brief ATLAS Z pT in Drell-Yan events at 7 TeV
  /// @author Elena Yatsenko, Judith Katzy
  class MCgrid_ATLAS_2011_S9131140 : public Analysis {
  public:

    /// Constructor
    MCgrid_ATLAS_2011_S9131140()
      : Analysis("MCgrid_ATLAS_2011_S9131140")
    {
      _sumw_el_bare = 0;
      _sumw_el_dressed = 0;
      _sumw_mu_bare = 0;
      _sumw_mu_dressed = 0;
    }


    /// @name Analysis methods
    //@{

    void init() {

      // Set up projections
      FinalState fs;
      Cut cut = etaIn(-2.4,2.4) & (pT >= 20.0*GeV);

      ZFinder zfinder_dressed_el(fs, cut, PID::ELECTRON, 66.0*GeV, 116.0*GeV, 0.1, ZFinder::CLUSTERNODECAY);
      addProjection(zfinder_dressed_el, "ZFinder_dressed_el");
      ZFinder zfinder_bare_el(fs, cut, PID::ELECTRON, 66.0*GeV, 116.0*GeV, 0.0, ZFinder::NOCLUSTER);
      addProjection(zfinder_bare_el, "ZFinder_bare_el");
      ZFinder zfinder_dressed_mu(fs, cut, PID::MUON, 66.0*GeV, 116.0*GeV, 0.1, ZFinder::CLUSTERNODECAY);
      addProjection(zfinder_dressed_mu, "ZFinder_dressed_mu");
      ZFinder zfinder_bare_mu(fs, cut, PID::MUON, 66.0*GeV, 116.0*GeV, 0.0, ZFinder::NOCLUSTER);
      addProjection(zfinder_bare_mu, "ZFinder_bare_mu");

      // Book histograms
      _hist_zpt_el_dressed     = bookHisto1D(1, 1, 2);  // electron "dressed"
      _hist_zpt_el_bare        = bookHisto1D(1, 1, 3);  // electron "bare"
      _hist_zpt_mu_dressed     = bookHisto1D(2, 1, 2);  // muon "dressed"
      _hist_zpt_mu_bare        = bookHisto1D(2, 1, 3);  // muon "bare"


      // Book APPLgrids
      MCgrid::gridArch arch(50,1,5,0);
      const string PDFname = "MCgrid_ATLAS_2011_S9131140.config";
      
      MCgrid::bookPDF(PDFname, histoDir(), MCgrid::BEAM_PROTON, MCgrid::BEAM_PROTON);

      _a_hist_zpt_mu_bare = MCgrid::bookGrid(_hist_zpt_mu_bare, histoDir(), PDFname, 0, 1E-7, 1, 8315.18, 8315.18, arch);

      _a_hist_zpt_mu_dressed = MCgrid::bookGrid(_hist_zpt_mu_dressed, histoDir(), PDFname, 0, 1E-7, 1, 8315.18, 8315.18, arch);

      _a_hist_zpt_el_bare = MCgrid::bookGrid(_hist_zpt_el_bare, histoDir(), PDFname, 0, 1E-7, 1, 8315.18, 8315.18, arch);

      _a_hist_zpt_el_dressed = MCgrid::bookGrid(_hist_zpt_el_dressed, histoDir(), PDFname, 0, 1E-7, 1, 8315.18, 8315.18, arch);

    }


    /// Do the analysis
    void analyze(const Event& evt) {
      const double weight = evt.weight();

      // Handle APPL event
      MCgrid::PDFHandler::HandleEvent(evt);

      const ZFinder& zfinder_dressed_el = applyProjection<ZFinder>(evt, "ZFinder_dressed_el");
      if (!zfinder_dressed_el.bosons().empty()) {
        _sumw_el_dressed += weight;
        const FourMomentum pZ = zfinder_dressed_el.bosons()[0].momentum();
        _hist_zpt_el_dressed->fill(pZ.pT()/GeV, weight);
        _a_hist_zpt_el_dressed->fill(pZ.pT()/GeV,evt);
      }

      const ZFinder& zfinder_bare_el = applyProjection<ZFinder>(evt, "ZFinder_bare_el");
      if (!zfinder_bare_el.bosons().empty()) {
        _sumw_el_bare += weight;
	    const FourMomentum pZ = zfinder_bare_el.bosons()[0].momentum();
        _hist_zpt_el_bare->fill(pZ.pT()/GeV, weight);
        _a_hist_zpt_el_bare->fill(pZ.pT()/GeV,evt);
      }

      const ZFinder& zfinder_dressed_mu = applyProjection<ZFinder>(evt, "ZFinder_dressed_mu");
      if (!zfinder_dressed_mu.bosons().empty()) {
        _sumw_mu_dressed += weight;
        const FourMomentum pZ = zfinder_dressed_mu.bosons()[0].momentum();
        _hist_zpt_mu_dressed->fill(pZ.pT()/GeV, weight);
        _a_hist_zpt_mu_dressed->fill(pZ.pT()/GeV,evt);
      }

      const ZFinder& zfinder_bare_mu = applyProjection<ZFinder>(evt, "ZFinder_bare_mu");
      if (!zfinder_bare_mu.bosons().empty()) {
        _sumw_mu_bare += weight;
        const FourMomentum pZ = zfinder_bare_mu.bosons()[0].momentum();
        _hist_zpt_mu_bare->fill(pZ.pT()/GeV, weight);
        _a_hist_zpt_mu_bare->fill(pZ.pT()/GeV,evt);
      }

    }


    void finalize() {
      if (_sumw_el_dressed != 0) scale(_hist_zpt_el_dressed, 1/_sumw_el_dressed);
      if (_sumw_el_bare    != 0) scale(_hist_zpt_el_bare,    1/_sumw_el_bare);
      if (_sumw_mu_dressed != 0) scale(_hist_zpt_mu_dressed, 1/_sumw_mu_dressed);
      if (_sumw_mu_bare    != 0) scale(_hist_zpt_mu_bare,    1/_sumw_mu_bare);

      _a_hist_zpt_mu_bare->scale(crossSection()/1/_sumw_mu_bare);
      _a_hist_zpt_mu_dressed->scale(crossSection()/1/_sumw_mu_dressed);
      _a_hist_zpt_el_bare->scale(crossSection()/1/_sumw_el_bare);
      _a_hist_zpt_el_dressed->scale(crossSection()/1/_sumw_el_dressed);

      // Export APPLgrids
      _a_hist_zpt_mu_bare->exportgrid();
      _a_hist_zpt_mu_dressed->exportgrid();
      _a_hist_zpt_el_bare->exportgrid();
      _a_hist_zpt_el_dressed->exportgrid();
      
      // Clear event handler
      MCgrid::PDFHandler::ClearHandler();

    }

    //@}


    private:

	double _sumw_el_bare, _sumw_el_dressed;
	double _sumw_mu_bare, _sumw_mu_dressed;

	Histo1DPtr _hist_zpt_el_dressed;
	Histo1DPtr _hist_zpt_el_bare;
	Histo1DPtr _hist_zpt_mu_dressed;
	Histo1DPtr _hist_zpt_mu_bare;

    // APPLgrids
        MCgrid::gridPtr _a_hist_zpt_mu_bare;
        MCgrid::gridPtr _a_hist_zpt_mu_dressed;
        MCgrid::gridPtr _a_hist_zpt_el_bare;
        MCgrid::gridPtr _a_hist_zpt_el_dressed;

  };


  // Hook for the plugin system
  DECLARE_RIVET_PLUGIN(MCgrid_ATLAS_2011_S9131140);

}
