//************************************************************/
//
//	Graph Utilities Implementation
//
//	Some useful graph utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		20.11.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXGraphUtilities.h"

const std::string cn = "SPXGraphUtilities::";

double SPXGraphUtilities::GetXMin(std::vector<TGraphAsymmErrors *> graphs) {
    std::string mn = "GetYMin: ";
    double min = 1e30;
    
    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        if (!graphs[i]) throw SPXGraphException(cn+"GetXMin: graph not found !");

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
    std::string mn = "GetXMax: ";
    double max = -1e30;


    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        if (!graphs[i]) throw SPXGraphException(cn+"GetXMax: graph not found !");

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
    std::string mn = "GetYMin: ";
    double min = 1e30;



    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        if (!graphs[i]) throw SPXGraphException(cn+"GetYMin: graph not found !");

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
    std::string mn = "GetYMax: ";
    double max = -1e30;



    for(int i = 0; i < graphs.size(); i++) {
        double xmin, xmax, ymin, ymax;

        if (!graphs[i]) throw SPXGraphException(cn+"GetXMax: graph not found !");

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


double SPXGraphUtilities::GetLargestRelativeError(TGraphAsymmErrors* graph) {
 // return systematics in the bin where it is largest
 // the low variation is negative, the high variation is positive 
 std::string mn =" GetLargestRelativeError: ";
 double errormax = 0.;
    
 for (int j = 0; j < graph->GetN(); j++) {
  //Loop over bins
  double x = 0., y = 0., error = 0.;

  graph->GetPoint(j, x, y);
  //error = (graph->GetErrorYlow(j) +  graph->GetErrorYhigh(j))/2.;
  error = fabs(graph->GetErrorYhigh(j));
  if (fabs(graph->GetErrorYlow(j))>error) error=graph->GetErrorYlow(j);
  error/=y; // relative error
  //error = graph->GetErrorYlow(j)/y;

  if (fabs(errormax) < fabs(error)) {
   errormax = error;
  }
  //std::cout<<cn<<"GetLargestRelativeError: j= "<<j<<" error= "<<error<<" errormax= "<<errormax<<std::endl;
 }

 return errormax;
}

TGraphErrors* SPXGraphUtilities::TH1TOTGraph(TH1 *h1)
{
 //
 //convert the histogram h1 into a TGraphErrors
 //
 std::string mn = "TH1TOTGraph: ";
 if (!h1) std::cout <<cn<<mn<< " histogram not found !" << std::endl;

 //else h1->Print();
 //cout<<name<<" n= "<<h1->GetNbinsX()<<endl;
 //TGraphErrors* g1;
 TGraphErrors* g1= new TGraphErrors();
 if (!g1) std::cout<<cn<<mn<<" graph g1 not created "<<std::endl;
 //g1->Print();

 double x, y, ex, ey;
 for (Int_t i=0; i<h1->GetNbinsX(); i++) {
   // cout<<cn<<mn<<<<" i= "<<i<<endl;
   y=h1->GetBinContent(i+1);
  ey=h1->GetBinError(i+1);
   x=h1->GetBinCenter(i+1);
  ex=h1->GetBinWidth(i+1)/2.;

  //cout<<cn<<mn << i<<" x,y = " << x << " " << y << " ex,ey = " << ex << " " << ey << endl;

  g1->SetPoint(i,x,y);
  g1->SetPointError(i,ex,ey);
 }

 return g1;
}


TGraphAsymmErrors* SPXGraphUtilities::TH1TOTGraphAsymm(TH1 *h1)
{
  //
  //convert the histogram h1 into a TGraphAsymmErrors
  //
      std::string mn = "TH1TOTGraphAsymm: ";

      //if(debug) cout<<cn<<"TH1TOTGraphAsymm: start"<<std::endl;

       if (!h1) {
         throw SPXGraphException(cn + mn + "Histogram not found !");
        }

	TGraphAsymmErrors* g1= new TGraphAsymmErrors();
        if (!g1) {
         throw SPXGraphException(cn + mn + "Can not create Graph !");
        }

        g1->SetName(h1->GetName());

	double x, y, ex, ey;
	for (Int_t i=0; i<h1->GetNbinsX(); i++) {
	 y=h1->GetBinContent(i+1);
	 ey=h1->GetBinError(i+1);
	 x=h1->GetBinCenter(i+1);
	 ex=h1->GetBinWidth(i+1)/2.;

         //if(debug) cout<<cn<<mn<<i<<" x,y = "<<x<<" "<<y<<" ex,ey = "<<ex<<" "<<ey<<std::endl;

	 g1->SetPoint(i,x,y);
	 g1->SetPointError(i,ex,ex,ey,ey);
	}

        return g1;
}



TH1D* SPXGraphUtilities::MatchandMultiply(TH1D *hfact, TH1D* hist, bool dividedByBinWidth) {
 std::string mn = "MatchandMultiply: ";
 bool debug=false;
 
 if (debug) {

  std::cout<<cn<<mn<<"Print hist= "<<hist->GetName()<<std::endl;
  hist->Print("all");

  std::cout<<cn<<mn<<"Print hfact= "<<hfact->GetName()<<std::endl;
  hfact->Print("all");

 }

 if (hist->GetNbinsX()!=hfact->GetNbinsX()){
   
  std::cout<<cn<<mn<<"Need to match histograms binning before multiplying "<<
  hist ->GetName()<<" nbin= "<< hist ->GetNbinsX()<<" "<<
  hfact->GetName()<<" nbin= "<< hfact->GetNbinsX()<<std::endl;

  TGraphAsymmErrors *gfact = new TGraphAsymmErrors();
  SPXGraphUtilities::HistogramToGraph(gfact, hfact);
  if (!gfact) {
   throw SPXGraphException(cn+mn+"Graph gfact can not be created !");
  }
  if (debug) {
   std::cout<<cn<<mn<<"Print correction graph "<<std::endl;
   gfact->Print("all");
  }
  //SPXGraphUtilities::HistogramToGraph(gband, hist);
  TH1D* htmp=SPXGraphUtilities::MatchBinning(gfact,hist,dividedByBinWidth);
  hist=htmp;
 }

 hist->Multiply(hfact);

 if (debug) {
  std::cout<<cn<<mn<<"After multiplication Print hist= "<<hist->GetName()<<std::endl;
  hist->Print("all");
 }

 return hist;

}

//Match binning of slave graph to the binning of the master graph
TH1D* SPXGraphUtilities::MatchBinning(TGraphAsymmErrors *master, TH1D *hslave, bool dividedByBinWidth) {
 std::string mn = "MatchBinning: ";
 bool debug = true;

 // Make sure graphs are valid
 if (!master) {
  throw SPXGraphException(cn + mn + "Master graph is invalid");
 }

 if (!hslave) {
  throw SPXGraphException(cn + mn + "Slave histogram is invalid");
 }

 //TGraphAsymmErrors* gslave= SPXGraphUtilities::TH1TOTGraphAsymm(hslave2);
 TGraphAsymmErrors* gslave= SPXGraphUtilities::TH1TOTGraphAsymm(hslave);
 if (!gslave) {
  throw SPXGraphException(cn + mn + "Slave graph could not be created ");
 }

 if (debug) {
  std::cout<<cn<<mn<<"Graph from histogram hslave "<<hslave->GetName()<<" is: "<<std::endl;
  gslave->Print();
 }

 SPXGraphUtilities::MatchBinning(master, gslave,dividedByBinWidth);

 if (debug) {
  std::cout<<cn<<mn<<"After MatchBinning graph gslave "<<gslave->GetName()<<" is: "<<std::endl;
  gslave->Print();
 }

 TH1D* hslave2=SPXGraphUtilities::GraphToHistogram(gslave);
 if (!hslave2) {
  throw SPXGraphException(cn + mn + "Problem converting graph to histogram ");
 }

 if (debug) {
  std::cout<<cn<<mn<<"After MatchBinning histogram hslave2 "<<hslave2->GetName()<<" is: "<<std::endl;
  hslave2->Print("all");
 }

 return hslave2;

}

//Match binning of slave graph to the binning of the master graph
void SPXGraphUtilities::MatchBinning(TGraphAsymmErrors *master, TGraphAsymmErrors *slave, bool dividedByBinWidth) {
    std::string mn = "MatchBinning: ";

    bool debug=false;

    //Make sure graphs are valid
    if (!master) {
     throw SPXGraphException(cn + mn + "Master graph is invalid");
    }

    if (!slave) {
        throw SPXGraphException(cn + mn + "Slave graph is invalid");
    }

    //Alias for dividedByBinWidth
    bool db = dividedByBinWidth;

    if (debug) if (db) std::cout<<cn<<mn<<"Divided by bin width is ON "<<std::endl;

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
        std::cout << cn << mn << "Stripped slave points" << std::endl;
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

        if(debug) {
            std::cout << std::endl;
            std::cout << cn << mn << "Checking master bin number " << i <<" x= "<<m_x<<" xmin= "<<m_exl<<" xmax= "<< m_exh<< std::endl;
        }

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

            if (debug) std::cout<<cn<<mn<<"          Slave bin number " << j <<" x= "<<s_x<<" xmin= "<<s_exl<<" xmax= "<< s_exh<< std::endl;

            //Exception if point lies within master bin AND slave bin width is greater than master bin width
            //double eps=std::numeric_limits<double>::epsilon();
            //double eps=1.e-5;
            if(((s_x >= m_exl) && (s_x <= m_exh)) && (s_bw > m_bw)) {
	    //if(((s_x >= m_exl) && (s_x <= m_exh)) && fabs(s_bw - m_bw)>eps) {
                if (debug) std::cout << std::endl;
                if (debug) std::cout << cn << mn << "Slave bin width greater than master bin width: Printing debug info: " << std::endl;
                if (debug) {
                 std::cout<< cn << mn << "Slave  point (index, x, y, exl, exh, eyl, eyh) = (" << j << ", "; 
		 std::cout.width(10);
		 std::cout<< s_x<< ", " << s_y << ", " << s_exl << ", " << s_exh << ", " << s_eyl << ", " \
                    << s_eyh << ")" << std::endl;
                
                 std::cout << cn << mn << "Master point (index, x, y, exl, exh, eyl, eyh) = (" \
                           << i << ", " << m_x << ", " << m_y << ", " << m_exl << ", " << m_exh << ", " << m_eyl << ", " \
                           << m_eyh << ")" << std::endl;
                }
                std::ostringstream oss;

                oss << cn << mn << "Slave bin width (" << s_bw << ") greater than master bin witdh (" << m_bw << "):" <<
                    "\n\tSlave  Point: (s index, x, exl, exh) = (" << j << ", " << s_x << ", " << s_exl << ", " << s_exh << ")" <<
                    "\n\tMaster Point: (m index, x, exl, exh) = (" << i << ", " << m_x << ", " << m_exl << ", " << m_exh << ")" << std::endl;

		std::cout<<cn<<mn<<"master_bw= "<< m_bw << " slave_bw= "<<s_bw <<std::endl;
		std::cout<<cn<<mn<<"Ratio slave/master= "<< s_bw/m_bw << " "<<(s_bw > m_bw ? " TRUE" : "FALSE")<<std::endl;

                printf("\n s_bw=%e  m_bw=%e ratio= %e \n",s_bw,m_bw, fabs((s_bw-m_bw)/m_bw));

                //double eps=std::numeric_limits<double>::epsilon();
                //if ( fabs((s_bw-m_bw))<eps)
		// std::cout<<cn<<mn<<"diff < eps "<< fabs(s_bw-m_bw) << " eps= "<<eps<<std::endl;
                //else
		// std::cout<<cn<<mn<<"diff > eps "<< fabs(s_bw-m_bw) << " eps= "<<eps<<std::endl;

		std::cerr<<oss.str().c_str()<<std::endl;

                throw SPXGraphException(oss.str());
            } 
	    //else if (debug)
	    // std::cout<<cn<<mn<<"not passed  if(((s_x >= m_exl) && (s_x <= m_exh)) && (s_bw > m_bw)) "<<std::endl; 

            //Exception if there is a phase shift (slave xlow is below master xlow AND slave xhigh is
            //	above, or vice versa for the master xhigh)
            if(((s_exl < m_exl) && (s_exh > m_exl)) || ((s_exh > m_exh) && (s_exl < m_exh))) {
            //double xlow=s_exl - m_exl, xhigh=(s_exh - m_exh);
            //if(((xlow<eps && xlow!=0) &&  (s_exh > m_exl)) || ((xhigh>eps&&xhigh!=0) && (s_exl < m_exh))) {


              printf("\n s_exl= %.15f ",s_exl);
              printf("\n m_exl= %.15f ",m_exl);
              printf("\n s_exh= %.15f ",s_exh);
              printf("\n m_exh= %.15f ",m_exh);

              //if((s_exl - m_exl)<eps && (s_exh > m_exl)) {
              // printf("\n s_exl-m_exl= %.15f   ",s_exl - m_exl);
              // printf("\n s_exh-m_exh= %.15f \n",s_exh - m_exh);
	      //}
	      //if ((s_exh - m_exh)>eps && (s_exl < m_exh)) {
              // printf("\n s_exh-m_exh= %.15f   ",s_exh - m_exh);
              // printf("\n s_exl-m_exl= %.15f \n",s_exl - m_exl);
              //}
	      std::cout<<std::endl;

	      std::cout<<cn<<mn<<" Master: "<<master->GetName()<<std::endl;
              master->Print("all");
	      std::cout<<cn<<mn<<" Slave: "<<slave->GetName()<<std::endl;
              slave->Print("all");
              std::cout<< cn << mn << "Slave point  (index, x, y, exl, exh, eyl, eyh) = (" << j << ", "; 
	      std::cout.width(10);
	      std::cout<< s_x<< ", " << s_y << ", " << s_exl << ", " << s_exh << ", " << s_eyl << ", " \
                        << s_eyh << ")" << std::endl;
                
              std::cout << cn << mn << "Master point (index, x, y, exl, exh, eyl, eyh) = (" \
                        << i << ", " << m_x << ", " << m_y << ", " << m_exl << ", " << m_exh << ", " << m_eyl << ", " \
                        << m_eyh << ")" << std::endl;

              throw SPXGraphException(cn + mn + "Slave graph is phase-shifted with respect to master: Unable to match binning");
            } 
	    //else if (debug)
	    //  std::cout<<cn<<mn<<"not passed if(((s_exl < m_exl) && (s_exh > m_exl)) || ((s_exh > m_exh) && (s_exl < m_exh)))"<<std::endl;


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

                    //If divided by bin width, scale by the slave bin width before summing
                    if(db) {
                        s_y_sum += s_y * s_bw;
                        //s_eyl_sum += pow(s_eyl * s_bw,2);
                        //s_eyh_sum += pow(s_eyh * s_bw,2);

                        s_eyl_sum += s_eyl * s_bw;
                        s_eyh_sum += s_eyh * s_bw;

                    } else {
                        s_y_sum += s_y;
                        //s_eyl_sum += pow(s_eyl,2);
                        //s_eyh_sum += pow(s_eyh,2);
                         s_eyl_sum += s_eyl;
                         s_eyh_sum += s_eyh;
                    }

                    if(debug) {
		      std::cout << cn << mn << "Slave point will be removed or modified s_bw= "<<s_bw << std::endl;
                      std::cout << "\t slave index = " << j << std::endl;
                      std::cout<< "\t s_x = ";        std::cout.width(15); std::cout << s_x << std::endl;
                      std::cout << "\t s_y = ";       std::cout.width(15); std::cout << s_y << std::endl;
                      std::cout << "\t s_exl = ";     std::cout.width(15); std::cout << s_exl << std::endl;
                      std::cout << "\t s_exh = ";     std::cout.width(15); std::cout << s_exh << std::endl;
                      std::cout << "\t s_y_sum = ";   std::cout.width(15); std::cout << s_y_sum << std::endl;
                      std::cout << "\t s_eyl_sum = "; std::cout.width(15); std::cout << s_eyl_sum << std::endl;
                      std::cout << "\t s_eyh_sum = "; std::cout.width(15); std::cout << s_eyh_sum << std::endl;
                    }
                }

                //At the end of each master bin recalculate the new slave bin based off the sum of the sub-bins
                if(s_exh == m_exh) {
                    if(debug) std::cout << cn << mn << "End of master bin " << i << " slave bin " << j << std::endl;

                    //New point values
                    double n_x, n_y, n_exl, n_exh, n_eyl, n_eyh;

                    n_exl = m_x - m_exl;
                    n_exh = m_exh - m_x;
                    n_x = (m_exh + m_exl) / 2;

                    //if (s_eyl_sum>0.)
                    // s_eyl_sum=sqrt(s_eyl_sum);

                    //if (s_eyh_sum>0.)
                    // s_eyh_sum=sqrt(s_eyh_sum);

                    //Divided by bin width
                    if (debug) {
		     if (db) std::cout << cn << mn << "Divided by bin width m_bw= "<<m_bw<<" "<< std::endl;
                    }

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

                    if(debug) {
                        std::cout << cn << mn << "MODIFYING slave point with (index, x, y) = (" << j << ", " << t_x << ", " << t_y << ")" << std::endl;
                        std::cout << cn << mn << "New point values: " << std::endl;
                        std::cout << "\t x = ";   std::cout.width(10); std::cout << n_x << std::endl;
                        std::cout << "\t y = ";   std::cout.width(10); std::cout << n_y << std::endl;
                        std::cout << "\t exl = "; std::cout.width(10); std::cout << n_exl << std::endl;
                        std::cout << "\t exh = "; std::cout.width(10); std::cout << n_exh << std::endl;
                        std::cout << "\t eyl = "; std::cout.width(10); std::cout << n_eyl << std::endl;
                        std::cout << "\t eyh = "; std::cout.width(10); std::cout << n_eyh << std::endl;
                    }

                    //Set last bin to use new values
                    slave->SetPoint(j, n_x, n_y);
                    slave->SetPointError(j, n_exl, n_exh, n_eyl, n_eyh);

                    // unsigned int rem_count = 0;

                    //Remove the (j - s_count - 1)th index, (s_count - 1) times
                    for(int k = 0; k < (s_count - 1); k++) {
                        unsigned int index = (j - (s_count - 1));

                       if(debug) {
                        double t_x, t_y;
                        double t_exl, t_exh, t_eyl, t_eyh;
                        slave->GetPoint(index, t_x, t_y);
                        t_exl = slave->GetErrorXlow(index);
                        t_exh = slave->GetErrorXhigh(index);
                        t_eyl = slave->GetErrorYlow(index);
                        t_eyh = slave->GetErrorYhigh(index);
                        t_exl = t_x - t_exl;
                        t_exh = t_x + t_exh;


			  std::cout << cn << mn << "REMOVING slave point with (index, x, y, exl, exh, eyl, eyh) = ("<< index << ", ";
                          std::cout.width(10); std::cout<< t_x << ", " << t_y << ", " << t_exl << ", " << t_exh << ", " << t_eyl << ", " << t_eyh << ")" << std::endl;
                        }

                        slave->RemovePoint(index);
                    }

                    //Move on to next master bin
                    break;
                }
            }
        }
    }


    // In this matching procedure, last bin might still be different
    // check this
    int nmaster=master->GetN(); 
    int nslave =slave->GetN(); 
    if (nmaster!=nslave) {
     std::cout<<cn<<mn<<"Different number of bins nmaster="<<nmaster<<" nslave= "<<nslave<< std::endl;
     std::cout<<cn<<mn<<"\nDifferent number of bins master: "<< master->GetName()<<std::endl;
     master->Print("all");

     std::cout<<cn<<mn<<"\nDifferent number of bins slave: "<< slave->GetName()<<std::endl;
     slave->Print("all");

     std::ostringstream oss;
     oss<<cn<<mn<<"Different number of bins nmaster="<<nmaster<<" nslave= "<<nslave<< std::endl;     
     throw SPXGeneralException(oss.str());
    }
    
    double s_x, s_y, m_x, m_y;
    slave ->GetPoint(nslave-1,  s_x, s_y);    
    master->GetPoint(nmaster-1, m_x, m_y);    

    if (debug) {
     std::cout<<cn<<mn<<"Number of bins nmaster="<<nmaster<<" nslave= "<<nslave<< std::endl;
     std::cout<<cn<<mn<<"Last bin master: x="<<m_x<<" slave: x= "<<s_x<< std::endl;
    }

    if (s_x!=m_x) {
     std::cout<<cn<<mn<< "Different last bins master="<<m_x<<" slave= "<<s_x<< std::endl;
     std::ostringstream oss;
     oss<<cn<<mn<<"Different last bins master="<<m_x<<" slave= "<<s_x<<std::endl;
     throw SPXGeneralException(oss.str());
    }
    
    //Print Graphs
    if(debug) {
        std::cout << cn << mn << "Printing Master Graph" << master->GetName()<<std::endl;
        master->Print();
        std::cout << std::endl;

        std::cout << cn << mn << "Printing Slave Graph" << slave->GetName() << std::endl;
        slave->Print();
        std::cout << std::endl;
    }
}

