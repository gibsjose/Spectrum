//************************************************************/
//
//	Chi2 calculations
//
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		20.1.2015
//	@Email:		tancredi.carli@cern.ch
//
//************************************************************/

#include "SPXChi2.h"

const std::string cn = "SPXChi2::";


SPXChi2::SPXChi2(std::vector<SPXData*> data, std::vector<SPXCrossSection> crossSections, SPXSteeringFile *steeringFile) {
 std::string mn = "SPXChi2: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn); 
 
 if (!steeringFile)  std::cout<<cn<<mn<<"Steering file not found "<<std::endl;

 if (data.size()==0) std::cout<<cn<<mn<<"No data object in vector"<<std::endl;
 else	             std::cout<<cn<<mn<<"Number of data objects= "<<data.size()<<std::endl;

 if (crossSections.size()==0) std::cout<<cn<<mn<<"No cross section object in vector"<<std::endl;
 else                         std::cout<<cn<<mn<<"Number of CrossSection objects: "<<crossSections.size()<<std::endl;

 if (steeringFile->GetCalculateChi2()==1){         
  for(int icross = 0; icross < crossSections.size(); icross++) {
   SPXPDF * pdf=crossSections[icross].GetPDF();
   if (!pdf) {std::cout<<cn<<mn<<"PDF object not found "<<std::endl; return;}

   if (debug) std::cout<<cn<<mn<<"Calculate simple Chi2"<<std::endl;
   if (icross>=data.size()) {
    std::cout<<cn<<mn<<"icross= "<<icross<<" but data.size()= "<<data.size()<<std::endl;
    return;
   }
   SPXData *mydata=data.at(icross);
   if (!mydata) {
    std::cout<<cn<<mn<<"Data object not found !"<<std::endl;
   }

   this->CalculateSimpleChi2(pdf, mydata);       

  }
 }

}
double SPXChi2::CalculateSimpleChi2(SPXPDF *pdf, SPXData *data) {
 std::string mn = "CalculateSimpleChi2: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (!pdf) {
  std::ostringstream oss;
  oss << cn <<mn<<"PDF object found !";
  throw SPXGeneralException(oss.str());
 }

 if (!data) {
  std::ostringstream oss;
  oss << cn <<mn<<"Data object found !";
  throw SPXGeneralException(oss.str());
 }

 const int nbin=data->GetNumberOfBins();

 // vector << TMatrixT<double> * >> vtheorycovmatrix;

 /// Fill in the theory covariance matrix
 //int nbands=pdf->GetNBands();
 //if (debug) std::cout << cn << mn <<"Number of bands= " <<nbands<< std::endl;

 /*
 for (int iband=0; iband<nbands; iband++) {
  TGraphAsymmErrors * gband   =pdf->GetBand(iband);
  if (!gband) {
   std::ostringstream oss;
   oss << cn <<mn<<"get bands "<<"Band "<<iband<<" not found at index "<<iband;
   throw SPXGeneralException(oss.str());
  }
 */

  TGraphAsymmErrors *gband=pdf->GetTotalBand();
  if (!gband) {
   throw SPXGeneralException(cn+mn+"Graph not found from PDF class");
  }

  if (nbin!=gband->GetN()) {
   std::ostringstream oss;
   oss << cn <<mn<<"Number of bins in data= "<<nbin<<" not equal number of bins in theory= "<<gband->GetN();
   throw SPXGeneralException(oss.str());
  }

  // Below needs to be replaced
  //
  TMatrixT<double> *theory_cov_matrix = new TMatrixT<double>(nbin, nbin);
  //
  // This should be replaced by a loop over the individual components
  // and could be moved to the cross section class
  //
  // for the moment we just have a diagonal matrix
  //
  for (int pi1 = 0; pi1 < nbin; pi1++) {
   for (int pi2 = 0; pi2 < nbin; pi2++) {
    if ( pi1 != pi2 ) 
     (*theory_cov_matrix) (pi1, pi2) = 0;
    if ( pi1 == pi2 ) {
     // is this reasonable ? might introduce special treatment later
     double theory_uncertainty = 0.5*(gband->GetErrorYhigh(pi1) + gband->GetErrorYlow(pi1));
     (*theory_cov_matrix)(pi1, pi2) = theory_uncertainty*theory_uncertainty;
    }
   }
  }
 
  if (debug) {
   std::cout<<cn<<mn<<"Print theory uncertainty matrix: "<<std::endl;
   theory_cov_matrix->Print();
  }
 //
 // Add theory matrices together
 // TMatrixT<double> *theory_tot_cov_matrix = 0;
 //for (int i=0; i<vtheorycovmatrix.size(): i++) {
 // *theory_tot_cov_matrix += *theory_cov_matrix[i];
 // 
 // if (debug) { 
 //  std::cout<<cn<<mn<<" i= "<<i<<" Print total theory matrix: "<<std::endl;
 //  theory_tot_cov_matrix->Print();
 // }
 //}

 //if (debug) { 
 // std::cout<<cn<<mn<<"Print total theory matrix: "<<std::endl;
 // theory_tot_cov_matrix->Print();
 //}

 TMatrixT<double> *data_cov_matrix=data->GetDataTotalCovarianceMatrix();
 if (!data_cov_matrix) {
  std::ostringstream oss;
  oss << cn <<mn<<"Data covariance matrix not found !";
  throw SPXGeneralException(oss.str());
 }

 if (debug) {
  std::cout<<cn<<mn<<"Data covariance matrix: "<<std::endl;
  data_cov_matrix->Print();
 } 

 //TMatrixT<double> *tot_cov_matrix = (*theory_cov_matrix) + (*data_cov_matrix);
 //
 //TMatrixT<double> tot_cov_matrix->Plus(*theory_cov_matrix,*data_cov_matrix);
 TMatrixT<double> *tot_cov_matrix = new TMatrixT<double>(*theory_cov_matrix,TMatrixD::kPlus,*data_cov_matrix);

 if (debug) {
  std::cout<<cn<<mn<<"Total covariance matrix: "<<std::endl;
  tot_cov_matrix->Print();
 } 

 TMatrixT<double> inverted_cov_matrix = tot_cov_matrix->Invert(); //includes theory errors in diagonal elements

 if (debug) {
  std::cout<<cn<<mn<<"Inverted total covariance matrix: "<<std::endl;
  inverted_cov_matrix.Print();
 }


 // Loop over bins and determine data-theory matrices
 TMatrixT<double> row_data_minus_theory(1, nbin);
 TMatrixT<double> col_data_minus_theory(nbin, 1);

 TGraphAsymmErrors * gdata=data->GetTotalErrorGraph();
 if (!gdata) {
  std::ostringstream oss;
  oss << cn <<mn<<"Data x not found !";
  throw SPXGeneralException(oss.str());
 }

 for (int pi1 = 0; pi1 < nbin; pi1++) {
  double data_val=0., theory_val=0., x_val=0.;

  gband->GetPoint(pi1, x_val, theory_val);
  gdata->GetPoint(pi1, x_val, data_val);

  row_data_minus_theory(0  ,pi1) = data_val - theory_val;
  col_data_minus_theory(pi1,0)   = data_val - theory_val;

  //if (debug) std::cout<<cn<<mn<<"CalcChi2: At "<<x_val<<" data = "<<data_val
  //              <<" theory = "<<theory_val<<", content = "<<row_data_minus_theory(0,pi)<<std::endl;
  }  

  TMatrixT<double> cov_times_col = inverted_cov_matrix*col_data_minus_theory;
  if (debug) {
   std::cout<<cn<<mn<<"After first multiplication matrix is: \n"<<std::endl;
   cov_times_col.Print();
   /*
      for (int pi = 0; pi < g_theory->GetN(); pi++) {
      cout<<cov_times_col(pi, 0)<<"\t";
      }
      cout<<"\n";
    */
  }
 
 TMatrixT<double> result = row_data_minus_theory*cov_times_col;
 if (debug) {
  std::cout<<cn<<mn<<"Result matrix"<<std::endl;
  result.Print();
 }

 double chi2 = result(0,0);
 if (debug) std::cout<<cn<<mn<<" chi2 = "<<chi2<<std::endl;

 return chi2;

}
