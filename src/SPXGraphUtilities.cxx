//************************************************************/
//
//	Graph Utilities Implementation
//
//	Some useful graph utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN ATLAS
//	@Date:		20.11.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXGraphUtilities.h"

double SPXGraphUtilities::GetXMin(std::vector<TGraphAsymmErrors *> graphs) {

    double min = 1e30;

    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

        if(xmin < min) {
            min = xmin;
        }
    }

#if PERFORM_DELTA_MIN_MAX == 1
    min -= (min * DELTA_MIN_MAX);
#endif

    return min;
}

double SPXGraphUtilities::GetXMax(std::vector<TGraphAsymmErrors *> graphs) {

    double max = -1e30;

    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

        if(xmax > max) {
            max = xmax;
        }
    }

#if PERFORM_DELTA_MIN_MAX == 1
    max += (max * DELTA_MIN_MAX);
#endif

    return max;
}

double SPXGraphUtilities::GetYMin(std::vector<TGraphAsymmErrors *> graphs) {

    double min = 1e30;

    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

        if(ymin < min) {
            min = ymin;
        }
    }

#if PERFORM_DELTA_MIN_MAX == 1
    min -= (min * DELTA_MIN_MAX);
#endif
    return min;
}

double SPXGraphUtilities::GetYMax(std::vector<TGraphAsymmErrors *> graphs) {

    double max = -1e30;

    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        graphs[i]->ComputeRange(xmin, ymin, xmax, ymax);

        if(ymax > max) {
            max = ymax;
        }
    }

#if PERFORM_DELTA_MIN_MAX == 1
    max += (max * DELTA_MIN_MAX);
#endif
    return max;
}

/*
//@TODO Modify to add the error from g2 to g1 instead of returning a new graph
TGraphAsymmErrors * SPXGraphUtilities::AddYErrors(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2) {
    //Make sure graphs are valid
    if((!g1) || (!g2)) {
        throw SPXGraphException("SPXGraphUtilities::AddYErrors: At least one of the operand graphs is invalid");
    }

    int n1 = g1->GetN();
    int n2 = g2->GetN();

    //Make sure the two graphs are the same size
    if(n1 != n2) {
        std::ostringstream oss;
        oss << "SPXGraphUtilities::AddYErrors: Graphs do not contain the same number of bins: G1 N= " << n1 << " G2 N = " << n2;
        throw SPXGraphException(oss.str());
    }

    //@TODO Pass result as parameter and return that way! Don't call 'new' in a function
    TGraphAsymmErrors *result = new TGraphAsymmErrors();

    double x, y;

    Double_t* 	EYhigh1 = g1->GetEYhigh();
    Double_t* 	EYlow1 =  g1->GetEYlow();
    Double_t* 	EYhigh2 = g2->GetEYhigh();
    Double_t* 	EYlow2 =  g2->GetEYlow();

    for(int i = 0; i < n1; i++) {
        double eyh_tot = 0;
        double eyl_tot = 0;

        eyh_tot = sqrt(pow(EYhigh1[i], 2.0) + pow(EYhigh2[i], 2.0));
        eyl_tot = sqrt(pow(EYlow1[i], 2.0) + pow(EYlow2[i], 2.0));

        result->SetPointEYhigh(i, eyh_tot);
        result->SetPointEYlow(i, eyl_tot);
    }

    return result;
}
*/