//Divide two graphs
TGraphAsymmErrors * SPXGraphUtilities::Divide(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, DivideErrorType_t dt) {
    std::string mn = "Divide: ";

    //Make sure graphs are valid
    if(!g1) {
     throw SPXGraphException(cn + mn + "Graph g1 not found");
    }

    if(!g2) {
     throw SPXGraphException(cn + mn + "Graph g2 not found");
    }

    int n1 = g1->GetN();
    int n2 = g2->GetN();

    //std::cout << cn<<mn<<" n1= " <<n1<<" n2= "<<n2<< std::endl;
    //g1->Print();
    //std::cout << cn<<mn<<" g2= "<<g2<< std::endl;
    //g2->Print();

    //Make sure the two graphs are the same size
    if(n1 != n2) {
     std::cout<<cn<<mn<<"Graph1: "<<g1->GetName()<<std::endl;
     g1->Print();
     std::cout<<cn<<mn<<"Graph2: "<<g2->GetName()<<std::endl;
     g2->Print();

     std::ostringstream oss;
     oss << cn << mn << "Graphs do not contain the same number of bins: Graph1= "
	 << g1->GetName() << " n1= " << n1 <<" Graph2= "<< g2->GetName() << " n2= " << n2;

     throw SPXGraphException(oss.str());
    }

    //std::cout << cn<<mn<<" n1= " <<n1<<" n2= "<<n2<< std::endl;
    
    //@TODO Pass result as parameter and return that way! Don't call 'new' in a function
    TGraphAsymmErrors *result = new TGraphAsymmErrors();
    if (!result) std::cout<<cn<<mn<<"WARNING Error creating ratio graph"<< std::endl;
    result->SetName(g1->GetName());

    //result->SetMarkerStyle(g1->GetMarkerStyle());

    //result->SetMarkerColor(g1->GetMarkerColor());
    //result->SetMarkerSize(g1->GetMarkerSize());

    //result->SetLineColor(g1->GetLineColor());
    //result->SetLineWidth(g1->GetLineWidth());

    //result->SetFillStyle(g1->GetFillStyle());
    //result->SetFillColor(g1->GetFillColor());

    SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors *)g1,(TGraphErrors *)result);

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
            throw SPXGraphException(cn + mn + "Too many X-Points matched");
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

void SPXGraphUtilities::SPXCopyGraphProperties(TGraphErrors* g1, TGraphErrors* g2){
 std::string mn = "SPXCopyGraphProperties: ";
 // copy properties of graph g1 to g2

 if (!g1) throw SPXGraphException(cn + mn + "Graph g1 does not exist ! " );
 if (!g2) throw SPXGraphException(cn + mn + "Graph g2 does not exist ! " );

 g2->SetName       (g1->GetName());

 g2->SetMarkerStyle(g1->GetMarkerStyle());
 g2->SetMarkerColor(g1->GetMarkerColor());

 g2->SetLineColor  (g1->GetLineColor());
 g2->SetLineStyle  (g1->GetLineStyle());
 g2->SetLineWidth  (g1->GetLineWidth());

 g2->SetFillColor  (g1->GetFillColor());
 g2->SetFillStyle  (g1->GetFillStyle());

 return;
}

void SPXGraphUtilities::SPXCopyGraphProperties(TGraphErrors* g1, TH1D* h2){
 std::string mn = "SPXCopyGraphProperties: ";
 // copy properties of graph g1 to histogram h2

 if (!g1) throw SPXGraphException(cn + mn + "Graph g1 does not exist ! " );
 if (!h2) throw SPXGraphException(cn + mn + "Graph h2 does not exist ! " );

 TString hname="histo";
 hname+=g1->GetName();
 h2->SetName       (hname);

 h2->SetMarkerStyle(g1->GetMarkerStyle());
 h2->SetMarkerColor(g1->GetMarkerColor());

 h2->SetLineColor  (g1->GetLineColor());
 h2->SetLineStyle  (g1->GetLineStyle());
 h2->SetLineWidth  (g1->GetLineWidth());

 h2->SetFillColor  (g1->GetFillColor());
 h2->SetFillStyle  (g1->GetFillStyle());


 return;
}


void SPXGraphUtilities::SPXPrintGraphProperties(TGraphErrors* g1){
 std::string mn = "SPXPrintGraphProperties: ";
 // Print properties of graph g1 

 if (!g1) throw SPXGraphException(cn + mn + "Graph g1 does not exist ! " );

 std::cout<<cn<<mn<<" Print properties of graph "<<g1->GetName()<<std::endl;

 std::cout<<cn<<mn<<" MarkerStyle= "<<g1->GetMarkerStyle()<<std::endl;
 std::cout<<cn<<mn<<" MarkerColor= "<<g1->GetMarkerColor()<<std::endl;

 std::cout<<cn<<mn<<" LineStyle= "<<g1->GetLineStyle()<<std::endl;
 std::cout<<cn<<mn<<" LineColor= "<<g1->GetLineColor()<<std::endl;
 std::cout<<cn<<mn<<" LineWidth= "<<g1->GetLineWidth()<<std::endl;

 std::cout<<cn<<mn<<" FillStyle= "<<g1->GetFillStyle()<<std::endl;
 std::cout<<cn<<mn<<" FillColor= "<<g1->GetFillColor()<<std::endl;

 return;
}


void SPXGraphUtilities::SPXPrintHistoProperties(TH1* h1){
 std::string mn = "SPXPrintHistoProperties: ";
 // Print properties of TH1 h1 

 if (!h1) throw SPXGraphException(cn + mn + "Histogram h1 does not exist ! " );

 std::cout<<cn<<mn<<"Print properties of histogram "<<h1->GetName()<<std::endl;

 std::cout<<cn<<mn<<"MarkerStyle= "<<h1->GetMarkerStyle()<<std::endl;
 std::cout<<cn<<mn<<"MarkerColor= "<<h1->GetMarkerColor()<<std::endl;

 std::cout<<cn<<mn<<"LineStyle= "<<h1->GetLineStyle()<<std::endl;
 std::cout<<cn<<mn<<"LineColor= "<<h1->GetLineColor()<<std::endl;
 std::cout<<cn<<mn<<"LineWidth= "<<h1->GetLineWidth()<<std::endl;

 std::cout<<cn<<mn<<"FillStyle= "<<h1->GetFillStyle()<<std::endl;
 std::cout<<cn<<mn<<"FillColor= "<<h1->GetFillColor()<<std::endl;

 return;
}



void SPXGraphUtilities::HistogramToGraph(TGraphAsymmErrors * g, TH1 *h) {
    std::string mn = "HistogramToGraph: ";

    //Make sure histogram is valid
    if(!h) {
        throw SPXGraphException(cn + mn + "Histogram provided was invalid");
    }

    double x, y, ex, ey;

    //Clear the graph first
    ClearGraph(g);

    //The 'i + 1' is to skip the underflow bin, since TH1's start their binning at i = 1
    for(int i = 0; i < h->GetNbinsX(); i++) {
        y =  h->GetBinContent(i + 1);
        ey = h->GetBinError(i + 1);
        x =  h->GetBinCenter(i + 1);
        ex = h->GetBinWidth(i + 1) / 2.0;

        g->SetPoint(i, x, y);
        g->SetPointError(i, ex, ex, ey, ey);
    }
    g->SetName(h->GetName());
}

/*
static void SPXGraphUtilities::CopyHistogramToHistogram(TH1D* h1,TH1D *h2){
 std::string mn = CopyHistogramToHistogram": ";

 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram h1 not found");
 }

 if(!h2) {
  throw SPXGraphException(cn + mn + "Histogram h2 not found");
 }

 h1->Reset();
 
 return;
}
*/

TH1D *SPXGraphUtilities::GraphToHistogram(TGraphAsymmErrors * g) {
    std::string mn = "GraphToHistogram: ";
    //Make sure histogram is valid
    if(!g) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TString name=TString("h")+g->GetName();

    int nbin=g->GetN();

    Double_t xbins[nbin+1];
    double x, y , exl, exh;
    for(int ibin = 0; ibin < nbin; ibin++) {
     g->GetPoint(ibin,x,y);
     exl=g->GetErrorXlow(ibin);
     exh=g->GetErrorXhigh(ibin);
     xbins[ibin]=x-exl;
     //std::cout << cn<<mn<<"xbins["<<ibin<<"]= "<<xbins[ibin] <<std::endl;
    }

    //std::cout << cn<<mn<<"x= "<<x<< " exh= "<< exh<<std::endl;
    xbins[nbin]=x+exh;
    //std::cout << cn<<mn<<"xbins["<<nbin<<"]= "<<xbins[nbin] <<std::endl;

    TH1D *h1 = new TH1D(name,name,nbin,xbins); 

    for(int ibin = 0; ibin < nbin; ibin++) {
        double eyh=g->GetErrorYhigh(ibin);
        double eyl=g->GetErrorYlow(ibin);
        double x, y;
        g->GetPoint(ibin,x,y);

        double ey=(eyh+eyl)/2.;
       	h1->SetBinContent(ibin+1,y);
        h1->SetBinError  (ibin+1,ey);
        //std::cout << cn<<mn<<ibin<<" x= " << x <<" y= "<<y<<" ey= "<<ey<< std::endl;
    }

    //std::cout << cn<<mn<<"Print Histogram: " << h1->GetName()<< std::endl;
    //h1->Print("all");

    return h1;
}


TH1D *SPXGraphUtilities::GraphToHistogramUpError(TGraphAsymmErrors * g) {
    std::string mn = "GraphToHistogramUpError: ";
    //Make sure histogram is valid
    if(!g) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TString name=TString("hUpError")+g->GetName();

    int nbin=g->GetN();

    Double_t xbins[nbin+1];
    double x, y , exl, exh;
    for(int ibin = 0; ibin < nbin; ibin++) {
     g->GetPoint(ibin,x,y);
     exl=g->GetErrorXlow(ibin);
     exh=g->GetErrorXhigh(ibin);
     xbins[ibin]=x-exl;
    }

    xbins[nbin]=x+exh;

    TH1D *h1 = new TH1D(name,name,nbin,xbins); 

    for(int ibin = 0; ibin < nbin; ibin++) {
        double eyh=g->GetErrorYhigh(ibin);
        double eyl=g->GetErrorYlow(ibin);
        double x, y;
        g->GetPoint(ibin,x,y);
        y+=eyh;
        double ey=0.;
        //double ey=(eyh+eyl)/2.;
       	h1->SetBinContent(ibin+1,y);
        h1->SetBinError  (ibin+1,ey);
    }

    return h1;
}

TH1D *SPXGraphUtilities::GraphToHistogramDownError(TGraphAsymmErrors * g) {
    std::string mn = "GraphToHistogramDownError: ";
    //Make sure histogram is valid
    if(!g) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TString name=TString("hDownError")+g->GetName();

    int nbin=g->GetN();

    Double_t xbins[nbin+1];
    double x, y , exl, exh;
    for(int ibin = 0; ibin < nbin; ibin++) {
     g->GetPoint(ibin,x,y);
     exl=g->GetErrorXlow(ibin);
     exh=g->GetErrorXhigh(ibin);
     xbins[ibin]=x-exl;
    }

    xbins[nbin]=x+exh;

    TH1D *h1 = new TH1D(name,name,nbin,xbins); 

    for (int ibin = 0; ibin < nbin; ibin++) {
        double eyh=g->GetErrorYhigh(ibin);
        double eyl=g->GetErrorYlow(ibin);
        double x, y;
        g->GetPoint(ibin,x,y);
        y-=eyl;
        double ey=0.;
        //double ey=(eyh+eyl)/2.;
       	h1->SetBinContent(ibin+1,y);
        h1->SetBinError  (ibin+1,ey);
    }

    return h1;
}

TH1D *SPXGraphUtilities::HistogramSwitchSign(TH1D * h1) {
    std::string mn = "HistogramSwitchSign: ";
    //Make sure histogram is valid
    if(!h1) {
     throw SPXGraphException(cn + mn + "Histogram provided was invalid");
    }

    TString name=TString("hswitchedsign")+h1->GetName();

    TH1D *h2 = (TH1D*) h1->Clone(name);
    int nbin=h1->GetNbinsX();
    for (int ibin = 0; ibin < nbin; ibin++) {
     double y=h1->GetBinContent(ibin+1,y);
     y=-1.*y;
     h2->SetBinContent(ibin+1,y);
    }

    return h2;
}


TH1D *SPXGraphUtilities::GetEdgeHistogram(TGraphAsymmErrors * g, bool low) {
    std::string mn ="GetEdgeHistogram: ";
    //Make sure histogram is valid
    if(!g) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    //TString name=TString("h")+g->GetName();
    TString name=g->GetName();
    if (low) name+="LowEdge";
    else     name+="HighEdge";
    //name+=g->GetName();

    int nbin=g->GetN();

    Double_t xbins[nbin+1];
    double x, y , exl, exh;
    for(int ibin = 0; ibin < nbin; ibin++) {
     g->GetPoint(ibin,x,y);
     exl=g->GetErrorXlow(ibin);
     exh=g->GetErrorXhigh(ibin);
     xbins[ibin]=x-exl;
     //std::cout << cn<<mn<<"xbins["<<ibin<<"]= "<<xbins[ibin] <<std::endl;
    }

    //std::cout << cn<<mn<<"x= "<<x<< " exh= "<< exh<<std::endl;
    xbins[nbin]=x+exh;
    //std::cout << cn<<mn<<"xbins["<<nbin<<"]= "<<xbins[nbin] <<std::endl;

    TH1D *h1 = new TH1D(name,name,nbin,xbins); 
    if (!h1) std::cout << cn<<mn<<"WARNING: Problem creating edge histogram for graph "<<g->GetName()<<std::endl;

    //h1->SetLineColor(g->GetLineColor());
    //h1->SetLineStyle(g->GetLineStyle());
    //h1->SetMarkerColor(g->GetMarkerColor());
    //h1->SetMarkerStyle(g->GetMarkerStyle());
    //h1->SetFillColor(g->GetFillColor());
    //h1->SetFillStyle(g->GetFillStyle());

    SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors*)g, h1);

    for(int ibin = 0; ibin < nbin; ibin++) {
        double eyh=g->GetErrorYhigh(ibin);
        double eyl=g->GetErrorYlow(ibin);
        double x, y;
        g->GetPoint(ibin,x,y);

        double ey=(eyh+eyl)/2.;
        if (low) 
         h1->SetBinContent(ibin+1,y-eyl);
        else
         h1->SetBinContent(ibin+1,y+eyh);

        h1->SetBinError  (ibin+1,0.);
        //std::cout << cn<<mn<<ibin<<" x= " << x <<" y= "<<y<<" ey= "<<ey<< std::endl;
    }

    h1->SetName(name);

    //TKey *key = gDirectory->GetKey(name);
    //if (!key) {
    // std::cout << cn<<mn<<"key not found " << name << std::endl;
    //} else
    // std::cout << cn<<mn<<"key found " << name<< std::endl;

    //std::cout << cn<<mn<<"List gDirectory Histogram: " << h1->GetName()<< std::endl;
    //gDirectory->ls();

    //std::cout << cn<<mn<<"Print Histogram: " << h1->GetName()<< std::endl;
    //h1->Print("all");

    return h1;
}