//Match binning of slave graph to the binning of the master graph
void SPXGraphUtilities::MatchBinning(TGraphAsymmErrors *master, TGraphAsymmErrors *slave, bool dividedByBinWidth) {

    bool debug = true;

    //Make sure graphs are valid
    if(!master || !slave) {
        throw SPXGraphException("SPXGraphUtilities::MatchBinning: Master and/or slave graph is invalid");
    }

    //Alias for dividedByBinWidth
    bool db = dividedByBinWidth;

    //Get the master/slave binning
    unsigned int m_bins = master->GetN();
    unsigned int s_bins = slave->GetN();

    //Get the master xmin/max
    double m_xmin, m_xmax, m_ymin, m_ymax;
    master->ComputeRange(m_xmin, m_ymin, m_xmax, m_ymax);

    //Remove slave points that are not within the master xmin/max
    for(int i = 0; i < slave->GetN(); ) {
        double s_x, s_y, s_exl, s_exh, s_eyl, s_eyh;

        slave->GetPoint(i, s_x, s_y);
        s_exl = slave->GetErrorXlow(i);
        s_exh = slave->GetErrorXhigh(i);
        s_eyl = slave->GetErrorYlow(i);
        s_eyh = slave->GetErrorYhigh(i);

        if((s_x < m_xmin) || (s_x > m_xmax)) {
            slave->RemovePoint(i);
            i = -1;
        }

        i++;
    }

    //After stripping points
    if(debug) {
        std::cout << "SPXGraphUtilities::MatchBinning: After stripping slave points" << std::endl;
        slave->Print();
        std::cout << std::endl;
    }

    //Match the binning
    for(int i = 0; i < m_bins; i++) {

        double m_x, m_y, m_exl, m_exh, m_eyl, m_eyh;
        double m_bw;

        master->GetPoint(i, m_x, m_y);
        m_exl = master->GetErrorXlow(i);
        m_exh = master->GetErrorXhigh(i);
        m_eyl = master->GetErrorYlow(i);
        m_eyh = master->GetErrorYhigh(i);
        m_exl = m_x - m_exl;
        m_exh = m_x + m_exh;
        m_bw = m_exh - m_exl;

        unsigned int s_count = 0;
        double s_y_sum = 0;
        double s_eyl_sum = 0;
        double s_eyh_sum = 0;

        //Recompute number of slave bins
        s_bins = slave->GetN();

        for(int j = 0; j < s_bins; j++) {
            double s_x, s_y, s_exl, s_exh, s_eyl, s_eyh;
            double s_bw;

            slave->GetPoint(j, s_x, s_y);
            s_exl = slave->GetErrorXlow(j);
            s_exh = slave->GetErrorXhigh(j);
            s_eyl = slave->GetErrorYlow(j);
            s_eyh = slave->GetErrorYhigh(j);
            s_exl = s_x - s_exl;
            s_exh = s_x + s_exh;
            s_bw = s_exh - s_exl;

            //Exception if slave bin width is greater than master bin width
            if(s_bw > m_bw) {
                std::ostringstream oss;
                oss << "SPXGraphUtilities::MatchBinning: (recomputed) Slave Bin " << j << ": (exl, exh) = (" << s_exl << \
                    ", " << s_exh << "): Slave bin width greater than master bin width";
                throw SPXGraphException(oss.str());
            }

            //Exception if there is a phase shift (slave xlow is below master xlow AND slave xhigh is
            //	above, or vice versa for the master xhigh)
            if(((s_exl < m_exl) && (s_exh > m_exl)) || ((s_exh > m_exh) && (s_exl < m_exh))) {
                throw SPXGraphException("SPXGraphUtilities::MatchBinning: Slave graph is phase-shifted with respect to master: Unable to match binning");
            }

            //
            //Slave is NOT phase shifted, and slave bin is equal to or less than master
            //

            //Check if slave point lies within the current master bin
            if((s_x >= m_exl) && (s_x <= m_exh)) {

                //Check for exact match: Do nothing and move on to next master bin
                if(s_bw == m_bw) {
                    break;
                }

                //Bin width is smaller than the master (multiple slave bins per single master bin)
                else {
                    //Count of slave sub bins inside master bin
                    s_count++;
                    std::cout << endl;
                    std::cout << "s_count = " << s_count << std::endl;

                    //If divided by bin width, scale by the slave bin width before summing
                    if(db) {
                        s_y_sum += s_y * s_bw;
                        s_eyl_sum += s_eyl * s_bw;
                        s_eyh_sum += s_eyh * s_bw;
                    } else {
                        s_y_sum += s_y;
                        s_eyl_sum += s_eyl;
                        s_eyh_sum += s_eyh;
                    }

                    std::cout << "Slave point will be removed or modified" << std::endl;
                    std::cout << "slave index = " << j << std::endl;
                    std::cout << "s_x = " << s_x << std::endl;
                    std::cout << "s_y = " << s_y << std::endl;
                    std::cout << "s_exl = " << s_exl << std::endl;
                    std::cout << "s_exh = " << s_exh << std::endl;
                    std::cout << "s_y_sum = " << s_y_sum << std::endl;
                    std::cout << "s_eyl_sum = " << s_eyl_sum << std::endl;
                    std::cout << "s_eyh_sum = " << s_eyh_sum << std::endl;
                }

                //At the end of each master bin recalculate the new slave bin based off the sum of the sub-bins
                if(s_exh == m_exh) {
                    std::cout << "End of master bin " << i << " slave bin " << j << std::endl;
                    slave->Print();
                    std::cout << endl;

                    //New point values
                    double n_x, n_y, n_exl, n_exh, n_eyl, n_eyh;

                    n_exl = m_x - m_exl;
                    n_exh = m_exh - m_x;
                    n_x = (m_exh + m_exl) / 2;

                    //Divided by bin width
                    if(db) {
                        n_y = s_y_sum / m_bw;
                        n_eyl = s_eyl_sum / m_bw;
                        n_eyh = s_eyh_sum / m_bw;
                    } else {
                        n_y = s_y_sum;
                        n_eyl = s_eyl_sum;
                        n_eyh = s_eyh_sum;
                    }

                    double t_x, t_y;
                    slave->GetPoint(j, t_x, t_y);
                    std::cout << "MODIFYING slave point with (index, x, y) = (" << j << ", " << t_x << ", " << t_y << ")" << std::endl;

                    std::cout << "n_x = " << n_x << std::endl;
                    std::cout << "n_y = " << n_y << std::endl;
                    std::cout << "n_exl = " << n_exl << std::endl;
                    std::cout << "n_exh = " << n_exh << std::endl;
                    std::cout << "n_eyl = " << n_eyl << std::endl;
                    std::cout << "n_eyh = " << n_eyh << std::endl;

                    //Set last bin to use new values
                    slave->SetPoint(j, n_x, n_y);
                    slave->SetPointError(j, n_exl, n_exh, n_eyl, n_eyh);

                    // unsigned int rem_count = 0;

                    //Remove the (j - s_count - 1)th index, (s_count - 1) times
                    for(int k = 0; k < (s_count - 1); k++) {
                        unsigned int index = (j - (s_count - 1));

                        double t_x, t_y;
                        double t_exl, t_exh, t_eyl, t_eyh;
                        slave->GetPoint(index, t_x, t_y);
                        t_exl = slave->GetErrorXlow(index);
                        t_exh = slave->GetErrorXhigh(index);
                        t_eyl = slave->GetErrorYlow(index);
                        t_eyh = slave->GetErrorYhigh(index);
                        t_exl = t_x - t_exl;
                        t_exh = t_x + t_exh;

                        std::cout << "REMOVING slave point with (index, x, y, exl, exh, eyl, eyh) = (" << index << ", " << t_x << ", " << t_y << ", " << t_exl << ", " << t_exh << ", " << t_eyl << ", " << t_eyh << ")" << std::endl;
                        slave->RemovePoint(index);
                    }

                    //Remove all sub-bins except last bin
                    // for(int k = (j - (s_count - 1) - rem_count); k < (j - rem_count); k++) {
                    //     double t_x, t_y;
                    //     double t_exl, t_exh, t_eyl, t_eyh;
                    //     slave->GetPoint(k, t_x, t_y);
                    //     t_exl = slave->GetErrorXlow(k);
                    //     t_exh = slave->GetErrorXhigh(k);
                    //     t_eyl = slave->GetErrorYlow(k);
                    //     t_eyh = slave->GetErrorYhigh(k);
                    //     t_exl = t_x - t_exl;
                    //     t_exh = t_x + t_exh;
                    //
                    //     std::cout << "REMOVING slave point with (index, x, y, exl, exh, eyl, eyh) = (" << k << ", " << t_x << ", " << t_y << ", " << t_exl << ", " << t_exh << ", " << t_eyl << ", " << t_eyh << ")" << std::endl;
                    //     slave->RemovePoint(k);
                    //
                    //     rem_count++;
                    //
                    //     std::cout << "rem_count incremented to " << rem_count << std::endl;
                    // }

                    //Move on to next master bin
                    break;
                }
            }
        }
    }

    //Print Graphs
    if(debug) {
        std::cout << "SPXGraphUtilities::MatchBinning: Printing Master Graph" << std::endl;
        master->Print();
        std::cout << std::endl;

        std::cout << "SPXGraphUtilities::MatchBinning: Printing Slave Graph" << std::endl;
        slave->Print();
        std::cout << std::endl;
    }
}