void SPXGraphUtilities::ClearGraph(TGraphAsymmErrors * g) {
    for(int i = 0; i < g->GetN(); i++) {
        g->RemovePoint(i);
    }
}

double SPXGraphUtilities::GetYBinWidthUnitsScale(std::string master, std::string slave) {
    return GetXUnitsScale(master, slave);
}

//Returns how to scale the SLAVE units to match the MASTER units
double SPXGraphUtilities::GetXUnitsScale(std::string master, std::string slave) {
    std::string mn = "GetXUnitsScale: ";

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
        if(debug) std::cout << cn << mn << "Found masterIndex = " << masterIndex << std::endl;
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException(cn + mn + "Master units (\"" + master + "\") are invalid");
    }

    //Get the index of the slave string
    try {
        slaveIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(slave));
        if(debug) std::cout << cn << mn << "Found slaveIndex = " << slaveIndex << std::endl;
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException(cn + mn + "Slave units (\"" + slave + "\") are invalid");
    }

    if (debug) std::cout << cn << mn << "Calculated scale = " << pow(10.0, ((double)(masterIndex - slaveIndex) * 3.0)) << std::endl;

    return pow(10.0, ((double)(masterIndex - slaveIndex) * 3.0));
}

//Returns how to scale the SLAVE units to match the MASTER units
double SPXGraphUtilities::GetYUnitsScale(std::string master, std::string slave) {
    std::string mn = "GetYUnitsScale: ";
    bool debug = false;

    if(debug) std::cout << "=======================================================" << std::endl;
    if(debug) std::cout << "         GetYUnitsScale                                " << std::endl;
    if(debug) std::cout << "=======================================================" << std::endl << std::endl;

    if(debug) std::cout << "master = " << master << ", slave = " << slave << std::endl;

    //Possible Y units are 'nb', 'pb', 'fb'		pb = 1000x fb
    std::vector<std::string> units;
    units.push_back("NB");
    units.push_back("PB");
    units.push_back("FB");

    int masterIndex;
    int slaveIndex;

    //Get the index of the master string
    try {
        masterIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(master));
        if (debug) std::cout << cn << mn << "Found masterIndex = " << masterIndex << std::endl;
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException(cn + mn + "Master units (\"" + master + "\") are invalid");
    }

    //Get the index of the slave string
    try {
        slaveIndex = SPXStringUtilities::GetIndexOfStringInVector(units, SPXStringUtilities::ToUpper(slave));
        if (debug) std::cout << cn << mn << "Found slaveIndex = " << slaveIndex << std::endl;
    } catch(const SPXException &e) {
        std::cerr << e.what() << std::endl;

        throw SPXGraphException(cn + mn + "Slave units (\"" + slave + "\") are invalid");
    }

    if (debug) std::cout << cn << mn << "Calculated scale = " << pow(10.0, ((double)(masterIndex - slaveIndex) * 3.0)) << std::endl;
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

//In this case, dividedByBinWidth tells whether the Y values of the graph g HAVE ALREADY been
//  divided by the bin width
double SPXGraphUtilities::GetTotalSigma(TGraphAsymmErrors *g, bool dividedByBinWidth) {
    std::string mn = "GetTotalSigma: ";

    if(!g) {
        throw SPXGraphException(cn + mn + "Graph is Invalid");
    }

    double totalSigma = 0;

    double *y = g->GetY();
    double *exh = g->GetEXhigh();
    double *exl = g->GetEXlow();

    for(int i = 0; i < g->GetN(); i++) {
        double binWidth = exh[i] + exl[i];

        //std::cout << cn << mn << "y [" << i << "] = " << y[i] << std::endl;

        if(dividedByBinWidth) {
	  //std::cout << cn << mn << " * binWidth [" << i << "] = " << binWidth << std::endl;
            totalSigma += y[i] * binWidth;
        } else {
            totalSigma += y[i];
        }

        //std::cout << cn << mn << "totalSigma = " << totalSigma << std::endl;
    }

    return totalSigma;
}


//Normalized the graph by the bin width for each bin
void SPXGraphUtilities::DivideByBinWidth(TGraphAsymmErrors *g) {
    std::string mn = "DivideByBinWidth: ";

    if(!g) {
        throw SPXGraphException(cn + mn + "Graph is Invalid");
    }

    double * y = g->GetY();
    double * exl = g->GetEXlow();
    double * exh = g->GetEXhigh();
    double * eyl = g->GetEYlow();
    double * eyh = g->GetEYhigh();

    //Loop over graph and divided by the bin width
    for(int i = 0; i < g->GetN(); i++) {
        double binWidth = exh[i] + exl[i];

        //Scale y, eyl, and eyh by 1 / bin width
        y[i] /= binWidth;
        eyl[i] /= binWidth;
        eyh[i] /= binWidth;
    }

    return;
}

//Divide histogram by the bin width for each bin
TH1D* SPXGraphUtilities::DivideByBinWidth(TH1D *h1) {
 std::string mn = "DivideByBinWidth:TH1D: ";

 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram is Invalid");
 }

 TString hname=h1->GetName();
 hname+="dividedbybinwidth";
 TH1D *h2=(TH1D*) h1->Clone(hname);
 if(!h2) {
  throw SPXGraphException(cn + mn + "Histogram h2 is Invalid");
 }

 int nbin=h1->GetNbinsX();
 for (int ibin = 0; ibin < nbin; ibin++) {

  double val= h1->GetBinContent(ibin+1);
  double eval= h1->GetBinError(ibin+1);
  double binWidth =h1->GetBinCenter(ibin+1);
  if (binWidth!=0.) {
   val/=binWidth;
   eval/=binWidth;
  }
  
  //std::cout<<cn<<mn<<" ibin= "<<ibin<<" binwidth= "<<binWidth<<" val= "<<val<<std::endl;

  h2->SetBinContent(ibin+1,val);
  h2->SetBinError  (ibin+1,eval);

 }

 return h2;
}


//Divide histogram by the bin width for each bin
TH1D* SPXGraphUtilities::DivideByBinWidth(TH1D *h1, std::vector <double> binw) {
 std::string mn = "DivideByBinWidth:vector: ";

 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram is Invalid");
 }

 TString hname=h1->GetName();
 hname+="dividedbybinwidth";
 TH1D *h2=(TH1D*) h1->Clone(hname);
 if(!h2) {
  throw SPXGraphException(cn + mn + "Histogram h2 is Invalid");
 }

 int nbin=h1->GetNbinsX();

 if(nbin!=binw.size()-1) {
  std::ostringstream oss;
  oss << cn <<mn<<"Number of bins in histogram nbin= "<<nbin<<" and vector "<<binw.size()-1<<" not the same ";
  std::cout<<oss.str()<<std::endl;
  throw SPXGraphException(oss.str());
 }

 for (int ibin = 0; ibin < nbin; ibin++) {

  double val= h1->GetBinContent(ibin+1);
  double eval= h1->GetBinError(ibin+1);
  double binWidth = binw.at(ibin+1)-binw.at(ibin);
  if (binWidth!=0.) {
   val/=binWidth;
   eval/=binWidth;
  }
  
  //std::cout<<cn<<mn<<" ibin= "<<ibin<<" binwidth= "<<binWidth<<" val= "<<val<<std::endl;

  h2->SetBinContent(ibin+1,val);
  h2->SetBinError  (ibin+1,eval);

 }

 return h2;
}


 void SPXGraphUtilities::Add(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, int noerr) {
 //
 // Add graph g1 and graph g2
 // result is stored in g1
 //
 // noerr=0: add error from graph 2 in quadrature 
 //       1: do not consider error from graph 2
 // 
 std::string mn = "Add: ";
 bool debug=false;

 int nBins1 = g1->GetN();
 int nBins2 = g2->GetN();
 if (nBins1!=nBins2) {
  std::ostringstream oss;
  oss << cn <<mn<<"WARNING: Graph g1 "<<g1->GetName()<<" NBin1= "<<nBins1
                   <<" and graph g2 "<<g2->GetName()<<" NBin2= "<<nBins2
                   <<" do not have the same lenght ! ";
  throw SPXGeneralException(oss.str());
 }

 double *x1   = g1->GetX();
 double *y1   = g1->GetY();
 double *exl1 = g1->GetEXlow();
 double *exh1 = g1->GetEXhigh();
 double *eyl1 = g1->GetEYlow();
 double *eyh1 = g1->GetEYhigh();

 double *x2   = g2->GetX();
 double *y2   = g2->GetY();
 double *exl2 = g2->GetEXlow();
 double *exh2 = g2->GetEXhigh();
 double *eyl2 = g2->GetEYlow();
 double *eyh2 = g2->GetEYhigh();

 //Loop over the smallest of the two
 unsigned int n = (nBins1 < nBins2 ? nBins1 : nBins2);

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 before: " <<g1->GetName()<< std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2 before: " <<g2->GetName()<< std::endl;
  g2->Print();

 }

 for(int i = 0; i < nBins1; i++) {
   bool nomatch=true;
  for(int j = 0; j < nBins2; j++) {

   //Check for exact bin match
   // std::cout<<"Old x1="<<x1[i]  <<" x2="<<x2[j]
   //          <<" exl1= "<<exl1[i]<<" exh1= "<<exh1[i]
   //          <<" exl2= "<<exl2[j]<<" exh2= "<<exh2[j]
   //          << std::endl;

    if(((x1[i] - exl1[i]) == (x2[j]-exl2[j])) && ((x1[i] + exh1[i]) == (x2[j]+exh2[j]))) {
    //double emean=(dx1+dx2)/2.;
    //if (fabs(x1-x2)>=emean && fabs(x1-x2)>dx 
    nomatch=false;

    //Bins match; Scale y, eyl, and eyh
    y1[i] += y2[j];

    // errors add up in quadrature
    double eyl=0., eyh=0.;
    // do not modify the graph, these are pointers !
    if (noerr==1) {eyl=0.; eyh=0.;}

    if (y1[i]*y1[i]!=0. && y2[i]*y2[i]!=0.) eyl=sqrt(y1[i]*y1[i]+y2[i]*y2[i]);

    if (y2[i]*y2[i]!=0.) eyh=sqrt(y1[i]*y1[i]+y2[i]*y2[i]);

    if (debug) std::cout << cn << mn << "Bins Match (i, j): (" << i << ", " << j << ")" 
                        <<" New y1= "<<y1[i]<<" eyl= "<<eyl<<" eyh= "<<eyh<< std::endl;

    g1->SetPoint(i, x1[i],y1[i]);
    g1->SetPointError(i,exl1[i],exh1[i],eyl,eyh);
    break;
   }; 
  };
  if(nomatch) std::cout << cn << mn << "No Bins Match for bin i= " << i << std::endl;  
 };

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 after " << g1->GetName()<<std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2 before: " <<g2->GetName()<< std::endl;
  g2->Print();
 }
 return;
}

 TGraphAsymmErrors * SPXGraphUtilities::AddGraphs(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, int noerr) {
 //
 // Add graph g1 and graph g2 and create new graph
 // new graph is returned
 //
 // noerr=0: add error from graph 2 in quadrature 
 //       1: do not consider error from graph 2
 // 
 std::string mn = "Add: ";
 bool debug=false;

 int nBins1 = g1->GetN();
 int nBins2 = g2->GetN();
 if (nBins1!=nBins2) {
  std::ostringstream oss;
  oss << cn <<mn<<"WARNING: Graph g1 "<<g1->GetName()<<" NBin1= "<<nBins1
                   <<" and graph g2 "<<g2->GetName()<<" NBin2= "<<nBins2
                   <<" do not have the same lenght ! ";
  throw SPXGeneralException(oss.str());
 }

 double *x1   = g1->GetX();
 double *y1   = g1->GetY();
 double *exl1 = g1->GetEXlow();
 double *exh1 = g1->GetEXhigh();
 double *eyl1 = g1->GetEYlow();
 double *eyh1 = g1->GetEYhigh();

 double *x2   = g2->GetX();
 double *y2   = g2->GetY();
 double *exl2 = g2->GetEXlow();
 double *exh2 = g2->GetEXhigh();
 double *eyl2 = g2->GetEYlow();
 double *eyh2 = g2->GetEYhigh();

 TGraphAsymmErrors* g3= new TGraphAsymmErrors();
 TString gname=g1->GetName();
 gname+=g2->GetName();

 g3->SetName(gname);

 //Loop over the smallest of the two
 unsigned int n = (nBins1 < nBins2 ? nBins1 : nBins2);

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 before: " <<g1->GetName()<< std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2 before: " <<g2->GetName()<< std::endl;
  g2->Print();

 }

 for(int i = 0; i < nBins1; i++) {
  bool nomatch=true;
  for(int j = 0; j < nBins2; j++) {

   //Check for exact bin match
   // std::cout<<"Old x1="<<x1[i]  <<" x2="<<x2[j]
   //          <<" exl1= "<<exl1[i]<<" exh1= "<<exh1[i]
   //          <<" exl2= "<<exl2[j]<<" exh2= "<<exh2[j]
   //          << std::endl;

    if(((x1[i] - exl1[i]) == (x2[j]-exl2[j])) && ((x1[i] + exh1[i]) == (x2[j]+exh2[j]))) {
    //double emean=(dx1+dx2)/2.;
    //if (fabs(x1-x2)>=emean && fabs(x1-x2)>dx 
    nomatch=false;

    //Bins match; Scale y, eyl, and eyh
    double y3=y1[i] + y2[j];

    // errors add up in quadrature
    double eyl=0., eyh=0.;
    eyl=eyl1[i]*eyl1[i]+eyl2[i]*eyl2[i];
    if (eyl>0) eyl=sqrt(eyl);

    eyh=eyh1[i]*eyh1[i]+eyh2[i]*eyh2[i];
    if (eyh>0) eyh=sqrt(eyh);

    if (noerr==1) {eyl=0.; eyh=0.;}

    if (debug) std::cout << cn << mn << "Bins Match (i, j): (" << i << ", " << j << ")" 
                        <<" New y3= "<<y3<<" eyl= "<<eyl<<" eyh= "<<eyh<< std::endl;

    g3->SetPoint(i, x1[i],y3);
    g3->SetPointError(i,exl1[i],exh1[i],eyl,eyh);
    break;
   }; 
  };
  if(nomatch) std::cout << cn << mn << "No Bins Match for bin i= " << i << std::endl;  
 };

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1: " << g1->GetName()<<std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2: " <<g2->GetName()<< std::endl;
  g2->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g3: " <<g3->GetName()<< std::endl;
  g3->Print();
 }
 return g3;
}

//
 TGraphAsymmErrors * SPXGraphUtilities::CopyGraph(TGraphAsymmErrors *g1) {
 //
 // Copy graph g1 to graph g2 
 // new graph is returned
 //
 // 
 std::string mn = "Copy: ";
 bool debug=false;

 int nBins1 = g1->GetN();

 double *x1   = g1->GetX();
 double *y1   = g1->GetY();
 double *exl1 = g1->GetEXlow();
 double *exh1 = g1->GetEXhigh();
 double *eyl1 = g1->GetEYlow();
 double *eyh1 = g1->GetEYhigh();

 TGraphAsymmErrors* g2= new TGraphAsymmErrors();
 TString gname="Copyof";
 gname+=g1->GetName();
 g2->SetName(gname);


 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 before: " <<g1->GetName()<< std::endl;
  g1->Print();

 }

 for(int i = 0; i < nBins1; i++) {

  g2->SetPoint(i, x1[i],y1[i]);
  g2->SetPointError(i,exl1[i],exh1[i],eyl1[i],eyh1[i]); 
 
 };

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1: " <<g1->GetName()<<std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2: " <<g2->GetName()<< std::endl;
  g2->Print();

 }
 return g2;
}
//


void  SPXGraphUtilities::Add(TH1D *h1, TH1D *h2){
 //
 // Add histogram h2 to h1
 //
 std::string mn = "Add: ";
 bool debug=false;

  if (!h1) {
   throw SPXGeneralException(cn+mn+"Histogram h1 not found !");   
  }

  if (!h2) {
   throw SPXGeneralException(cn+mn+"Histogram h2 not found !");   
  }
 
  TString hname=h1->GetName();
  hname+="_+_";
  hname+=h2->GetName(); 
  //h1->Clone(hname)
  h1->Add(h2);
  h1->SetName(hname);

  return;
};

void SPXGraphUtilities::Multiply(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, int noerr) {
 //
 // multiply graph g1 and graph g2
 // result is stored in g1
 //
 // noerr=0: add error from graph 2 in quadrature 
 //       1: do not consider error from graph 2
 // 
 std::string mn = "Multiply: ";
 bool debug=false;

 int nBins1 = g1->GetN();
 int nBins2 = g2->GetN();
 if (nBins1!=nBins2) {
  std::ostringstream oss;
  oss << cn <<mn<<"WARNING: Graph g1 "<<g1->GetName()<<" NBin1= "<<nBins1
                   <<" and graph g2 "<<g2->GetName()<<" NBin2= "<<nBins2
                   <<" do not have the same lenght ! ";
  throw SPXGeneralException(oss.str());
 }

 double *x1   = g1->GetX();
 double *y1   = g1->GetY();
 double *exl1 = g1->GetEXlow();
 double *exh1 = g1->GetEXhigh();
 double *eyl1 = g1->GetEYlow();
 double *eyh1 = g1->GetEYhigh();

 double *x2   = g2->GetX();
 double *y2   = g2->GetY();
 double *exl2 = g2->GetEXlow();
 double *exh2 = g2->GetEXhigh();
 double *eyl2 = g2->GetEYlow();
 double *eyh2 = g2->GetEYhigh();

 //Loop over the smallest of the two
 unsigned int n = (nBins1 < nBins2 ? nBins1 : nBins2);

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 before: " <<g1->GetName()<< std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2 before: " <<g2->GetName()<< std::endl;
  g2->Print();

 }

 for(int i = 0; i < nBins1; i++) {
   bool nomatch=true;
  for(int j = 0; j < nBins2; j++) {

   //Check for exact bin match
   // std::cout<<"Old x1="<<x1[i]  <<" x2="<<x2[j]
   //          <<" exl1= "<<exl1[i]<<" exh1= "<<exh1[i]
   //          <<" exl2= "<<exl2[j]<<" exh2= "<<exh2[j]
   //          << std::endl;

    if(((x1[i] - exl1[i]) == (x2[j]-exl2[j])) && ((x1[i] + exh1[i]) == (x2[j]+exh2[j]))) {
    //double emean=(dx1+dx2)/2.;
    //if (fabs(x1-x2)>=emean && fabs(x1-x2)>dx 
    nomatch=false;

    //Bins match; Scale y, eyl, and eyh
    y1[i] *= y2[j];

    // relative errors add up in quadrature
    double eyl=0., eyh=0.;
    // do not modify the graph, these are pointers !
    double myeyl2=eyl2[i], myeyh2=eyh2[i]; 
    if (noerr==1) {myeyl2=0.; myeyh2=0.;}

    if (y1[i]*y1[i]!=0. && y2[i]*y2[i]!=0.)
     eyl=sqrt(eyl1[i]*eyl1[i]/(y1[i]*y1[i])+myeyl2*myeyl2/(y2[i]*y2[i]))*(y1[i]*y2[i]);

    if (y2[i]*y2[i]!=0.)
     eyh=sqrt(eyh1[i]*eyh1[i]/(y1[i]*y1[i])+myeyh2*myeyh2/(y2[i]*y2[i]))*(y1[i]*y2[i]);

    if (debug) std::cout << cn << mn << "Bins Match (i, j): (" << i << ", " << j << ")" 
                        <<" New y1= "<<y1[i]<<" eyl= "<<eyl<<" eyh= "<<eyh<< std::endl;

    g1->SetPoint(i, x1[i],y1[i]);
    g1->SetPointError(i,exl1[i],exh1[i],eyl,eyh);
    break;
   }; 
  };
  if(nomatch) std::cout << cn << mn << "No Bins Match for bin i= " << i << std::endl;  
 };

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 after " << g1->GetName()<<std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2 before: " <<g2->GetName()<< std::endl;
  g2->Print();
 }
}


int SPXGraphUtilities::CompareValues(TGraphAsymmErrors *g1, TGraphAsymmErrors *g2, bool bandsize) {
 //
 // returns number of bins where graph g1 has larger value than graph g2 
 // 
 std::string mn = "CompareValues: ";
 bool debug=false;

 int nBins1 = g1->GetN();
 int nBins2 = g2->GetN();
 if (nBins1!=nBins2) {
  std::ostringstream oss;
  oss << cn <<mn<<"WARNING: Graph g1 "<<g1->GetName()<<" NBin1= "<<nBins1
                   <<" and graph g2 "<<g2->GetName()<<" NBin2= "<<nBins2
                   <<" do not have the same lenght ! ";
  throw SPXGraphException(oss.str());
 }

 double *x1   = g1->GetX();
 double *y1   = g1->GetY();
 double *exl1 = g1->GetEXlow();
 double *exh1 = g1->GetEXhigh();
 double *eyl1 = g1->GetEYlow();
 double *eyh1 = g1->GetEYhigh();

 double *x2   = g2->GetX();
 double *y2   = g2->GetY();
 double *exl2 = g2->GetEXlow();
 double *exh2 = g2->GetEXhigh();
 double *eyl2 = g2->GetEYlow();
 double *eyh2 = g2->GetEYhigh();

 //Loop over the smallest of the two
 unsigned int n = (nBins1 < nBins2 ? nBins1 : nBins2);

 if (debug) {
  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g1 before: " <<g1->GetName()<< std::endl;
  g1->Print();

  std::cout << " " << std::endl;
  std::cout << cn << mn << "Print g2 before: " <<g2->GetName()<< std::endl;
  g2->Print();

 }

 int nlargerbins=0;

 for(int i = 0; i < nBins1; i++) {
   bool nomatch=true;
  for(int j = 0; j < nBins2; j++) {

   //Check for exact bin match
   // std::cout<<"Old x1="<<x1[i]  <<" x2="<<x2[j]
   //          <<" exl1= "<<exl1[i]<<" exh1= "<<exh1[i]
   //          <<" exl2= "<<exl2[j]<<" exh2= "<<exh2[j]
   //          << std::endl;

    if(((x1[i] - exl1[i]) == (x2[j]-exl2[j])) && ((x1[i] + exh1[i]) == (x2[j]+exh2[j]))) {
    //double emean=(dx1+dx2)/2.;
    //if (fabs(x1-x2)>=emean && fabs(x1-x2)>dx 
    nomatch=false;

    if (bandsize) {
     //if (y1[i]+eyh1[i]>y2[j]+eyh2[j] || y1[i]-eyl1[i]<y2[j]-eyl2[j]) nlargerbins++;

    //double yh=max((y1[i]+eyh1[i]),(y2[j]+eyh2[j]));
    //double yl=max((y1[i]-eyl1[i]),(y2[j]-eyl2[j]));

      double yh=std::max(eyh1[i],eyh2[j]);
      double yl=std::max(eyl1[i],eyl2[j]);

    //std::cout<<i<<" x= "<< x1[i]<<" match yh="<<yh  <<" yl="<<yl<<std::endl;
     
     if (fabs(yh)>fabs(yl)) {
      //if (y1[i]+eyh1[i]>y2[j]+eyh2[j]) nlargerbins++;
      if (eyh1[i]>eyh2[j]) nlargerbins++;
      //std::cout<<" high match y1="<<y1[i]+eyh1[i]  <<" y2="<<y2[j]+eyh2[j]<<" nlargerbins= "<<nlargerbins<<std::endl;
     } else {
      //if (y1[i]-eyl1[i]>y2[j]-eyl2[j]) nlargerbins++;
      if (eyl1[i]>eyl2[j]) nlargerbins++;
      //std::cout<<"low match y1="<<y1[i]-eyl1[i]  <<" y2="<<y2[j]-eyl2[j]<<" nlargerbins= "<<nlargerbins<<std::endl;
     }

    } else {
     if (y1[i]>y2[j] ) nlargerbins++;
     //std::cout<<"match y1="<<y1[i]  <<" y2="<<y2[j]<<" nlargerbins= "<<nlargerbins<<std::endl;
    }
    break;
   }; 
  };
  if(nomatch) std::cout << cn << mn << "No Bins Match for bin i= " << i << std::endl;  
 };

 return nlargerbins;
}