//Divide two graphs
TGraphAsymmErrors * SPXGraphUtilities::Divide(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, DivideErrorType_t dt) {

    //Make sure graphs are valid
    if((!g1) || (!g2)) {
        throw SPXGraphException("SPXGraphUtilities::Divide: At least one of the operand graphs is invalid");
    }

    int n1 = g1->GetN();
    int n2 = g2->GetN();

    //Make sure the two graphs are the same size
    if(n1 != n2) {
        std::ostringstream oss;
        oss << "SPXGraphUtilities::Divide: Graphs do not contain the same number of bins: G1 N= " << n1 << " G2 N = " << n2;
        throw SPXGraphException(oss.str());
    }

    //@TODO Pass result as parameter and return that way! Don't call 'new' in a function
    TGraphAsymmErrors *result = new TGraphAsymmErrors();

    result->SetName(g1->GetName());
    result->SetMarkerStyle(g1->GetMarkerStyle());
    result->SetMarkerColor(g1->GetMarkerColor());
    result->SetLineColor(g1->GetLineColor());

    Double_t   	x1=0.0,   y1=0.0, x2=0.0, y2=0.0;
    Double_t 	dx1h=0.0, dx1l=0.0;
    Double_t 	dy1h=0.0, dy1l=0.0;
    Double_t 	dy2h=0.0, dy2l=0.0;

    Double_t* 	EXhigh1 = g1->GetEXhigh();
    Double_t* 	EXlow1 =  g1->GetEXlow();
    Double_t* 	EYhigh1 = g1->GetEYhigh();
    Double_t* 	EYlow1 =  g1->GetEYlow();

    Double_t* 	EXhigh2 = g2->GetEXhigh();
    Double_t* 	EXlow2 =  g2->GetEXlow();
    Double_t* 	EYhigh2 = g2->GetEYhigh();
    Double_t* 	EYlow2 =  g2->GetEYlow();

    Int_t iv = 0;

    for(Int_t i1 = 0; i1 < n1; i1++) {
        Int_t matchcount = 0;

        for(Int_t i2 = 0; i2 < n2; i2++) {
            g1->GetPoint(i1, x1, y1);
            g2->GetPoint(i2, x2, y2);

            Double_t emean=(EXhigh1[i1] + EXhigh2[i2] + EXlow1[i1] + EXlow2[i2]) / 4.0;

            if(fabs(x1 - x2) > emean) {
                //
            } else {
                matchcount++;

                dx1h = EXhigh1[i1];
                dx1l = EXlow1[i1];

                if(y1 != 0.0) {
                    dy1h = EYhigh1[i1] / y1;
                    dy1l = EYlow1[i1] / y1;
                } else {
                    dy1h = 0;
                    dy1l = 0;
                }

                if(y2 != 0.0) {
                    dy2h = EYhigh2[i2] / y2;
                    dy2l = EYlow2[i2] / y2;
                } else {
                    dy2h = 0;
                    dy2l = 0;
                }

                if(y2 != 0.0) {
                    result->SetPoint(iv, x1, y1/y2);
                } else {
                    result->SetPoint(iv, x1, y2);
                }

                Double_t el = 0.0;
                Double_t eh = 0.0;

                if(dt == ZeroNumGraphErrors) {
                    dy1l = 0.0;
                    dy1h = 0.0;
                }

                if(dt == ZeroDenGraphErrors) {
                    dy2l = 0.0;
                    dy2h = 0.0;
                }

                if((y1 != 0.0) && (y2 != 0.0)) {
                    el = sqrt(dy1l * dy1l + dy2l * dy2l) * (y1 / y2);
                    eh = sqrt(dy1h * dy1h + dy2h * dy2h) * (y1 / y2);
                }

                if(dt == ZeroAllErrors) {
                    result->SetPointError(iv, dx1l, dx1h, 0, 0);
                } else {
                    result->SetPointError(iv, dx1l, dx1h, el, eh);
                }

                iv++;
            }
        }

        if(matchcount > 1) {
            throw SPXGraphException("SPXGraphUtilities::Divide: Too many X-Points matched");
        }
    }

    return result;
}

void SPXGraphUtilities::ClearXErrors(TGraphAsymmErrors * g) {
    for(int i = 0; i < g->GetN(); i++) {
        g->SetPointEXhigh(i, 0.0);
        g->SetPointEXlow(i, 0.0);
    }
}

void SPXGraphUtilities::ClearYErrors(TGraphAsymmErrors * g) {
    for(int i = 0; i < g->GetN(); i++) {
        g->SetPointEYhigh(i, 0.0);
        g->SetPointEYlow(i, 0.0);
    }
}

void SPXGraphUtilities::SetAllXErrors(TGraphAsymmErrors * g, double err) {
    for(int i = 0; i < g->GetN(); i++) {
        g->SetPointEXhigh(i, err);
        g->SetPointEXlow(i, err);
    }
}

void SPXGraphUtilities::SetAllYErrors(TGraphAsymmErrors * g, double err) {
    for(int i = 0; i < g->GetN(); i++) {
        g->SetPointEYhigh(i, err);
        g->SetPointEYlow(i, err);
    }
}

void SPXGraphUtilities::ScaleXErrors(TGraphAsymmErrors * g, double scale) {
    for(int i = 0; i < g->GetN(); i++) {
        double exl, exh;
        exl = g->GetErrorXlow(i);
        exh = g->GetErrorXhigh(i);
        g->SetPointEXlow(i, exl * scale);
        g->SetPointEXhigh(i, exl * scale);
    }
}