void SPXGraphUtilities::StaggerGraph(int index, int ngraph, TGraphAsymmErrors *graph) {
 std::string mn = "StaggerGraph: ";
 bool debug=false;

 double dx= pow(-1,index) * int((index+1)/2.);

 if (debug) std::cout << cn << mn <<index<<" dx= " <<dx <<" ngraph= "<<ngraph<<std::endl;

 for(int j = 0; j < graph->GetN(); j++) {
  //Loop over bins
  double x = 0., y = 0., range = 0., error = 0.;
  double exh = 0., exl = 0., dr = 0., newX = 0.;

  graph->GetPoint(j, x, y);
  error = graph->GetErrorXlow(j) +  graph->GetErrorXhigh(j);

  dr = dx* (error/float(ngraph));
  dr = dr - dr/2.;
  newX = x + dr;
  if (debug) std::cout << cn << mn <<j<<" dr= " <<dr<<" x= "<<x<<" newX= "<<newX<< " error= "<<error<<std::endl;

  exh=graph->GetErrorXhigh(j)-dr;
  exl=graph->GetErrorXlow(j)+dr;

  graph->SetPoint(j, newX, y);
  graph->SetPointEXhigh(j, exh);
  graph->SetPointEXlow (j, exl);
 }	

 return;
}

void SPXGraphUtilities::AddtoBand(TGraphErrors* g1, TGraphAsymmErrors* g2, bool addinquadrature) {
 //
 // add the graph g1 to the band g2
 // 
 // addinquadrature=true:  added in quadrature
 // addinquadrature=false: errors are taken 
 //
 std::string mn = "AddtoBand: ";
 bool debug=false;

 Double_t  x1=0., y1=0.,  y2=0., y0=0;

 if (g1->GetN()!=g2->GetN()) {
  std::cout<<cn<<mn<<"WARNING: Graphs have not the same # of elements g1= " << g1->GetN() <<" g2 = "<<g2->GetN()<< std::endl;
  return;
 }

 Double_t* EYhigh = g2-> GetEYhigh();
 Double_t* EYlow  = g2-> GetEYlow();

 for (Int_t i=0; i<g1->GetN(); i++) {
  g1->GetPoint(i, x1,y1);
  g2->GetPoint(i, x1,y2);

  Double_t eyh=0., eyl=0.;

  if (debug) printf("%d: y1=%f y2=%f Eyhigh= %f Eylow= %f \n",i,y1,y2,EYhigh[i],EYlow[i]);

  y0=y1-y2;
  if (y0>=0){
   eyh=EYhigh[i];
   if (addinquadrature) 
    eyh=sqrt(eyh*eyh+y0*y0);
   else
    if(fabs(y0)>eyh) eyh=fabs(y0);

   if (debug) printf("high: %d: y0=%f eyh=%f  \n",i,y0,eyh);
   g2->SetPointEYhigh(i,eyh);
  } else {
   eyl=EYlow[i];
   if (addinquadrature) 
    eyl=sqrt(eyl*eyl+y0*y0);
   else
    if(fabs(y0)>eyl) eyl=fabs(y0);

   if (debug) printf("low: %d: y0=%f eyl=%f  \n",i,y0,eyl);
   g2->SetPointEYlow (i,eyl);
  }
 }
 return;
}

void SPXGraphUtilities::AddinQuadrature(TGraphAsymmErrors* g1, TGraphAsymmErrors* g2, bool takesign) {
 //
 // add the errors of g2 to the graph g1
 //
 std::string mn = "AddinQuadrature: ";
 bool debug=false;

 Double_t  x1=0., y1=0.,  y2=0., y0=0;

 if (!g1) {
  std::cout<<cn<<mn<<"WARNING: Graphs g1 not found | "<< std::endl;
  return;
 }

 if (!g2) {
  std::cout<<cn<<mn<<"WARNING: Graphs g2 not found | "<< std::endl;
  return;
 }

 if (g1->GetN()!=g2->GetN()) {
  std::cout<<cn<<mn<<"WARNING: Graphs have not the same # of elements g1= " << g1->GetN() <<" g2 = "<<g2->GetN()<< std::endl;
  return;
 }

 //if (takesign) std::cout<<cn<<mn<<"WARNING takesign option not yet implemented "<<std::endl;

 Double_t* EYhigh1 = g1-> GetEYhigh();
 Double_t* EYlow1  = g1-> GetEYlow();

 Double_t* EYhigh2 = g2-> GetEYhigh();
 Double_t* EYlow2  = g2-> GetEYlow();

 for (Int_t i=0; i<g1->GetN(); i++) {
  Double_t eyh=0., eyl=0.;

  if (takesign) {
   if (EYhigh1[i]<0 && EYlow1[i]>0) {
    Double_t x1=0., y1=0.;
    g1->GetPoint(i, x1,y1);
    if (y1==0.) 
     std::cout<<cn<<mn<<"INFO: switch signs for g1= "<<g1->GetName()<<" EYhigh1["<<i<<"]= " << EYhigh1[i]<<" EYlow1["<<i<<"]= "<<EYlow1[i]<< std::endl;   
    else
     std::cout<<cn<<mn<<"INFO: switch signs for relative g1= "<<g1->GetName()<<" EYhigh1["<<i<<"]= " << EYhigh1[i]/y1 <<" EYlow1["<<i<<"]= "<<EYlow1[i]/y1<< std::endl;   
    double tmp=EYhigh1[i];
    EYhigh1[i]= EYlow1[i];
    EYlow1[i]=tmp;
   }

   if (EYhigh2[i]<0 && EYlow2[i]>0) {
    Double_t x2=0., y2=0.;
    g2->GetPoint(i, x2,y2);
    if (y2==0.) 
     std::cout<<cn<<mn<<"INFO: switch signs for g2= "<<g2->GetName()<<" EYhigh2["<<i<<"]= " << EYhigh2[i] <<" EYlow2["<<i<<"]= "<<EYlow2[i]<< std::endl;   
    else
     std::cout<<cn<<mn<<"INFO: switch signs for relative g2= "<<g2->GetName()<<" EYhigh2["<<i<<"]= " << EYhigh2[i]/y2 <<" EYlow2["<<i<<"]= "<<EYlow2[i]/y2<< std::endl;   
    double tmp=EYhigh2[i];
    EYhigh2[i]= EYlow2[i];
    EYlow2[i]=tmp;
   }
  }

  if ((EYhigh2[i]>0 && EYlow2[i]>0) || (EYhigh2[i]<0 && EYlow2[i]<0)) {
   Double_t x2=0., y2=0.;
   g2->GetPoint(i, x2,y2);
   if (y2==0.) y2=1.;
   if (EYhigh2[i]!=0 || EYlow2[i]!=0)
    std::cout<<cn<<mn<<"INFO: low and high errors have the same sign signs  g2= "<<g2->GetName()
                     <<" relative EYhigh2["<<i<<"]= " << EYhigh2[i]/y2 <<" EYlow2["<<i<<"]= "<<EYlow2[i]/y2<< std::endl;   
  }

  eyh=sqrt(EYhigh1[i]*EYhigh1[i]+EYhigh2[i]*EYhigh2[i]);
  eyl=sqrt(EYlow1[i]*EYlow1[i]+EYlow2[i]*EYlow2[i]);

  g1->SetPointEYlow (i,eyl);
  g1->SetPointEYhigh(i,eyh);

 }
 return;
}

void SPXGraphUtilities::SetColors(TGraphAsymmErrors* g1, Color_t icol) {
 std::string mn = "SetColors: ";
 //
 if (!g1) {
  std::cout<<cn<<mn<<"WARNING Graph g1 not found ! "<<std::endl;
  return;
 }

 g1->SetLineColor(icol);
 g1->SetFillColor(icol);
 g1->SetMarkerColor(icol);

 return;
}