void SPXGraphUtilities::ScaleYErrors(TGraphAsymmErrors * g, double scale) {
    for(int i = 0; i < g->GetN(); i++) {
        double eyl, eyh;
        eyl = g->GetErrorYlow(i);
        eyh = g->GetErrorYhigh(i);
        g->SetPointEYlow(i, eyl * scale);
        g->SetPointEYhigh(i, eyl * scale);
    }
}

TGraphAsymmErrors * SPXGraphUtilities::HistogramToGraph(TH1 *h) {

    //Make sure histogram is valid
    if(!h) {
        throw SPXGraphException("SPXGraphUtilities::HistogramToGraph: Histogram provided was invalid");
    }

    //@TODO Don't call 'new' in a function call...
    TGraphAsymmErrors *graph = new TGraphAsymmErrors();

    double x, y, ex, ey;

    for(int i = 0; i < h->GetNbinsX(); i++) {
        y =  h->GetBinContent(i + 1);
        ey = h->GetBinError(i + 1);
        x =  h->GetBinCenter(i + 1);
        ex = h->GetBinWidth(i + 1) / 2.0;

        graph->SetPoint(i, x, y);
        graph->SetPointError(i, ex, ex, ey, ey);
    }

    return graph;
}

double SPXGraphUtilities::GetYBinWidthUnitsScale(std::string master, std::string slave) {
    return GetXUnitsScale(master, slave);
}

//Returns how to scale the SLAVE units to match the MASTER units
double SPXGraphUtilities::GetXUnitsScale(std::string master, std::string slave) {

    bool debug = false;

    if(debug) std::cout << "=======================================================" << std::endl;
    if(debug) std::cout << "         GetXUnitsScale                                " << std::endl;
    if(debug) std::cout << "=======================================================" << std::endl << std::endl;

    if(debug) std::cout << "master = " << master << ", slave = " << slave << std::endl;

    //Possible X units are 'MeV, GeV, TeV'	TeV = 1000x GeV, GeV = 1000x MeV
    //std::vector<std::string> units = {"MEV", "GEV", "TEV"};
    std::vector<std::string> units;
    units.push_back("MEV");
    units.push_back("GEV");
    units.push_back("TEV");

    int masterIndex;
    int slaveIndex;

    //Get the index of the master string
    try {
        masterIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(master));
        if(debug) std::cout << "Found masterIndex = " << masterIndex << std::endl;
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException("SPXGraphUtilities::GetXUnitsScale: Master units (\"" + master + "\") are invalid");
    }

    //Get the index of the slave string
    try {
        slaveIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(slave));
        if(debug) std::cout << "Found slaveIndex = " << slaveIndex << std::endl;
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException("SPXGraphUtilities::GetXUnitsScale: Slave units (\"" + slave + "\") are invalid");
    }

    if(debug) std::cout << "Calculated scale = " << pow(10.0, ((double)(masterIndex - slaveIndex) * 3.0)) << std::endl;

    return pow(10.0, ((double)(masterIndex - slaveIndex) * 3.0));
}

//Returns how to scale the SLAVE units to match the MASTER units
double SPXGraphUtilities::GetYUnitsScale(std::string master, std::string slave) {

    //Possible Y units are 'pb', 'fb'		pb = 1000x fb
    //std::vector<std::string> units = {"FB", "PB"};
        std::vector<std::string> units;
        units.push_back("FB");
        units.push_back("PB");
    int masterIndex;
    int slaveIndex;

    //Get the index of the master string
    try {
        masterIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(master));
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException("SPXGraphUtilities::GetXUnitsScale: Master units (\"" + master + "\") are invalid");
    }

    //Get the index of the slave string
    try {
        slaveIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(slave));
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException("SPXGraphUtilities::GetXUnitsScale: Slave units (\"" + slave + "\") are invalid");
    }

    return pow(10.0, ((double)(masterIndex - slaveIndex) * 3.0));
}

//Scales the TGraph based on the x and y scale
void SPXGraphUtilities::Scale(TGraphAsymmErrors *graph, double xScale, double yScale) {
    double *x = graph->GetX();
    double *y = graph->GetY();
    double *exh = graph->GetEXhigh();
    double *exl = graph->GetEXlow();
    double *eyh = graph->GetEYhigh();
    double *eyl = graph->GetEYlow();

    for(int i = 0; i < graph->GetN(); i++) {
        graph->SetPoint(i, (x[i] * xScale), (y[i] * yScale));
        graph->SetPointError(i, (exl[i] * xScale), (exh[i] * xScale),
            (eyl[i] * yScale), (eyh[i] * yScale));
    }
}

//Normalizes the TGraph and handles bin width division
void SPXGraphUtilities::Normalize(TGraphAsymmErrors *graph, double yBinWidthScale, bool normalizeToTotalSigma, bool divideByBinWidth) {

    //TEMPORARY DEBUG
    bool debug = false;

    //NOTE:
    //Y Bin Width Scale is the scale needed to convert the Y Bin Width Units to the X Units, i.e. If Y Units are [1/TeV] and X Units
    //	are in GeV, yBinWidthScale would = 1e-3
    //normalizeToTotalSigma indicates that the graph should be normalized to the total sigma
    //divideByBinWidth indicates that the graph should be divided by the bin width

    double totalSigma = 0;

    double *pX = graph->GetX();
    double *pY = graph->GetY();
    double *pEXhigh = graph->GetEXhigh();
    double *pEXlow = graph->GetEXlow();
    double *pEYhigh = graph->GetEYhigh();
    double *pEYlow = graph->GetEYlow();

    unsigned int numberOfBins = graph->GetN();

    double sigma = 0;

    //Compute total sigma
    for(int i = 0; i < numberOfBins; i++) {
        sigma = pY[i];
        double binWidth = pEXlow[i] + pEXhigh[i];

        if(debug) std::cout << "Bin[" << i << "]: sigma = " << sigma << std::endl;

        if(debug) std::cout << "Bin[" << i << "]: binWidth = " << binWidth << std::endl;

        if(divideByBinWidth) {
            totalSigma += sigma * binWidth;
        } else {
            totalSigma += sigma;
        }

        //@TODO Check normalization and use of double += sigma here
        //totalSigma += sigma;

        if(debug) std::cout << "Bin[" << i << "]: totalSigma = " << totalSigma << std::endl;
    }

    double scale = 1.0;

    if(normalizeToTotalSigma) {
        scale = 1.0 / totalSigma;
    }

    //Normalize
    for(int i = 0; i < numberOfBins; i++) {

        if(debug) std::cout << "Bin[" << i << "]: scale = "  << scale << std::endl;

        double binWidth = 1;

        if(divideByBinWidth) {
            binWidth = pEXlow[i] + pEXhigh[i];
        }

        if(debug) std::cout << "Bin[" << i << "]: binWidth = " << binWidth << std::endl;

        double y = pY[i] * scale * binWidth;
        double eyl = pEYlow[i] * scale * binWidth;
        double eyh = pEYhigh[i] * scale * binWidth;

        if(divideByBinWidth) {
            binWidth *= yBinWidthScale;
            eyl /= binWidth;
            eyh /= binWidth;
            y /= binWidth;
        }

        if(debug) {
            std::cout << "Bin[" << i << "]:" << std::endl;
            std::cout << "\t x = " << pX[i] << std::endl;
            std::cout << "\t y = " << y << std::endl;
            std::cout << "\t exl = " << pEXlow[i] << std::endl;
            std::cout << "\t exh = " << pEXhigh[i] << std::endl;
            std::cout << "\t eyl = " << eyl << std::endl;
            std::cout << "\t eyh = " << eyh << std::endl;
        }

        graph->SetPoint(i, pX[i], y);
        graph->SetPointError(i, pEXlow[i], pEXhigh[i], eyl, eyh);
    }
}