TGraphAsymmErrors* SPXGraphUtilities::FindCommonBins(TGraphAsymmErrors* gmaster, TGraphAsymmErrors* gslave) {
 //
 // Merge bins in gslave to bins given in gmaster
 // gslave must have finer binning than gmaster
 //
 std::string mn = "FindCommonBins: ";
 const bool debug=false; 
 
 if (!gmaster){
  std::cout<<mn<<"Graph gmaster does not exist ! "<< std::endl; 
  throw SPXGraphException(cn + mn + "Master graph is invalid");
  return (TGraphAsymmErrors*)0; 
 }
 if (!gslave) {
  std::cout<<mn<<"Graph gslave does not exist ! "<< std::endl; 
  throw SPXGraphException(cn + mn + "Slave graph is invalid");
  return (TGraphAsymmErrors*)0; 
 }

 int nmaster=gmaster->GetN();
 int nslave =gmaster->GetN();
 
 //
 //if (nmaster>nslave) {
 //  std::cout<<cn<<mn<<"WARNING can not merge nmaster= "<<nmaster<<" nslave= "<<nslave<<std::endl; 
 // return 0;
 //}
 //

 TGraphAsymmErrors* g3= new TGraphAsymmErrors();
 if (!g3) {std::cout<<cn<<mn<<"Problem to make new vector g3 ! " << std::endl; return 0;};
 SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors*)gslave,(TGraphErrors*)g3);

 //TGraphAsymmErrors* g4= new TGraphAsymmErrors();
 //if (!g4) {std::cout<<cn<<mn<<"Problem to make new vector g4 ! " << std::endl; return 0;};
 
 double* Xmaster = gmaster->GetX();
 double* Ymaster = gmaster->GetY();
 double* EXhighmaster = gmaster->GetEXhigh();
 double* EXlowmaster =  gmaster->GetEXlow();

 Double_t* Xslave = gslave->GetX();
 Double_t* Yslave = gslave->GetY();
 Double_t* EXhighslave=gslave->GetEXhigh();
 Double_t* EXlowslave= gslave->GetEXlow();
 Double_t* EYhighslave=gslave->GetEYhigh();
 Double_t* EYlowslave= gslave->GetEYlow();

 int inew=0;
 for (int imaster=0; imaster<nmaster; imaster++) {  //loop over points of master
  for (int islave=0; islave<nslave; islave++) {  //loop over points of slave

   if (debug) {
    std::cout<<cn<<mn<<" Xslave["<<islave<<"]= "<<Xslave[islave]<<" Xslave["<<islave+1<<"]= "<<Xslave[islave+1]<<std::endl;
    std::cout<<cn<<mn<<" Xmaster["<<imaster<<"]= "<<Xmaster[imaster]<<" Xmaster["<<imaster+1<<"]= "<<Xmaster[imaster+1]<<std::endl;
   }

   double dxminslave = Xslave[islave]-EXlowslave[islave];
   double dxmaxslave = Xslave[islave]+EXhighslave[islave];
   double dxminmaster=Xmaster[imaster]-EXlowmaster[imaster];
   double dxmaxmaster=Xmaster[imaster]+EXhighmaster[imaster];

   if (dxminslave==dxminmaster && dxmaxslave==dxmaxmaster) {
    if (debug) {
     std::cout<<cn<<mn<<"Match "<<" Xslave["<<islave<<"]= "<<Xslave[islave]
 			        <<" Yslave["<<islave<<"]= "<<Yslave[islave]<<std::endl; 
     std::cout<<cn<<mn<<"Match "<<" Xmaster["<<imaster<<"]= "<<Xmaster[imaster]
			        <<" Ymaster["<<imaster<<"]= "<<Ymaster[imaster]<<std::endl; 
    }

    g3->SetPoint     (inew,  Xslave[islave]  ,Yslave[islave] ); 
    g3->SetPointError(inew, EXlowslave[islave],EXhighslave[islave], EYlowslave[islave],EYhighslave[islave]);
    inew++;
   } 
  }
 }

 /*
 double x=0., dx=0.;
 double gysum, gy, gx, dgx;
 double xmin=0, xmax=0;
 const int Nslice=10;

 double BIG=1.e20;
 double x1minp=BIG, x1maxp=-BIG, x1minm=BIG, x1maxm=-BIG;
 
 for (int imaster=0; imaster<nmaster; imaster++) {  //loop over points of master

  double xi=Xmaster[imaster]-EXlowmaster[imaster];
  double xj=Xmaster[imaster]+EXhighmaster[imaster]; 

  if (debug) std::cout<<cn<<mn<<imaster<<" Xi[imaster]= "<<xi<<" Xmaster[imaster+1]= "<< xj <<" Ymaster= "<<Ymaster[imaster]<<std::endl;

  if   (xi>xj){xmin=xj; xmax=xi; }
  else        {xmin=xi; xmax=xj; }
  dgx=(xmax-xmin)/Nslice;
  gysum=0.;
  
  int iflag=0;
  for (int islave=0; islave<Nslice; islave++) {  //loop over points of slave
   gx=xmin+dgx/2.+islave*dgx;
   TSpline *spline=0;
   gy=g4->Eval(gx,spline,"S");
   // if vector does not cover full range of bin, set to zero
   // no extrapolation
   if (gx>0) {if (gx<=x1minp||gx>x1maxp) iflag=1;}
   else      {if (gx<=x1minm||gx>x1maxm) iflag=1;}
   if (iflag==1) {gy=0.; gysum=0.;}

   gysum+=gy; 
   if (debug) std::cout<<cn<<mn<<imaster<<" iflag= "<<iflag<<" gx= "<<gx<<" gysum= "<<gysum<<" gy= "<<gy<<std::endl;
  }

 gysum/=Nslice;
  x= Xmaster[imaster]; double dxlow=(x-xmin); double dxhigh=(xmax-x);

  if (debug) std::cout<<cn<<mn<<imaster<<" fill: x= "<<x<<" dx= "<<dx<<" gy="<<gysum<<" gy/dx= "<<gysum/dx<<endl;
  g3->SetPoint     (imaster, x  , gysum);
  g3->SetPointError(imaster,dxlow,dxhigh,0.,0.);
 }

 delete g4;
 */
 
 if (debug){
  std::cout<<cn<<mn<<"Print new graph "<<g3->GetName()<<std::endl;
  g3->Print();
 }
 return g3;
}

bool SPXGraphUtilities::GraphWithNoError(TGraphAsymmErrors * g) {
    std::string mn = "GraphWithNoError: ";
    //Make sure graph is valid
    if(!g) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }
    bool allerrorszero=true;
    int nbin=g->GetN();
    double eyl, eyh;
    for(int ibin = 0; ibin < nbin; ibin++) {
     eyl=g->GetErrorYlow(ibin);
     eyh=g->GetErrorYhigh(ibin);
     if (eyl!=0 || eyh!=0) return false;
    }
    return allerrorszero;
}

TH1D* SPXGraphUtilities::SPXZeroErrors(TH1* h1){
 std::string mn = "SPXZeroErrors: ";
 //
 // zero y-errors in a histogram
 //
 if (!h1) {
  throw SPXGraphException(cn + mn + "Graph provided was invalid");
 }
 
 TString name="nom";
 name+=h1->GetName();

 TH1D *h2=(TH1D*) h1->Clone(name);

 for (int i = 0; i < h1->GetNbinsX(); i++) {
  h2->SetBinError(i+1,0.);
 }
 return h2;
}


TGraphAsymmErrors * SPXGraphUtilities::SPXTGraphSetXErrorsZero(TGraphAsymmErrors *g1){
    std::string mn = "SPXTGraphSetXErrorsZero: ";

    if (!g1) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TGraphAsymmErrors* g2= new TGraphAsymmErrors();
    g2->SetName(g1->GetName());
    SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors *)g1,(TGraphErrors *)g2);

    Double_t x=0., y=0.;
    for (Int_t i=0; i<g1->GetN(); i++) {
     g1->GetPoint(i,x,y);
     g2->SetPoint(i,x,y);
     double eyl=g1->GetErrorYlow(i);
     double eyh=g1->GetErrorYhigh(i);
     g2->SetPointError(i,0.,0.,eyl,eyh);

    }
    return g2;
}
TGraphAsymmErrors * SPXGraphUtilities::SPXTGraphSetYErrorsZero(TGraphAsymmErrors *g1){
    std::string mn = "SPXTGraphSetYErrorsZero: ";

    if (!g1) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TGraphAsymmErrors* g2= new TGraphAsymmErrors();
    g2->SetName(g1->GetName());
    SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors *)g1,(TGraphErrors *)g2);

    Double_t x=0., y=0.;
    for (Int_t i=0; i<g1->GetN(); i++) {
     g1->GetPoint(i,x,y);
     g2->SetPoint(i,x,y);
     double exl=g1->GetErrorXlow(i);
     double exh=g1->GetErrorXhigh(i);
     g2->SetPointError(i,exl,exh,0.,0.);

    }
    return g2;
}

TGraphErrors * SPXGraphUtilities::SPXTGraphSetYErrorsZero(TGraphErrors *g1){
    std::string mn = "SPXTGraphSetYErrorsZero: ";

    if (!g1) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TGraphErrors* g2= new TGraphErrors();
    g2->SetName(g1->GetName());
    SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors *)g1,(TGraphErrors *)g2);

    Double_t x=0., y=0.;
    for (Int_t i=0; i<g1->GetN(); i++) {
     g1->GetPoint(i,x,y);
     g2->SetPoint(i,x,y);
     double ex=g1->GetErrorX(i);
     g2->SetPointError(i,ex,0.);

    }
    return g2;
}

TGraphErrors * SPXGraphUtilities::SPXTGraphSetXErrorsZero(TGraphErrors *g1){
    std::string mn = "SPXTGraphSetXErrorsZero: ";

    if (!g1) {
     throw SPXGraphException(cn + mn + "Graph provided was invalid");
    }

    TGraphErrors* g2= new TGraphErrors();
    g2->SetName(g1->GetName());
    SPXGraphUtilities::SPXCopyGraphProperties((TGraphErrors *)g1,(TGraphErrors *)g2);

    Double_t x=0., y=0.;
    for (Int_t i=0; i<g1->GetN(); i++) {
     g1->GetPoint(i,x,y);
     g2->SetPoint(i,x,y);
     double ey=g1->GetErrorY(i);
     g2->SetPointError(i,0.,ey);

    }
    return g2;
}

TVectorD* SPXGraphUtilities::SPXHistogramToVector( TH1D* h_ ){
  std::string mn = "SPXHistogramToVector: ";

  if (!h_) {
   throw SPXGraphException(cn+mn+"Histogram provided was invalid");
  }

  TVectorD* v_ = new TVectorD( h_->GetNbinsX() );
  for (Int_t i=0; i<h_->GetNbinsX(); i++) {
    (*v_)[i] = h_->GetBinContent(i+1);
  }
  return v_;
}


TH1D* SPXGraphUtilities::CalculateRelativeUncertainty(TH1D* h1, TH1D* h2){
  std::string mn = "CalculateRelativeUncertainty:";
  // calculate h3=(h1-h2)/h2
  if (!h1) {
   throw SPXGraphException(cn+mn+"First histogram provided was invalid");
  }

  if (!h2) {
   throw SPXGraphException(cn+mn+"Second histogram provided was invalid");
  }

  TString hname="relU";
  hname+=h1->GetName();
  TH1D* h3=(TH1D*) h1->Clone(hname); 
  if (!h3) {
   throw SPXGraphException(cn+mn+"Result histogram can not be created !");
  }

  h3->Add(h2,-1.);
  h3->Divide(h2);
  
  return h3;
}

bool SPXGraphUtilities::SPXHistosHaveSameMean(TH1* h1, TH1D *h2){
 std::string mn = "SPXHistoAreEqual: ";
 //
 // Test if h1 and h2 have the same mean value
 //
 bool showinfo=true;

 // Print properties of TH1 h1 
 if (!h1) {
  throw SPXGraphException(cn+mn+"First histogram provided was invalid");
 }

 if (!h2) {
  throw SPXGraphException(cn+mn+"Second histogram provided was invalid");
 }

 if (h1->GetNbinsX() != h2->GetNbinsX()) {
  if (showinfo) std::cout<<cn<<mn<<"Different number of h1 bins= "<<h1->GetNbinsX()<<" h2 bins= "<<h2->GetNbinsX()<<std::endl;
  return false;
 }

 for (int i = 0; i < h1->GetNbinsX(); i++) {
  double y1= h1->GetBinContent(i + 1);
  double y2= h2->GetBinContent(i + 1);
  if ( y1 != y2 ) {
   if (showinfo) std::cout<<cn<<mn<<"Difference in bin= "<<i<<" h1= "<<y1<<" h2= "<<y2<<std::endl;
   return false;
  }
 }
 return true;
} 

/*
// This method is not yet tests
TH1D* SPXGraphUtilities::HistogramCutXAxis(TH1D * h1, double xmin, double xmax){
 std::string mn = "HistogramCutXAxis: ";
 //Make sure histogram is valid
 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram provided was invalid");
 }

 TString name=TString("hxaxixcut")+h1->GetName();

 // TH1D *h2 = (TH1D*) h1->Clone(name);
 int nbin=h1->GetNbinsX();
 vector <double> vy;
 vector <double> vey;
 vector <double> vx;
 //vector <double> vex;

 for (int ibin = 0; ibin < nbin; ibin++) {
  double x=h1->GetBinCenter(ibin+1);
  if (x>xmin && x<=xmax) {
   vy.push_back(h1->GetBinContent(ibin+1));
   vey.push_back(h1->GetBinError(ibin+1));
   vx.push_back(h1->GetBinCenter(ibin+1));
  }
 }

 TH1D *h2= new TH1D(name,h1->GetTitle(),vy.size(),vy.at(0), vy.back());
 h2->SetEntries(h1->GetEntries());

 std::cout<<cn<<mn<<" Print new histgram: "<<std::endl;
 h2->Print("all");

 for (int i=0; i<vy.size(); i++) {
  h2->SetBinContent(i+1,vy.at(i));  
  h2->SetBinError(i+1,vey.at(i));  
 }

 return h2;
}
*/
 

TH1D* SPXGraphUtilities::SPXHistogramCutXAxis(TH1D * h1, int ixmin, int ixmax){
 std::string mn = "HistogramCutXAxis:TH1D: ";
 //Make sure histogram is valid
 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram provided was invalid");
 }

 TString name=TString("hxaxixcut")+h1->GetName();

 int nbin=h1->GetNbinsX();
 std::vector <double> vy;
 std::vector <double> vey;

 for (int ibin = 0; ibin < nbin; ibin++) {
  if (ibin>=ixmin && ibin<ixmax) {
   vy.push_back(h1->GetBinContent(ibin+1));
   vey.push_back(h1->GetBinError(ibin+1));
  }
 }
 
 TH1D *h2= new TH1D(name,h1->GetTitle(),vy.size(),0., vy.size());

 //std::cout<<cn<<mn<<" Print new histgram: "<<std::endl;
 //h2->Print("all");

 for (int i=0; i<vy.size(); i++) {
  h2->SetBinContent(i+1,vy.at(i));  
  h2->SetBinError(i+1,vey.at(i));  
 }

 h2->SetEntries(h1->GetEntries());

 return h2;
}

TH2D* SPXGraphUtilities::SPXHistogramCutXAxis(TH2D * h1, int ixmin, int ixmax){
 std::string mn = "HistogramCutXAxis:TH2D: ";
 //
 // Make sure histogram is valid
 //
 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram provided was invalid");
 }

 TString hname=TString("hxcut")+h1->GetName();

 int nbinx=h1->GetNbinsX();
 int nbiny=h1->GetNbinsY();

 std::vector<TH1D*>  vtmp;

 for (int ibin = 0; ibin < nbinx; ibin++) {
  if (ibin>=ixmin && ibin<ixmax) {
   TString pname="projectionbin";
   pname+=ibin+1;
   pname+=h1->GetName();
   TH1D *htmp=h1->ProjectionY(pname,ibin+1,ibin+1);
   vtmp.push_back(htmp);

   //std::cout<<cn<<mn<<" Print htmp for ibin= "<<ibin<<std::endl;
   //htmp->Print("all");

  }
 }

 //std::cout<<cn<<mn<<"New number of bins on x-axis= "<<vtmp.size()<<std::endl;

 TH2D *h2= new TH2D(hname,h1->GetTitle(),vtmp.size(),0.,vtmp.size(),nbiny,0.,nbiny);

 //std::cout<<cn<<mn<<" Print new histgram: "<<std::endl;
 //h2->Print("all");

 for (int ix = 0; ix < vtmp.size(); ix++) {

  if (!vtmp.at(ix)) {
   throw SPXGraphException(cn + mn + "Histogram provided was invalid");
  }

  for (int iy = 0; iy < nbiny; iy++) {
   double val= vtmp.at(ix)->GetBinContent(iy+1);
   double eval=vtmp.at(ix)->GetBinError(iy+1);
   h2->SetBinContent(ix+1,iy+1,val);  
   h2->SetBinError  (ix+1,iy+1,eval);  
  }
 }

 h2->SetEntries(h1->GetEntries());

 /*
 TCanvas *c1= new TCanvas("old","old");
 h1->Draw("colz");

 TCanvas *c2= new TCanvas("new","new");
 h2->Draw("colz");
 */

 for (int ix = 0; ix < vtmp.size(); ix++) {
  delete vtmp.at(ix);
 }

 return h2;
}

TH1D* SPXGraphUtilities::SPXTransformXaxisToIndex(TH1D * h1){
 std::string mn = "SPXTransformXaxisToIndex: ";
 //
 // Transforms the xaxis given in float to an index
 //

 //Make sure histogram is valid
 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram provided was invalid");
 }

 TString hname=TString("indexed")+h1->GetName();

 int nbin=h1->GetNbinsX();
 TH1D *h2= new TH1D(hname,h1->GetTitle(),nbin,0., nbin);
 if(!h2) {
  throw SPXGraphException(cn + mn + "Histogram h2 not created !");
 }
 for (int i=0; i<nbin; i++) {
   h2->SetBinContent(i+1,h1->GetBinContent(i+1));  
   h2->SetBinError  (i+1,h1->GetBinError(i+1));  
 }

 h2->SetEntries(h1->GetEntries());

 return h2;
}




double  SPXGraphUtilities::SPXCompareHistograms(TH1D * h1, TH1 *h2, double reltolerance, bool debug){
 std::string mn = "SPXCompareHistograms: ";
 //
 // Compares the value of two histograms
 //
 //Make sure histogram is valid
 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram h1 is invalid");
 }

 if(!h2) {
  throw SPXGraphException(cn + mn + "Histogram h2 is invalid");
 }

 int n1bin=h1->GetNbinsX();
 int n2bin=h2->GetNbinsX();

 if(n1bin!=n2bin) {
  std::ostringstream oss;
  oss<<cn<<mn<<"Histograms have different number of bins n1bin= "<<n1bin<<" n2bin= "<<n2bin <<" "<< std::endl;     
  throw SPXGeneralException(oss.str());
 }

 double dev=0.;
 for (int ibin = 0; ibin < n1bin; ibin++) {
  double c1=h1->GetBinCenter(ibin+1);
  double c2=h2->GetBinCenter(ibin+1);
  if(c1!=c2) {
   std::ostringstream oss;
   oss<<cn<<mn<<"Histograms have different bin center c1= "<<c1<<" c2= "<<c2 << std::endl;     
   throw SPXGeneralException(oss.str());
  }

  double val1= h1->GetBinContent(ibin+1);
  double val2= h2->GetBinContent(ibin+1);
  double diff=0.;

  if (val1!=0) {
   diff=fabs(val1-val2)/val1;

   if (diff>=reltolerance) {
    dev+=diff;
   }
  } else {
   if (val2!=0) {
    std::cout<<cn<<mn<<"WARNING: val1=0, but not val2= "<<val2<<std::endl;
    return HUGE_VAL;
   }
  }

  if (debug)
   std::cout<<cn<<mn<<"val1= "<<val1<<" val2= "<<val2<<" diff= "<<diff<<" rel= "<<val1/val2<<" summed deviation= "<<dev<<std::endl;
 }
 
 return dev;
};

TMatrixD * SPXGraphUtilities::SPXGetMatrix(TH2D * h1){
 std::string mn = "SPXGetMatrix: ";

 if(!h1) {
  throw SPXGraphException(cn + mn + "Histogram is invalid");
 }

 int nbinx=h1->GetNbinsX();
 int nbiny=h1->GetNbinsY();

 TMatrixT<double> *matrix = new TMatrixT<double>(nbinx,nbiny);
 if (!matrix) {
  throw SPXGraphException(cn + mn + "Matrix can not be created !");
 }

 for (int ix = 0; ix < nbinx; ix++) {
  for (int iy = 0; iy < nbiny; iy++) {
    double val=h1->GetBinContent(ix+1,iy+1);
    (*matrix)(ix,iy)=val;
  }
 }

 //TString name="m";
 //name+=h1->GetName();
 //matrix->SetName(name);
 
 return matrix;
}

bool SPXGraphUtilities::SPXCheckGraphBinning(TGraphErrors *g1, std::vector <double > bins){
 std::string mn = "SPXCheckGraphBinning: ";
 //
 // Check of graph has same binning as given by vector bins
 // Check bin centers 
 // vector is supposed to be the bin boundary

 //Make sure graph is valid

 if(!g1) {
  throw SPXGraphException(cn + mn + "TGraph g1 is invalid");
 }

 int n1bin=g1->GetN();
 int n2bin=bins.size()-1;

 if(n1bin!=n2bin) {
  std::ostringstream oss;
  oss<<cn<<mn<<"TGraph has different number of bins in graph= "<<n1bin<<" but vector bin= "<<n2bin <<" "<< std::endl;     
  std::cout<<oss.str()<<std::endl;

  std::cout<<cn<<mn<<"Graph g1: "<<std::endl;
  g1->Print();

  std::cout<<cn<<mn<<"vector bins: "<< n2bin <<std::endl;
  for (int i=0; i<bins.size()-1; i++) { // vector has one element more since it is supposed to be the bin boundary
   double xm=(bins.at(i)+bins.at(i+1))/2;
   std::cout<<cn<<mn<<i<<" bins: "<<bins.at(i)<<" xm= "<<xm<<std::endl;
   //std::cout<<cn<<mn<<i<<" bins: "<<bins.at(i)<<std::endl;
  }

  throw SPXGeneralException(oss.str());
 }
 
 bool ok=true;

 double x=0., y=0.;
 for (Int_t i=0; i<g1->GetN(); i++) {
  g1->GetPoint(i,x,y);
  //double ey=g1->GetErrorY(i);
  //g2->SetPointError(i,0.,ey);

  double xm=(bins.at(i)+bins.at(i+1))/2;

  //std::cout<<cn<<mn<<i<<" x= "<<x<<" xm= "<<xm<<" y= "<<y<<std::endl;

  if (x!=xm) ok=false;

 }

 return ok;
}

double SPXGraphUtilities::SPXCompareTMatrices(TMatrixD * m1, TMatrixD *m2, double reltolerance, bool debug){
 std::string mn = "SPXCompareTMatrices: ";
 //
 // Compares the value of two TMatrices
 //
 // Make sure matrix is valid
 //
 if(!m1) {
  throw SPXGraphException(cn + mn + "TMatrix m1 is invalid");
 }

 if(!m2) {
  throw SPXGraphException(cn + mn + "TMatrix m2 is invalid");
 }

 if(m1->GetNrows() != m2->GetNrows() ) {
  std::ostringstream oss;
  oss<<cn<<mn<<"TMatrix have different number of rows m1= "<<m1->GetNrows() <<" m2= "<< m2->GetNrows() <<" "<< std::endl;     
  throw SPXGeneralException(oss.str());
 }

 if(m1->GetNcols() != m2->GetNcols() ) {
  std::ostringstream oss;
  oss<<cn<<mn<<"TMatrix have different number of columns m1= "<<m1->GetNcols() <<" m2= "<< m2->GetNcols() <<" "<< std::endl;     
  throw SPXGeneralException(oss.str());
 }

 double dev=0.;

 for( Int_t i=0; i<m1->GetNcols(); i++ ){
  for( Int_t j=0; j<m1->GetNrows(); j++ ){
   double val1= (*m1)[i][j];
   double val2= (*m2)[i][j];

   if (val1*val2<0) {
    std::cout<<cn<<mn<<"WARNING: elements have different signs i= "<< i <<" j= "<< j << " val1= "<<val1<<" val2= "<<val2<<std::endl;
   }

   double diff=0.;
   if (val1!=0.) {
    diff=fabs(val1-val2)/val1;

    if (diff>=reltolerance) {
     dev+=diff;

     std::cout<<cn<<mn<<"WARNING: element not the same within "<<reltolerance<<" i= "<<i<<" j= "<<j<< std::scientific << " val1= "<<val1<<" val2= "<<val2<<
             " rel= "<<val1/val2<<" diff= "<<diff<<" dev= "<<dev<<std::endl;

    } else {
     if (diff>=0.02) {
      std::cout<<cn<<mn<<"WARNING: element not the same within 2% "<<" i= "<<i<<" j= "<<j<< std::scientific << " val1= "<<val1<<" val2= "<<val2<<
             " rel= "<<val1/val2<<" diff= "<<diff<<" dev= "<<dev<<std::endl;
     }
    }
   } else {
    if (val2!=0) {
     std::cout<<cn<<mn<<"WARNING: val1=0, but not val2= "<<val2<<std::endl;
     std::cerr<<cn<<mn<<"WARNING: val1=0, but not val2= "<<val2<<std::endl;
     return HUGE_VAL;
    }
   }

   if (debug)
    std::cout<<cn<<mn<<" i= "<<i<<" j= "<<j<< std::scientific << " val1= "<<val1<<" val2= "<<val2<<
             " rel= "<<val1/val2<<" diff= "<<diff<<" dev= "<<dev<<std::endl;
  }
 }
 
 return dev;
};

>>>>>>> release-0.98

 return true;
} 
