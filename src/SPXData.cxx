//************************************************************/
//
//	Data Implementation
//
//	Implements the SPXData class, which stores the data from a
//	given data file in a map structure and intelligently parses
//	data from different formats.
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		08.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include <iomanip>
#include <string.h> //malloc

#include "SPXData.h"

//Class name for debug statements
const std::string cn = "SPXData::";

//Must define the static debug variable in the implementation
bool SPXData::debug;

SPXData::SPXData(const SPXPlotConfigurationInstance &pci) {
 std::string mn = "SPXData: ";

 this->pci = pci;
 this->dataFormat  = this->pci.dataSteeringFile.GetDataFormat();
 dividedByBinWidth = this->pci.dataSteeringFile.IsDividedByBinWidth();

 RemoveXbins=this->pci.dataSteeringFile.GetDataRemoveXbinsFlag();
 if (debug) std::cout<<cn<<mn<<"RemoveXbins "<< ( RemoveXbins ? "ON" : "OFF")<< std::endl;

 datafilename=this->pci.dataSteeringFile.GetFilename();


 if (RemoveXbins) {
  DataCutXmin=this->pci.dataSteeringFile.GetDataCutXmin();
  DataCutXmax=this->pci.dataSteeringFile.GetDataCutXmax();

  if (debug) std::cout<<cn<<mn<<"Remove bins with values < "<<DataCutXmin<<" and > "<<DataCutXmax<<std::endl;
 } else {
  if (debug) std::cout<<cn<<mn<<"All data are kept ...unless main steering over-rules "<<std::endl;
  DataCutXmin=-HUGE_VAL;
  DataCutXmax= HUGE_VAL;
 }

 TakeSignforTotalError=false;
 AddMCStattoTotalStatError=""; 
 
 //set pointer to zero
 statisticalErrorGraph=0;
 systematicErrorGraph=0;
 totalErrorGraph=0;
 
 cov_matrixtot=0;
 cov_matrixstat=0;
 cov_matrixsyst=0;

 corr_matrixtot=0;
 corr_matrixstat=0;
 corr_matrixsyst=0;

 individualsystematicErrorGraph.clear();

 //
 // The below function must be call outside the SPXData class
 // to allow for setting from the main steering
 // Parse in data
 // 
 //if (debug) std::cout<<cn<<mn<<"Parse in Data"<<std::endl;
 //this->Parse();
 //
 //if (debug) std::cout<<cn<<mn<<"Create graphs"<<std::endl;
 // this->CreateGraphs(); // is now called outside

 if (debug) std::cout<<cn<<mn<<"Finished SPXData constructor"<<std::endl;

 return;
}


//Public helper method to parse the data file based on the type
void SPXData::Parse(void) {
	std::string mn = "Parse: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Parsing data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;
	if(debug) std::cout << cn << mn << "Parsing filename: " << pci.dataSteeringFile.GetFilename() << std::endl;

	try {
		OpenDataFile();
	} catch(const SPXException &e) {
		std::cerr << e.what() << std::endl;

		throw SPXFileIOException(cn + mn + "Unable to open data file");
	}

	if(dataFormat.IsSpectrum()) {
		if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		try {
			ParseSpectrum();
		} catch(const SPXException &e) {
			std::cerr << e.what() << std::endl;
			throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		}
		//} else if(dataFormat.IsHERAFitter()) {
		//if(debug) std::cout << cn << mn << "Data format is " << dataFormat.ToString() << std::endl;

		//try {
		//	ParseHERAFitter();
		//} catch(const SPXException &e) {
		//	std::cerr << e.what() << std::endl;
		//	throw SPXParseException(pci.dataSteeringFile.GetDataFile(), "Error parsing data file");
		//}
	} else {
	 throw SPXParseException("DataSteeringFile " + pci.dataSteeringFile.GetFilename() + " has invalid data format");
	}

	return;
}

void SPXData::ParseSpectrum(void) {
	std::string mn = "ParseSpectrum: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

	if(!(*dataFile)) {
		throw SPXFileIOException("Something went awry with the dataFile ifstream...");
	}

	//Fixed column indices
	const unsigned int XM_COL = 		0;
	const unsigned int XLOW_COL = 		1;
	const unsigned int XHIGH_COL = 		2;
	const unsigned int SIGMA_COL = 		3;
	const unsigned int STAT_COL = 		4;
	const unsigned int SYST_COL = 		5;	//For symmetric total systematics
	const unsigned int SYST_P_COL =		5;	//For asymmetric total systematics
	const unsigned int SYST_N_COL = 	6;	//...

	const unsigned int REQ_COLS =		5;	//Required columns (xm - stat)

	std::string line;
	double xm_t, xlow_t, xhigh_t, sigma_t, stat_t, syst_p_t, syst_n_t;
	std::vector<double> xm;				//Mean x
	std::vector<double> xlow;			//X min
	std::vector<double> xhigh;			//X max
	std::vector<double> sigma;			//Sigma (cross-section)
	std::vector<double> stat;			//Statistical error
	std::vector<double> syst_p;			//Total systematic error (+)
	std::vector<double> syst_n;			//Total systematic error (-)

	//Individual systematic error namess
	std::vector<std::string> names;

	//Count the number of positive/negative systematic errors to check balance
	unsigned int pos_count = 0;
	unsigned int neg_count = 0;

	//Bin count
	unsigned int bin_count = 0;


	keepbin.clear();

	//Number of columns (used to determine symmetric, asymmetric, or no total systematic error)
	unsigned int numberOfColumns = 0;

        double faclumi=pci.dataSteeringFile.GetLumiScaleFactor();

        if (faclumi!=1) {
	 if (debug) std::cout<<cn<<mn<<"Rescale data by factor= "<<faclumi<<std::endl;
	} 

	//if (debug) std::cout<<cn<<mn<<"Start looping over file "<<std::endl;

	while(dataFile->good()) {
		std::getline(*dataFile, line);

		//Skip empty lines
		if(line.empty()) {
			continue;
		}

		std::istringstream iss(line);

                // trim away whitespace
                line=SPXStringUtilities::LeftTrim(line);

		//Skip comments
		if(!line.empty() && (line[0] == ';')) {
			continue;
		} else if(!line.empty()) {

		  if (debug) std::cout<<cn<<mn<<"find systematics "<<std::endl;
		  //if (debug) std::cout<<cn<<mn<<"line=XX"<<line<<"XXX"<<std::endl;

		  //std::string test=SPXStringUtilities::LeftTrim(line);
		  //if (debug) std::cout<<cn<<mn<<"test=XX"<<test<<"XXX"<<std::endl;

		  //throw SPXParseException(cn + mn + "There must be at least 5 data columns (xm, xlow, xhigh, sigma, stat)");

			//Check for systematic errors (line contains 'syst_')
			if(line.find("syst_") != std::string::npos) {

			        //if (debug) std::cout<<cn<<mn<<"syst_ "<<std::endl;
				//Split the line into systematic name and data
				std::string name;
				std::vector<double> tmp_syst;

				//Parse
				iss >> name;
				std::string formatted_line = SPXStringUtilities::RemoveFirstSubstring(line, name);
				formatted_line = SPXStringUtilities::ReplaceAll(formatted_line, "\t", " ");
				tmp_syst = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

 	                        //if (debug) {
                                // std::cout<<cn<<mn<<"after tmp_syst"<<std::endl;
                                // for (int i=0; i<tmp_syst.size(); i++)
				//   std::cout<<cn<<mn<<" tmp_syst["<<i<<"]= "<<tmp_syst.at(i)<<std::endl;
				//}

				//Symmetric Error: Create both + and - and add them to map
				if((name.find("+") == std::string::npos) && (name.find("-") == std::string::npos)) {
					//Increment both positive and negative count
					pos_count++;
					neg_count++;

					std::string p_name = name + "+";
					std::string n_name = name + "-";

					if(debug) std::cout << std::endl;
					if(debug) std::cout << cn << mn << "Found new symmetric systematic error: " << name << std::endl;
					if(debug) std::cout << cn << mn << "Converted to asymmetric errors: " << p_name << " and " << n_name << std::endl;
					if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

                                        if (faclumi!=1) {
					 //if (debug) std::cout<<cn<<mn<<"Rescale data by factor "<<faclumi<<std::endl;
                                         std::transform(tmp_syst.begin(), tmp_syst.end(), tmp_syst.begin(), 
                                         std::bind1st(std::multiplies<double>(), faclumi));
                                        }

					//Add to map
					StringDoubleVectorPair_T p_pair(p_name, tmp_syst);
                                        //negative error should have negative sign
                                        std::transform(tmp_syst.begin(), tmp_syst.end(), tmp_syst.begin(), 
					std::bind1st(std::multiplies<double>(), -1.));

                                        //if (debug) 
                                         std::cout<<cn<<mn<<"negative systematics multiplied by -1"<<std::endl;
					StringDoubleVectorPair_T n_pair(n_name, tmp_syst);


                                        if (individualSystematics.count(p_name)>0) {
					 std::cout<<cn<<mn<<"WARNING: positive systematics "<<p_name.c_str()<<" already in map "<<std::endl;
					 std::cerr<<cn<<mn<<"WARNING: positive systematics "<<p_name.c_str()<<" already in map "<<std::endl;
					}

                                        if ( individualSystematics.count(n_name)>0) {
					 std::cout<<cn<<mn<<"WARNING: negative systematics "<<n_name.c_str()<<" already in map "<<std::endl;
					 std::cerr<<cn<<mn<<"WARNING: negative systematics "<<n_name.c_str()<<" already in map "<<std::endl;
					}

					individualSystematics.insert(p_pair);
					individualSystematics.insert(n_pair);

					if (debug) std::cout<<cn<<mn<<p_name<<" in individualSystematics.count= "<<individualSystematics.count(p_name)<<std::endl;
				}

				//Asymmetric Error: If '-', make sure there was already a '+', else issue warning
				else {
					if(name.find("+") != std::string::npos) {
						pos_count++;

						if(debug) std::cout << std::endl;
						if(debug) std::cout << cn << mn << "Found new individual systematic error: " << name << std::endl;
						if(debug) std::cout << cn << mn << "Line: " << line << std::endl;
					}
					else if(name.find("-") != std::string::npos) {
						neg_count++;

						if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

						if(individualSystematics.count(SPXStringUtilities::ReplaceAll(name, "-", "+")) == 0) {
							std::cerr << cn << mn << "WARNING: Unbalanced sytematic error: " << SPXStringUtilities::RemoveCharacters(name, "+-") << std::endl;
						}
					}

                                        if (faclumi!=1) {
					 //if (debug) std::cout<<cn<<mn<<"Rescale data by factor "<<faclumi<<std::endl;
                                         std::transform(tmp_syst.begin(), tmp_syst.end(), tmp_syst.begin(), 
                                         std::bind1st(std::multiplies<double>(), faclumi));
                                        }

					//Add it to the map
					StringDoubleVectorPair_T pair(name, tmp_syst);

					//std::cout<<cn<<mn<<"1 count= "<<individualSystematics.count(name)<<std::endl;
                                        if ( individualSystematics.count(name)>0) {
					 std::cout<<cn<<mn<<"WARNING: systematics "<<name.c_str()<<" already in map "<<std::endl;
					 std::cerr<<cn<<mn<<"WARNING: systematics "<<name.c_str()<<" already in map "<<std::endl;
					}

					individualSystematics.insert(pair);

					//std::cout<<cn<<mn<<"1 count= "<<individualSystematics.count(name)<<std::endl;
				}
			}

			//Not a systematic error: Read as data if it starts with a number (if first non-whitespace character is a digit)
                        //if (debug) std::cout<<cn<<mn<<"Not a systematic error: Read as data if it starts with a number (if first non-whitespace character is a digit)"<<std::endl;

			else if(isdigit((int)SPXStringUtilities::LeftTrim(line).at(0))) {

			       if(debug) std::cout << cn << mn << "Read in data table Line: " << line << std::endl;

				//Parse line into data vector
				//Convert all tabs to spaces
				std::string formatted_line = SPXStringUtilities::ReplaceAll(line, "\t", " ");
				std::vector<double> tmp_data = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

				//Make sure there are at least 5 columns
				if(tmp_data.size() < REQ_COLS) {
					throw SPXParseException(cn + mn + "There must be at least 5 data columns (xm, xlow, xhigh, sigma, stat)");
				}

				//Set number of columns to the size of the 0th bin
				if(!bin_count) {
					//Set number of columns
					numberOfColumns = tmp_data.size();

					//Warn user that the aforementioned columns will be assumed for the remaining bins
					std::cout << cn << mn << "INFO: The remaining bins MUST also have exactly " << numberOfColumns << " columns" << std::endl;
				}
				//After the 0th bin, make sure all other bins have the exact same number of columns
				else {
					if(tmp_data.size() != numberOfColumns) {
						std::ostringstream oss;
						oss << cn << mn << "Number of columns for bin " << bin_count + 1<< " (" << tmp_data.size() << \
							") does NOT match expected (" << numberOfColumns << ")" << std::endl;
						throw SPXParseException(oss.str());
					}
				}

				//Parse out required data
				xm_t    = tmp_data[XM_COL];
				xlow_t  = tmp_data[XLOW_COL];
				xhigh_t = tmp_data[XHIGH_COL];
				sigma_t = tmp_data[SIGMA_COL];
				stat_t  = tmp_data[STAT_COL];

				//Check for symmetric, asymmetric, or no total systematic error (set numberOfColumns)
				if(numberOfColumns == 5) {
					if(!bin_count && debug) std::cout << cn << mn << "Assuming No Total Systematic Errors: Will sum individuals to calculate total" << std::endl;
				} else if(numberOfColumns == 6) {
					if(!bin_count && debug) std::cout << cn << mn << "Assuming Symmetric Systematic Errors: Will sum individuals to compare against total, if they are given" << std::endl;
					syst_p_t = tmp_data[SYST_COL];
					syst_n_t = tmp_data[SYST_COL];
					syst_p.push_back(syst_p_t);
					syst_n.push_back(syst_n_t);
				} else if(numberOfColumns == 7) {
					if(!bin_count && debug) std::cout << cn << mn << "Assuming Asymmetric Systematic Errors: Will sum individuals to compare against total, if they are given" << std::endl;
					syst_p_t = tmp_data[SYST_P_COL];
					syst_n_t = tmp_data[SYST_N_COL];
					syst_p.push_back(syst_p_t);
					syst_n.push_back(syst_n_t);
				}

                                if (faclumi!=1) {
				 //if (debug) std::cout<<cn<<mn<<" rescale data by factor "<<faclumi<<std::endl;
                                 sigma_t*=faclumi;
                                 stat_t *=faclumi;
                                }

				//Increment bin count
				bin_count++;

				// Fill required vectors with temp variables
                                // remove bins if required in steering
                                if (RemoveXbins) {
				 if ( xlow_t < DataCutXmin) {
				  if (debug) std::cout<<cn<<mn<<"------> Remove bin "<<bin_count<<" with xlow_t= " <<xlow_t <<" DataCutXmin= "<<DataCutXmin<<std::endl;
                                  continue;
                                 }

				 if ( xhigh_t > DataCutXmax) {
				  if (debug) std::cout<<cn<<mn<<"------> Remove bin "<<bin_count<<" with xhigh_t= "<<xhigh_t<<" DataCutXmax= "<<DataCutXmax<<std::endl;
                                  continue;
                                 } 

				 if (debug) {
				  std::cout<<cn<<mn<<"------> Keep bin "<<bin_count<<" with xlow_t= "<<xlow_t<<" and xhigh_t= "<<xhigh_t<<std::endl;
                                 }

				 xm.push_back(xm_t);
				 xlow.push_back(xlow_t);
				 xhigh.push_back(xhigh_t);
				 sigma.push_back(sigma_t);
				 stat.push_back(stat_t);
   
                                 // bin_count starts at 1
                                 keepbin[bin_count-1]=xm.size()-1;

                                } else {
				 xm.push_back(xm_t);
				 xlow.push_back(xlow_t);
				 xhigh.push_back(xhigh_t);
				 sigma.push_back(sigma_t);
				 stat.push_back(stat_t);
				}
			}
		}
	}

	//if (debug) std::cout<<cn<<mn<<"After loop "<<std::endl;

	//Check for no bin count...
	if(!bin_count) {
		throw SPXParseException("No data bins found in data file: Make sure it is not empty");
	}

	//Check positive/negative individual systematic count
	if(pos_count != neg_count) {
	  std::cerr << cn << mn << "WARNING: Different number of positive/negative systematic errors: Data errors could be skewed pos_count= " << pos_count << " neg_count= "<<neg_count << std::endl;
	}

        if (debug&&RemoveXbins) {
	 std::cout<<cn<<mn<<"After looping over data: The following bins will be kept: " << std::endl;
	 std::cout<<cn<<mn<<"INFO: keepbin[ keptbinindex ]= newbinindex " <<std::endl;
	 for(std::map<int,int>::iterator it = keepbin.begin(); it != keepbin.end(); it++)
	   std::cout<<cn<<mn <<"keepbin["<<it->first<<"]= " <<  it->second<<std::endl;

	 std::cout<<cn<<mn <<" " <<std::endl;

	 //std::cout<<cn<<mn <<"Number of bins= " <<  bin_count<<std::endl;
         //for (int i=0; i<bin_count; i++)
	 // std::cout<<cn<<mn << " keepbin.count("<<i<<")= " << keepbin.count(i)<<std::endl;
        }
   
	//Set master size to number of bins
	int masterSize = bin_count;
	if(debug) std::cout << cn << mn << "Master size set to size of \"bin_count\": " << masterSize << std::endl;

	//If no total sytematics were given but there are no individual errors given either, bail out
	if((numberOfColumns == 5) && !pos_count) {
		throw SPXParseException("No total systematic error provided AND no individuals to sum");
	}

	if (pci.dataSteeringFile.AddLuminosityUncertainyToSystematics()) {
	 if(debug) std::cout<<cn<<mn<<"Add luminosity as systematic components"<<std::endl;
         double elumi=pci.dataSteeringFile.GetDatasetLumiUncertainty();
	 //
         // in case lumi needs to be symmetrized
         //elumi/=2.;
	 //
	 std::vector<double> tmp_systp(masterSize, elumi);
	 std::vector<double> tmp_systn(masterSize,-elumi);
	 std::string name="syst_lumi";
         StringDoubleVectorPair_T p_pair(name+"+", tmp_systp);
	 individualSystematics.insert(p_pair);
         StringDoubleVectorPair_T n_pair(name+"-", tmp_systn);
	 individualSystematics.insert(n_pair);
        }

	if(debug) std::cout<<" "<<std::endl;

	std::cout<<cn<<mn<<"HUHU AddMCStattoTotalStatError= "<<AddMCStattoTotalStatError<<std::endl;
        if (AddMCStattoTotalStatError!="") {
	 std::string name=AddMCStattoTotalStatError;
         if (!TString(AddMCStattoTotalStatError).Contains("syst_"))
	  name="syst_"+AddMCStattoTotalStatError;

  	 if (debug) std::cout<<cn<<mn<<"INFO Remove name: "<<name<<std::endl;

         for (int i=0; i<2; i++) {
	  std::string iname;
	  if (i==0) iname=name+"+";
          else      iname=name+"-";

  	  if (debug) std::cout<<cn<<mn<<"INFO Remove from map of systematics: "<<iname<<std::endl;

          if (individualSystematics.count(iname)>0) {
	   if (debug) std::cout<<cn<<mn<<"Found in map, now Erase from map "<<iname<<" in map individualSystematics "<<std::endl;

	   std::vector <double> vtmp=individualSystematics[iname];

           individualSystematics.erase(iname);

           if (i==1) {
	    if (debug) std::cout<<cn<<mn<<"Only add up negative "<<AddMCStattoTotalStatError<<" to data stat uncertainty "<<std::endl;
            continue;
           }

           if (stat.size()!=vtmp.size()){
	    std::cout<<cn<<mn<<"Data stat and MC stat vectors do not have the same size "<<std::endl;
	    std::cout<<cn<<mn<<"Data stat size= "<<stat.size()<<std::endl;
	    std::cout<<cn<<mn<<"MC stat size= "<<vtmp.size()<<std::endl;
           }

           for (int i=0; i<vtmp.size(); i++){ 
	    std::cout<<cn<<mn<<"i= "<<i<<" MCstat= "<<vtmp.at(i)<<std::endl;
           } 

           for (int i=0; i<stat.size(); i++){ 
	    std::cout<<cn<<mn<<"i= "<<i<<" Datastat= "<<stat.at(i)<<std::endl;
           } 

 	   std::vector <double> quadstat;
           for (int i=0; i<vtmp.size(); i++) {
	    double quad=vtmp.at(i)*vtmp.at(i)+stat.at(i)*stat.at(i);
            if (quad>0.) quad=sqrt(quad);
	    quadstat.push_back(quad); 
	    std::cout<<cn<<mn<<"i= "<<i<<" quad= "<<quad<<std::endl;
           }
           //
           if (debug) std::cout<<cn<<mn<<"Replace data statistical by MC and data statistics added in quadrature"<<std::endl;
           //stat=quadstat; 
           std::copy (stat.begin(), stat.end(), quadstat.begin());
            
           for (int i=0; i<stat.size(); i++){ 
	    std::cout<<cn<<mn<<"i= "<<i<<" total stat= "<<stat.at(i)<<std::endl;
           } 
           //
          } else {
	   std::cout<<cn<<mn<<"WARNING: Not found in map "<<iname<<std::endl;
          }

         }
        }

	//If necessary, compute/compare total positive/negative systematics for each bin using the individual systematic errors
	if(pos_count) {

		//The acceptable percent difference between given total and summed individuals (0.10 = 10%)
		const double INDIV_VS_TOT_ACCEPTABLE_ERROR = 0.10;

		for(int i = 0; i < masterSize; i++) {
			std::vector<double> p_errors;
			std::vector<double> n_errors;
			p_errors.clear();
			n_errors.clear();

                        if (RemoveXbins) {
                         if (keepbin.count(i)==0) {
			  if (debug) std::cout<<cn<<mn<<"-----> Bin i= "<<i<<" is removed ! "<<std::endl;
                          continue;
                         } else {
			  if (debug) std::cout<<cn<<mn<<"-----> Bin i= "<<i<<" is kept "<<std::endl;
			 }
                        }

			for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); ++it) {

				const std::string   &name = it->first;
				std::vector<double> &syst = it->second;

				std::cout<<cn<<mn<<name<<" i= "<<i<<" syst.size()= "<<syst.size()<<std::endl;

			        if (i>=syst.size()) { 
				 std::ostringstream oss;
				 oss<<cn<<mn<<"Systematic uncertainty vector too small syst.size()= "<<syst.size()<<" i= "<<i<<" Check number of bins in systematic text file name= "<<name.c_str();
				 throw SPXParseException(oss.str());
                                }


				//Positive systematic
				if(name.find("+") != std::string::npos) {
				 if (TakeSignforTotalError) {
				  if (syst.at(i)>0) 
				   p_errors.push_back(syst.at(i));
                                  else
				   n_errors.push_back(syst.at(i));
                                 } else
				  p_errors.push_back(syst.at(i));
				}

				//Negative systematic
				else if(name.find("-") != std::string::npos) {

				 if (TakeSignforTotalError) {
				  if (syst.at(i)>0) 
				   p_errors.push_back(syst.at(i));
                                  else
				   n_errors.push_back(syst.at(i));
                                 } else
				  n_errors.push_back(syst.at(i));
				}

				//Symmetric systematic: +/- are the same
				else {
					p_errors.push_back(syst.at(i));
					n_errors.push_back(syst.at(i));
				}

			}

			syst_p_t = SPXMathUtilities::AddErrorsInQuadrature(p_errors);
			syst_n_t = SPXMathUtilities::AddErrorsInQuadrature(n_errors);

			if(debug) std::cout << std::endl;
			if(debug) std::cout << cn << mn << "Total positive systematic error for bin " << i << " calculated as: " << syst_p_t << std::endl;
			if(debug) std::cout << cn << mn << "Total negative systematic error for bin " << i << " calculated as: " << syst_n_t << std::endl;

			//No totals given, just use sum of individuals
			if(numberOfColumns == 5) {
				syst_p.push_back(syst_p_t);
				syst_n.push_back(syst_n_t);
			}

			//Otherwise, compare the summed individuals against the total within the acceptable percent difference
			else if((numberOfColumns == 6) || (numberOfColumns == 7)) {
			  if(debug) std::cout << cn << mn << " i= " << i << " size: syst_p= "<<syst_p.size()<<" syst_n "<<syst_n.size()<< std::endl;
  			        if (i>=syst_n.size()) {
				 std::ostringstream oss;
				 oss << cn << mn << "Systematic uncertainty vector too small size neg= "<<syst_n.size()<<" pos= "<<syst_p.size()<<" i= "<<i;
				 throw SPXParseException(oss.str());
                                }
				double given_total_p = syst_p.at(i);
				double given_total_n = syst_n.at(i);
				double delta_p = fabs(given_total_p - syst_p_t);
				double delta_n = fabs(given_total_n - syst_n_t);
				double average_p = (given_total_p + syst_p_t) / 2;
				double average_n = (given_total_n + syst_n_t) / 2;

				double percentDifference_p = delta_p / average_p;
				double percentDifference_n = delta_n / average_n;

				if(percentDifference_p > INDIV_VS_TOT_ACCEPTABLE_ERROR) {
					std::cerr << cn << mn << "WARNING: Bin " << i << ": Sum of POSITIVE individual errors (" << syst_p_t << \
					 	") does not agree within " << (INDIV_VS_TOT_ACCEPTABLE_ERROR * 100) << \
							"% with the given POSITIVE tot syst error (" << given_total_p << ")"
						  <<" percent difference is= "<<percentDifference_p*100. << std::endl;
				}

				if(percentDifference_n > INDIV_VS_TOT_ACCEPTABLE_ERROR) {
					std::cerr << cn << mn << "WARNING: Bin " << i << ": Sum of NEGATIVE individual errors (" << syst_n_t << \
						") does not agree within " << (INDIV_VS_TOT_ACCEPTABLE_ERROR * 100) << \
							"% with the given NEGATIVE tot syst error (" << given_total_n << ")"
						  <<" percent difference is= "<<percentDifference_n*100. << std::endl;
				}
			}
		}
	}

        /*
        if (debug&&RemoveXbins) {
	 std::cout<<cn<<mn << "The following bins will be kept: " << std::endl;
	 std::cout<<cn<<mn << "INFO: keepbin[ keptbinindex ]= newbinindex " <<std::endl;
	 for(std::map<int,int>::iterator it = keepbin.begin(); it != keepbin.end(); it++)
	  std::cout<<cn<<mn << " keepbin["<<it->first<<"]= " <<  it->second <<std::endl;

         //for (int i=0; i<bin_count; i++)
	 // std::cout<<cn<<mn << " keepbin.count("<<i<<")= " << keepbin.count(i)<<std::endl;
        }
        */

        if (RemoveXbins) {
         int newmaster=keepbin.size();

	 if(numberOfColumns >5) {
          // remove bins from syst vector
  	  std::vector<double> vsyst;
          for (int i=0; i<syst_p.size(); i++) {
	   if (keepbin.count(i)>0) {
	     vsyst.push_back(syst_p.at(i));
	   }
          }
          syst_p=vsyst;

  	  vsyst.clear();
          for (int i=0; i<syst_n.size(); i++) {
	   if (keepbin.count(i)>0) {
	     vsyst.push_back(syst_n.at(i));
	   }
          }
          syst_n=vsyst;
         
	 }

	 // remove bins from vector in the map
	 for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
	  const std::string   &syst_name  = it->first;
	  std::vector<double> &systematic = it->second;
          
	  std::vector<double> vsyst;
          //vsyst.clear();
          for (int i=0; i<systematic.size(); i++) {

	  /*
	  std::cout<<cn<<mn << " TEST "<<std::endl;
          for (int ii=0; ii<bin_count; ii++) {
	    //std::cout<<cn<<mn << " keepbin.count("<<ii<<")= " << keepbin.count(ii)<<std::endl;

	   if (debug) std::cout<<cn<<mn<<" i= "<< i<<" keepbin.count("<< i<<")= "<<keepbin.count(i)
	 		              <<" ii= "<<ii<<" keepbin.count("<<ii<<")= "<<keepbin.count(ii)<<std::endl;
	  }
          */
	   if (keepbin.count(i)>0) {
	    vsyst.push_back(systematic.at(i));
	    //if (debug) std::cout<<cn<<mn<<"Loop over systematics Bin i= "<<i<<" new size= "<<vsyst.size() <<std::endl;
           } else {
	    //if (debug) std::cout<<cn<<mn<<"Loop over systematics Bin i= "<<i<<" is removed  "<<std::endl;
           }
          } 

	  if (debug) std::cout<<cn<<mn<<"New size of "<<syst_name<<" is "<< vsyst.size()<<" masterSize= "<<masterSize<< " newmaster= "<<newmaster<<std::endl;
          it->second=vsyst;
         }

	 if (debug) std::cout<<" "<<std::endl;

	 if (masterSize!=newmaster) {
	  if (debug) std::cout<<cn<<mn<<"Some bins are removed -> update old masterSize= "<<masterSize<<" to "<<newmaster<<std::endl;
	  masterSize = newmaster;
	 } else 
	  if (debug) std::cout<<cn<<mn<<"No bins removed old masterSize "<<masterSize<<" = "<<newmaster<<std::endl;
	}

        if (masterSize==0) {
	 if (RemoveXbins) {
	  throw SPXParseException(cn+mn+"No data found or all data removed !");
	 } else
	  throw SPXParseException(cn+mn+"No data found !");
        }

	//Check vector sizes: all vectors should be the same size
	if(debug) std::cout << std::endl;
	if(debug) std::cout << cn << mn << "Checking sizes of all other vectors..." << std::endl;

	try {
		CheckVectorSize(xm,   "xm", masterSize);
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat,  "stat", masterSize);
		CheckVectorSize(syst_p,"syst_p", masterSize);
		CheckVectorSize(syst_n,"syst_n", masterSize);

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

	} catch(const SPXException &e) {
		throw;
	}

	//All vectors passed size check
	if(debug) std::cout << cn << mn << "Success: All vector sizes match master size" << std::endl;

	//Set numberOfBins based on master size
	numberOfBins = masterSize;
	if(debug) std::cout << cn << mn << "Number of Bins set to match master size: " << numberOfBins << std::endl;

	//Add all data to map
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));

	if(debug) std::cout<<cn<<mn<<"Successfully added data to map" << std::endl;

	if(debug) std::cout<<cn<<mn<<"Filling correlation Type of systematics components" << std::endl;
        std::vector<std::string> SystematicsUncorrelatedBetweenBins=pci.dataSteeringFile.GetUncertaintyCorrelationTypeVector();
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); ++it) {
	 const std::string &syst_name = it->first;
         individualSystematicsIsCorrelated[syst_name]=true;

	 for (int i=0; i<SystematicsUncorrelatedBetweenBins.size(); i++) {
          const std::string &syst_uncorr_name = SystematicsUncorrelatedBetweenBins.at(i);
	  //if (debug) std::cout<<cn<<mn<<"name= "<<syst_uncorr_name.c_str()<<std::endl;
	  std::string systp=syst_uncorr_name; std::string systn=syst_uncorr_name; 
          if (syst_uncorr_name.find("+") != std::string::npos || syst_uncorr_name.find("-") != std::string::npos) {
	   //if (debug) std::cout<<cn<<mn<<"Uncorrelated systematics specified with + and - "<<syst_uncorr_name.c_str()<<std::endl;
          } else {
	   systp+="+"; systn+="-";
          }
          //if(syst_name.compare(syst_uncorr_name) == 0) {
          if(syst_name.compare(systp) == 0 || syst_name.compare(systn) == 0) {
	   //if (debug) std::cout<<cn<<mn<<"name "<<syst_uncorr_name.c_str()<<" filling false "<<std::endl;
	   individualSystematicsIsCorrelated[syst_name]=false; // map replaces entry if already there !
          }
         }
        }

        if (debug) {
	  std::cout<<cn<<mn<<"Print correlation type for each systematic in map individualSystematicsIsCorrelated: "<<std::endl;
	 for(std::map<std::string, bool>::iterator it =  individualSystematicsIsCorrelated.begin(); it != individualSystematicsIsCorrelated.end(); ++it) {
	   std::cout<<cn<<mn<<"name= "<<it->first.c_str()<<(it->second ? " CORRELATED" : " UNCORRELATED") <<std::endl;
         }
        }

}

/*
void SPXData::ParseHERAFitter(void) {
	std::string mn = "ParseHERAFitter: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Fixed column indices (starting at Column 0)
	const unsigned int XLOW_COL = 1;
	const unsigned int XHIGH_COL = 2;
	const unsigned int SIGMA_COL = 5;
	const unsigned int STAT_P_COL = 6;
	const unsigned int STAT_N_COL = 7;
	const unsigned int SYST_BEGIN_COL = 8;

	if(debug) std::cout << cn << mn << "Beginning to parse data file: " << pci.dataSteeringFile.GetDataFile() << std::endl;

	if(!(*dataFile)) {
		throw SPXFileIOException("Something went awry with the dataFile ifstream...");
	}

	unsigned int numberOfBins = 0;
	unsigned int numberOfColumns = 0;
	unsigned int numberOfSystematics = 0;

	std::string line;
	double xm_t, xlow_t, xhigh_t, sigma_t, stat_t, syst_p_t, syst_n_t;
	std::vector<double> xm;				//Mean x
	std::vector<double> xlow;			//X min
	std::vector<double> xhigh;			//X max
	std::vector<double> sigma;			//Sigma (cross-section)
	std::vector<double> stat;			//Statistical error
	std::vector<double> syst_p;			//Total systematic error (+)
	std::vector<double> syst_n;			//Total systematic error (-)

	//Individual systematic error namess
	std::vector<std::string> names;

	while(dataFile->good()) {
		std::getline(*dataFile, line);

		//String stream to parse the individual lines
		std::istringstream iss(line);

		//Temp string
		std::string tmp;
		tmp.clear();

		if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

		//Skip empty lines
		if(line.empty()) {
			continue;
		}

		//Skip comments
		if(!line.empty() && (line[0] == ';')) {
			continue;
		} else if(!line.empty()) {

			//Get the number of bins
			if(line.find("NDATA") != std::string::npos) {
				iss >> tmp;	//NDATA
				iss >> tmp;	//=
				iss >> tmp;
				numberOfBins = atoi(tmp.c_str());
				if(debug) std::cout << cn << mn << "Number of Bins set to " << numberOfBins << std::endl;
			}

			//Get number of columns and initialize individual vector
			if(line.find("NColumn") != std::string::npos) {
				iss >> tmp;
				iss >> tmp;
				iss >> tmp;
				numberOfColumns = atoi(tmp.c_str());
				if(debug) std::cout << cn << mn << "Number of Columns set to "  << numberOfColumns << std::endl;

				//Determine number of individual systematics
				numberOfSystematics = numberOfColumns - SYST_BEGIN_COL;
				if(debug) std::cout << cn << mn << "Number of Systematics set to " << numberOfSystematics << std::endl;
			}

			//Compile the list of all individual systematic error names
			if(line.find("ColumnName") != std::string::npos) {

				tmp = line;

				//Get rid of 'ColumnName = '
				tmp = SPXStringUtilities::RemoveFirstSubstring(tmp, "ColumnName = ");

				//Remove all single quotes and spaces and delimit with "," returning a vector of tokens
				tmp = SPXStringUtilities::RemoveCharacters(tmp, "\' ");
				names = SPXStringUtilities::SplitString(tmp, ",");

				//Remove the non-systematic error names
				names.erase(names.begin(), names.begin() + SYST_BEGIN_COL);

				//DEBUG: Print all names
				if(debug) {
					std::cout << cn << mn << "names.size() = " << names.size() << std::endl;
					std::cout << cn << mn << "Individual Systematic Error Names: " << std::endl;
				}

				unsigned int pos_count = 0;
				unsigned int neg_count = 0;

				//Make sure there are the same number of +/- errors, if not issue a warning
				for(int i = 0; i < names.size(); i++) {
					if(debug) std::cout << "\t " << names.at(i) << std::endl;

					if(names.at(i).find("+") != std::string::npos) {
						pos_count++;
					}
					else if(names.at(i).find("-") != std::string::npos) {
						neg_count++;
					}
				}

				if(pos_count != neg_count) {
				 std::cerr << cn << mn << "WARNING: Different number of positive/negative systematic errors: Data errors could be skewed" 
				 <<" pos_count= "<<pos_count<< " neg_count= "<<neg_count<< std::endl;
				}

				//Check size against calculated number of columns
				if(names.size() != numberOfSystematics) {
					std::ostringstream oss;
					oss << cn << mn << "Error parsing systematic names: Name vector size (" << names.size() << \
						") is not equal to the expected number of systematics (" << numberOfSystematics << ")" << std::endl;
					throw SPXParseException(oss.str());
				}

				//Initialize a data vector for each name found
				for(int i = 0; i < numberOfSystematics; i++) {
					individualSystematics[names[i]] = std::vector<double>();
				}
			}

			//Data begins with numeric character at position 0 (and # bins and # columns have already been read correctly)
			if(isdigit((int)line.at(0)) && (numberOfBins) && (numberOfColumns)) {

				//Parse line into vector of doubles
				std::vector<double> dataVector;

				try {
					dataVector = SPXStringUtilities::ParseStringToDoubleVector(line, ' ');
				} catch(const SPXException &e) {
					std::cerr << e.what() << std::endl;
					throw SPXParseException(cn + mn + "Could not parse HERAFitter data file " + pci.dataSteeringFile.GetDataFile());
				}

				if(dataVector.size() != numberOfColumns) {
					std::ostringstream oss;
					oss << cn << mn << "Error parsing data: line's data vector size (" << dataVector.size() << \
						") is not equal to the expected number of columns (" << numberOfColumns << ")" << std::endl;
					throw SPXParseException(oss.str());
				}

				xlow_t = dataVector[XLOW_COL];
				xhigh_t = dataVector[XHIGH_COL];
				xm_t = (xlow_t + xhigh_t) / 2;
				sigma_t = dataVector[SIGMA_COL];
				double stat_p = dataVector[STAT_P_COL];
				double stat_n = dataVector[STAT_N_COL];
				stat_t = (stat_p + stat_n) / 2;

				//Read in systematics: Must use names vector instead of map keys to ensure correct order
				for(int i = SYST_BEGIN_COL; i < numberOfSystematics; i++) {
					if(debug) std::cout << "[" << i << "]" << " --> Adding error for " << names.at(i) << ": " << dataVector.at(i) << std::endl;

					if(individualSystematics.count(names.at(i)) == 0) {
						throw SPXParseException(cn + mn + "Name \"" + names.at(i) + "\" not found in systematics map");
					} else {
						std::vector<double> &v = individualSystematics[names.at(i)];
						v.push_back(dataVector.at(i));
					}
				}

				//Fill vectors with temp variables
				xm.push_back(xm_t);
				xlow.push_back(xlow_t);
				xhigh.push_back(xhigh_t);
				sigma.push_back(sigma_t);
				stat.push_back(stat_t);
			}
		}
	}

	//Set master size
	int masterSize = numberOfBins;
	if(debug) std::cout << cn << mn << "Master size set to size of \"NDATA\" " << masterSize << std::endl;

	//Compute total positive/negative systematics for each bin using the individual systematic errors
	for(int i = 0; i < masterSize; i++) {
		std::vector<double> p_errors;
		std::vector<double> n_errors;
		p_errors.clear();
		n_errors.clear();

		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); ++it) {

			const std::string &name = it->first;
			std::vector<double> &syst = it->second;

			//Positive systematic
			if(name.find("+") != std::string::npos) {
				p_errors.push_back(syst.at(i));
			}

			//Negative systematic
			else if(name.find("-") != std::string::npos) {
				n_errors.push_back(syst.at(i));
			}

			//Symmetric systematic: +/- are the same
			else {
				p_errors.push_back(syst.at(i));
				n_errors.push_back(syst.at(i));
			}
		}

		syst_p_t = SPXMathUtilities::AddErrorsInQuadrature(p_errors);
		syst_n_t = SPXMathUtilities::AddErrorsInQuadrature(n_errors);
		syst_p.push_back(syst_p_t);
		syst_n.push_back(syst_n_t);

		if(debug) std::cout << cn << mn << "Total positive systematic error for bin " << i << " calculated as: " << syst_p_t << std::endl;
		if(debug) std::cout << cn << mn << "Total negative systematic error for bin " << i << " calculated as: " << syst_n_t << std::endl;
	}

	//Check vector sizes: all vectors should be the same size
	if(debug) std::cout << cn << mn << "Checking sizes of all vectors.." << std::endl;

	try {
		CheckVectorSize(xm, "xm", masterSize);
		CheckVectorSize(xlow, "xlow", masterSize);
		CheckVectorSize(xhigh, "xhigh", masterSize);
		CheckVectorSize(sigma, "sigma", masterSize);
		CheckVectorSize(stat, "stat", masterSize);
		CheckVectorSize(syst_p, "syst_p", masterSize);
		CheckVectorSize(syst_n, "syst_n", masterSize);

		//Check size of all individual systematic errors
		for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
			const std::string &syst_name = it->first;
			std::vector<double> &systematic = it->second;
			CheckVectorSize(systematic, syst_name, masterSize);
		}

	} catch(const SPXException &e) {
		throw;
	}

	//All vectors passed size check
	if(debug) std::cout << cn << mn << "Success: All vector sizes match master size" << std::endl;

	//Set numberOfBins based on master size
	if(debug) std::cout << cn << mn << "Setting numberOfBins to match master size" << std::endl;
	this->numberOfBins = masterSize;
	if(debug) std::cout << cn << mn << "numberOfBins = " << this->numberOfBins << std::endl;

	//Add to data map
	data.insert(StringDoubleVectorPair_T("xm", xm));
	data.insert(StringDoubleVectorPair_T("xlow", xlow));
	data.insert(StringDoubleVectorPair_T("xhigh", xhigh));
	data.insert(StringDoubleVectorPair_T("sigma", sigma));
	data.insert(StringDoubleVectorPair_T("stat", stat));
	data.insert(StringDoubleVectorPair_T("syst_p", syst_p));
	data.insert(StringDoubleVectorPair_T("syst_n", syst_n));

	PrintHERAFitter();
	while(1);
}
*/
//Helper method to choose correct print method based on data format
void SPXData::Print(void) {

	if(dataFormat.IsSpectrum()) {
		PrintSpectrum();
	 //} else if(dataFormat.IsHERAFitter()) {
	 //PrintHERAFitter();
	}
}

void SPXData::PrintTotalCrossSection(void){
 std::string mn = "PrintTotalCrossSection: ";
 if (debug) SPXUtilities::PrintMethodHeader(cn, mn);

 double sigmatot=0.;
 for (int i = 0; i < numberOfBins; i++) {
  double binw=data["xhigh"][i]-data["xlow"][i];
  sigmatot+=data["sigma"][i]*binw;
 }

 std::cout<<cn<<mn<<"Data: "<<pci.dataSteeringFile.GetDataFile()<<std::endl;
 std::cout<<cn<<mn<<"Total Cross section: "<<sigmatot<<std::endl;
 
 return;
};

void SPXData::PrintSpectrum(void) {
	std::string mn = "PrintSpectrum: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	std::cout << std::endl << "Spectrum Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;
                                                                                                                 
	std::cout << "============================================================================================" << std::endl;
	std::cout << "|   xm       |  xlow      |  xhigh     |   sigma    |   stat     |  syst +    |  syst -    |" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//Show 4 decimal places
		std::cout << std::fixed;
		std::cout.precision(6);

		std::cout << "| ";
		std::cout.width(10);
		std::cout << data["xm"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xlow"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xhigh"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["sigma"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["stat"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst_p"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst_n"][i];
		std::cout << " |" << std::endl;
	}

	std::cout << "============================================================================================" << std::endl;

	std::cout << "This data has " << individualSystematics.size() / 2<< " Individual Systematic Errors:" << std::endl << std::endl;

        if (TakeSignforTotalError) 
         std::cout << "Take sign into account when calculating total error, i.e. Total systematics is calculated such that all negative/positive systematics go in negative/positive total uncertainty "<<std::endl;
        else
         std::cout << "Take sign NOT into account when calculating total error, i.e. Total systematics is calculated directly from positive/negative components "<<std::endl;
        if (debug) {
         this->PrintSystematics(individualSystematics);
        }
	//Iterate over individual systematic errors
	//for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
	//	const std::string &syst_name = it->first;
	//	std::vector<double> &systematic = it->second;
	//
	//	std::cout << std::left << std::setw(24) << syst_name << "  ";
	//	std::cout << std::fixed;
	//	std::cout.precision(4);
	//	for(int j = 0; j < systematic.size(); j++) {
	//		std::cout.width(10);
	//		std::cout << systematic.at(j) << " ";
	//	}
	//	std::cout << std::endl;
	//}

	//std::cout << std::endl << std::endl;
}

/*
void SPXData::PrintHERAFitter(void) {
	std::string mn = "PrintHERAFitter: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
	
	std::cout << std::endl << "HERAFitter Data File: " << pci.dataSteeringFile.GetDataFile() << std::endl << std::endl;
                                                                                                                 
	std::cout << "============================================================================================" << std::endl;
	std::cout << "|    xm      |  xlow      |  xhigh     |  sigma     |  stat      | syst +     |  syst -    |" << std::endl;                    
	std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

	//Iterate over map
	for(int i = 0; i < numberOfBins; i++) {
		//StringDoubleVectorPair_T p;

		//Show 4 decimal places
		std::cout << std::fixed;
		std::cout.precision(4);

		std::cout << "| ";
		std::cout.width(10);
		std::cout << data["xm"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xlow"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["xhigh"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["sigma"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["stat"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst_p"][i];
		std::cout << " | ";
		std::cout.width(10); std::cout << data["syst_n"][i];
		std::cout << " |" << std::endl;
	}

	std::cout << "============================================================================================" << std::endl;

	std::cout << "This data has " << individualSystematics.size() << " Individual Systematic Errors (including asymmetries):" << std::endl << std::endl;

	//Iterate over individual systematic errors
	for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
		const std::string &syst_name = it->first;
		std::vector<double> &systematic = it->second;

		std::cout << std::left << std::setw(24) << syst_name << "  ";
		std::cout << std::fixed;
		std::cout.precision(4);
		for(int j = 0; j < systematic.size(); j++) {
			std::cout.width(10);
			std::cout << systematic.at(j) << " ";
		}
		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl << std::endl;
}
*/

void SPXData::PrintGraphs(void) {
 std::string mn = "PrintGraphs: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if (totalErrorGraph) {
  std::cout<<cn<<mn<<"totalErrorGraph "<<totalErrorGraph->GetName()<<std::endl;
  totalErrorGraph->Print();
 } else
  std::cout<<cn<<mn<<"totalErrorGraph is missing "<<std::endl;

 if (statisticalErrorGraph) {
  std::cout<<cn<<mn<<"statisticalErrorGraph "<<statisticalErrorGraph->GetName()<<std::endl;
  statisticalErrorGraph->Print();
 } else
  std::cout<<cn<<mn<<"statisticalErrorGraph is missing "<<std::endl;

 if (systematicErrorGraph) {
  std::cout<<cn<<mn<<"systematicErrorGraph "<<systematicErrorGraph->GetName()<<std::endl;
  systematicErrorGraph->Print();
 } else
  std::cout<<cn<<mn<<"systematicErrorGraph is missing "<<std::endl;

 return;
}
void SPXData::CreateGraphs(void) {
	std::string mn = "CreateGraphs: ";
	if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

	//Create name strings
	TString name;
	TString statName;
	TString systName;
	TString totName;

	//Check if name exists
	if(!pci.dataSteeringFile.GetName().empty()) {
		name = TString(pci.dataSteeringFile.GetName());
		statName = name + "_stat";
		systName = name + "_syst";
		totName  = name + "_tot";
	}

	//Data steering file has no [DESC]:name
	//Default to filename
	else {
		if(debug) std::cout << cn << mn << "Using name of Data steering as graph name" << std::endl;
		name = pci.dataSteeringFile.GetFilename();
		name.ReplaceAll(TString(".txt"), TString(""));
		//name.ReplaceAll(TString("\"), TString("_"));
		statName = name + "_stat";
		systName = name + "_syst";
		totName  = name + "_tot";
	}

	double exl[numberOfBins];		// = (xm - ((xh + xl) / 2) + ((xh - xl) / 2))
	double exh[numberOfBins];		// = (((xh + xl) / 2) + ((xh - xl) / 2) - xm)

	//Calculate exl and exh
	for(int i = 0; i < numberOfBins; i++) {
		double m, n;

		m = (data["xhigh"][i] + data["xlow"][i]) / 2;
		n = (data["xhigh"][i] - data["xlow"][i]) / 2;

		exl[i] = data["xm"][i] - m + n;
		exh[i] = m + n - data["xm"][i];
	}

	//Convert to raw number if errors are given in percent
	if(pci.dataSteeringFile.IsErrorInPercent()) {
		if(debug) std::cout << cn << mn << "Errors were given in percent: Converting to raw numbers" << std::endl;

		for(int i = 0; i < numberOfBins; i++) {
			//Convert stat errors
			data["stat"][i] *= data["sigma"][i] / 100.;

			//Convert syst errors
			data["syst_p"][i] *= data["sigma"][i] / 100.;
			data["syst_n"][i] *= data["sigma"][i] / 100.;
		}
	}

	double *x = &data["xm"][0];
	double *y = &data["sigma"][0];
	double *stat = &data["stat"][0];
	double *eyl_syst = &data["syst_n"][0];
	double *eyh_syst = &data["syst_p"][0];

	std::vector<double> eyl_tot_v;
	std::vector<double> eyh_tot_v;

	//Calculate total errors = sqrt(stat^2 + syst^2)
	for(int i = 0; i < numberOfBins; i++) {
		double eylt;
		double eyht;

		eylt = sqrt(pow(stat[i], 2.0) + pow(eyl_syst[i], 2.0));
		eyht = sqrt(pow(stat[i], 2.0) + pow(eyh_syst[i], 2.0));

		eyl_tot_v.push_back(eylt);
		eyh_tot_v.push_back(eyht);
	}

	double *eyl_tot = &eyl_tot_v[0];
	double *eyh_tot = &eyh_tot_v[0];

	//Create statistical error graph
	statisticalErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, stat, stat);

	//Create systematic error graph
	systematicErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, eyl_syst, eyh_syst);

	//Create total error graph
	totalErrorGraph = new TGraphAsymmErrors(numberOfBins, x, y, exl, exh, eyl_tot, eyh_tot);

	//Modify names
	statisticalErrorGraph->SetName(statName);
	systematicErrorGraph->SetName(systName);
	totalErrorGraph->SetName(totName);

	//if(debug) {
	//	std::cout << std::endl;
	//	std::cout << cn << mn << "Statistical Error Graph created with name: " << statName << std::endl;
	//	statisticalErrorGraph->Print();
	//	std::cout << std::endl;
	//	std::cout << cn << mn << "Systematic Error Graph created with name: " << systName << std::endl;
	//	systematicErrorGraph->Print();
	//	std::cout << std::endl << std::endl;
	//	std::cout << cn << mn << "Total Error Graph created with name: " << totName << std::endl;
	//	totalErrorGraph->Print();
	//	std::cout << std::endl;
	//}
}


void SPXData::ReadCorrelation()
{
 std::string mn = "ReadCorrelation: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 std::string corrtotalfilename=pci.dataSteeringFile.GetTotalCorrellationFileName();
 if (debug) std::cout <<cn<<mn<<"Read total correlations corrfilename= "<<corrtotalfilename<< std::endl;
 if (corrtotalfilename.empty()) {
  std::cout <<cn<<mn<<"INFO: Total correlation file is empty "<<std::endl;
 }

 std::string corrstatfilename=pci.dataSteeringFile.GetStatCorrellationFileName();
 if (corrstatfilename.empty()) {
  std::cout <<cn<<mn<<"INFO: Statistical correlation file is empty "<<std::endl;
 } else {
  if (debug) std::cout <<cn<<mn<<"Read statistical correlations corrfilename= "<<corrstatfilename<< std::endl;
 }

 if (!totalErrorGraph) {
  throw SPXParseException(cn+mn+"Total data graph totalErrorGraph not found !");
 }

 //if (debug) {
 // std::cout <<cn<<mn<<"Print totalErrorGraph: "<< std::endl; 
 // totalErrorGraph->Print("all");
 //}

 if (!statisticalErrorGraph) {
  throw SPXParseException(cn+mn+"Statistical data graph statistical ErrorGraph not found !");
 }

 /*
 if (debug) {
  std::cout <<cn<<mn<<"Print statisticalErrorGraph: "<< std::endl; 
  statisticalErrorGraph->Print("all");
 }

 if (!systematicErrorGraph) {
  throw SPXParseException(cn+mn+"Systematics data graph tsystematicErrorGraph not found !");
 }

 if (debug) {
  std::cout <<cn<<mn<<"Print systematics ErrorGraph: "<< std::endl; 
  systematicErrorGraph->Print("all");
 }
*/

 bool hastotal=false;
 bool hasstatistical=false;
 if (corrtotalfilename.size()>0) hastotal=true;
 if (corrstatfilename.size()>0) hasstatistical=true;

 if (hastotal)
  std::cout <<cn<<mn<<"INFO: Total correllation matrix provided ! "<< std::endl; 

 if (hasstatistical)
  std::cout <<cn<<mn<<"INFO: Statistical correllation matrix provided ! "<< std::endl; 

 // create covariance and correlation matrices
 const int nbin=totalErrorGraph->GetN();
 if (debug) std::cout <<cn<<mn<<"INFO: nbin= "<<nbin<<std::endl;

 if (hasstatistical) { 
  if (debug) std::cout <<cn<<mn<<"Create stat covariance and correlation matrix  "<<std::endl;
  cov_matrixstat = new TMatrixT<double>(nbin, nbin);
  corr_matrixstat= new TMatrixT<double>(nbin, nbin);

  if (debug) 
   std::cout <<cn<<mn<<"Read correlation matrix from: "<<corrtotalfilename<< std::endl; 

  this->ReadCorrelationMatrix(corrstatfilename);

 } else {
  cov_matrixstat  = 0;  
  corr_matrixstat = 0;
 }

 if (hastotal) {
  cov_matrixtot  = new TMatrixT<double>(nbin, nbin);
  corr_matrixtot = new TMatrixT<double>(nbin, nbin);

  if (debug) 
   std::cout <<cn<<mn<<"Read correlation matrix from: "<<corrtotalfilename<< std::endl; 

  this->ReadCorrelationMatrix(corrtotalfilename);

 } else {

  if (debug) std::cout <<cn<<mn<<"Create total covariance and correlation matrix ! "<<std::endl;
  cov_matrixtot  = 0;
  corr_matrixtot = 0;

  if (!hasstatistical) {
   if (debug) std::cout <<cn<<mn<<"Calculate statistical (diagonal) covariance matrix from statistical uncertainties"<< std::endl; 

   cov_matrixstat   = new TMatrixT<double>(nbin, nbin);
   corr_matrixstat  = new TMatrixT<double>(nbin, nbin);
   corr_matrixstat->UnitMatrix();

   if (debug) std::cout <<cn<<mn<<"INFO: correlation matrix set to unity ! "<< std::endl; 

   for (int ibin=0; ibin<nbin; ibin++){
    double eystath=statisticalErrorGraph->GetErrorYhigh(ibin);
    double eystatl=statisticalErrorGraph->GetErrorYlow(ibin);
    double eystat=0.5*(eystath+eystatl);

    for (int jbin=0; jbin<nbin; jbin++){
     double val=0.;
     if (ibin==jbin) val=eystat;
     (*cov_matrixstat)(ibin,jbin)=val*val;
    }
   }

   //if (debug) {
   // std::cout <<cn<<mn<<"Statistical covariance matrix: "<< std::endl; 
   // cov_matrixstat->Print();

   //std::cout <<cn<<mn<<"Statistical correlation matrix: "<< std::endl; 
   // corr_matrixstat->Print();
   // }
  }

  if (debug) {
   std::cout<<cn<<mn<<"Statistical covariance matrix: "<< std::endl; 
   cov_matrixstat->Print();
  }

  if (debug) 
   std::cout<<cn<<mn<<"INFO: Calculate systematic covariance matrix from systematic components ! "<< std::endl; 

  if (individualSystematics.size()==0) {
   std::cout<<cn<<mn<<"WARNING no systematics components found ! "<< std::endl; 
   std::cerr<<cn<<mn<<"WARNING no systematics components found ! "<< std::endl; 
  } else {
   this->CalculateSystematicCovarianceMatrix();
   std::cout<<cn<<mn<<"Finished calculation of systematic covariance matrix"<< std::endl; 
  }
<<<<<<< HEAD
 
  // add up stat and syst covariance matrice
  if (debug) std::cout <<cn<<mn<<"Add up stat and syst covariance matrices "<< std::endl; 
=======
>>>>>>> release-0.98
  //
  // calculate total covariance
  //
  if (!cov_matrixsyst) {
   std::cout <<cn<<mn<<"WARNING: Systematic covariance matrix not found ! "<< std::endl; 
   std::cerr <<cn<<mn<<"WARNING: Systematic covariance matrix not found ! "<< std::endl; 
   //throw SPXParseException(cn+mn+"WARNING: Systematic covariance matrix not found ! ");
   return;
  }

  //if (debug) {
   std::cout<<cn<<mn<<"Print systematic covariance matrix "<< std::endl; 
   cov_matrixsyst->Print();
   //}

  // add up stat and syst covariance matrice
  if (debug) std::cout <<cn<<mn<<"Add up stat and syst covariance matrices "<< std::endl; 

  //cov_matrixtot->Plus(*cov_matrixstat,*cov_matrixsyst);
  cov_matrixtot = new TMatrixT<double>(*cov_matrixstat,TMatrixD::kPlus,*cov_matrixsyst);

  if (!cov_matrixtot) {
   std::cout <<cn<<mn<<"WARNING: Total covariance matrix not found ! "<< std::endl; 
   std::cerr <<cn<<mn<<"WARNING: Total covariance matrix not found ! "<< std::endl; 
   return;
  }
 
  if (debug) {
   std::cout<<cn<<mn<<"Print total covariance matrix "<< std::endl; 
   cov_matrixtot->Print();
  }   

 }

 return;

}

void SPXData::ReadCorrelationMatrix(std::string filename) {
 std::string mn = "ReadCorrelationMatrix: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if(filename.empty()) {
  throw SPXFileIOException(cn+mn+"Filename is empty !");
 }

 filename=pci.dataDirectory+"/"+filename;

 std::ifstream infile(filename.c_str(), std::ios::in);
 if(!infile){ // Check open
  infile.close();
  std::ostringstream oss;
  oss <<cn<<mn<< "Can't open " << filename;
  throw SPXParseException(oss.str());
 } else {
  if (debug) std::cout <<cn<<mn<<"Read data correlations file: " << filename.c_str() << std::endl;
 }

 //Bin count to make sure that always the same number of bins is read
 unsigned int bin_count = -1; //first index is zero
 // Line count
 unsigned int line_count = -1; //first index is zero

 //Number of columns (used to determine symmetric, asymmetric, or no total systematic error)
 unsigned int numberOfColumns = 0;

 int nbin=totalErrorGraph->GetN();
 if (nbin==0) {
  throw SPXParseException(cn+mn+"Number of bins is zero; do not know what to do");
 }

 std::string line;

 bool iscorrelationmatrix=false;
 bool iscovariancematrix=false;

 bool isstat=false;
 bool istotal=false;

 int col_num= -1;

 std::vector<double> vrow;
 std::vector< std::vector<double> > vmatrix;
 vrow.clear();
 vmatrix.clear(); 

 while (infile.good()) {
  //Skip comments
  std::getline(infile, line);
  if (line.empty()) continue;
  //String stream to parse the individual lines
  std::istringstream iss(line);
  //if(debug) std::cout << cn << mn << "Line: " << line << std::endl;

  if (!line.empty() && (line[0] == ';')) {
   continue;
  } else if(!line.empty()) {

   if (TString(line).Contains("is_correlation_matrix")) iscorrelationmatrix=true;
   if (TString(line).Contains("is_covariance_matrix"))  iscovariancematrix=true;

   if (TString(line).Contains("is_totalerror"))       istotal=true;
   if (TString(line).Contains("is_statisticserror"))  isstat=true;

   //if (isdigit((int)SPXStringUtilities::LeftTrim(line).at(0))) std::cout<<cn<<mn<<"is digit "<<std::endl;
   //if (line.at(0)=='-') std::cout<<cn<<mn<<"starts with minus sign "<<std::endl;

   if (isdigit((int)SPXStringUtilities::LeftTrim(line).at(0)) || SPXStringUtilities::LeftTrim(line).at(0)=='-') {
    //if(debug) std::cout << cn << mn << bin_count<<" Line: " << line << std::endl;

    //Convert all tabs to spaces
    std::string formatted_line = SPXStringUtilities::ReplaceAll(line, "\t", " ");
    std::vector<double> vrow = SPXStringUtilities::ParseStringToDoubleVector(formatted_line, ' ');

    line_count++;

    if (debug) std::cout<<cn<<mn<<line_count<<" Number of columns read: "<<vrow.size()<<" number of bins= "<<nbin<<std::endl;

    /*
    if (debug) {
     std::cout<<cn<<mn<<"line= " << line.c_str() << std::endl;
     std::cout<<cn<<mn<<"push vrow to matrix bin_count= " << bin_count << std::endl;
     for (int j=0; j<vrow.size(); j++) {
      std::cout<<" j= "<<j<<" vrow= "<<vrow[j]<<std::endl;
     }
    }
    */

    if (RemoveXbins) {
     /*
     if (debug) {
      std::cout<<cn<<mn<<"Remove bins with values < "<<DataCutXmin<<" and > "<<DataCutXmax<<std::endl;

      std::cout<<cn<<mn<<"After looping over data: The following bins will be kept: " << std::endl;
      std::cout<<cn<<mn<<"INFO: keepbin[ keptbinindex ]= newbinindex " <<std::endl;
      for (std::map<int,int>::iterator it = keepbin.begin(); it != keepbin.end(); it++) {
       std::cout<<cn<<mn <<"keepbin["<<it->first<<"]= " <<  it->second<<std::endl;
      }
      std::cout<<cn<<mn <<" " <<std::endl;
     }
     */

     std::vector<double> vrowtmp;
     for (int i=0; i<vrow.size(); i++) {
       //if (debug) std::cout<<cn<<mn<<"line= "<<line_count<<" vrow[ "<<i<<"]= "<<vrow.at(i)<<std::endl;
      if (keepbin.count(i)>0) {
       vrowtmp.push_back(vrow.at(i));
      } else {
	if (debug) std::cout<<cn<<mn<<"-----> Bin i= "<<i<<" is removed ! "<<std::endl;
      }
     }
     //if (debug) {
     // for (int i=0; i<vrowtmp.size(); i++) {
     //  if (debug) std::cout<<cn<<mn<<"vrowtmp[ "<<i<<"]= "<<vrowtmp.at(i)<<std::endl;
     // }
     //}
     // replace vrow vector
     vrow=vrowtmp;
    }

    //
    //Set number of columns
    //
    if(vrow.size()!= nbin) {
     std::ostringstream oss;
     oss<<cn<<mn<<"Number of columns read= "<< vrow.size() <<"  but nbin= "<<nbin<<" for filename= "<<filename;
     std::cout<<cn<<mn<<"Number of columns read= "<< vrow.size() <<"  but nbin= "<<nbin<<" for filename= "<<filename<<std::endl;
     throw SPXParseException(oss.str());
    }

    if (RemoveXbins) {
     if (keepbin.count(line_count)>0) {
      bin_count++;
      //if (debug) std::cout<<cn<<mn<<"vmatrix are bin_count= "<<bin_count<<" is filled with vrow["<<line_count<<"]"<<std::endl;
      vmatrix.push_back(vrow);
     } else {
      if (debug) std::cout<<cn<<mn<<"bin "<<bin_count<<" is removed from vmatrix "<<std::endl;
     }
    } else {
     bin_count++;
     vmatrix.push_back(vrow);
    }

    //if (debug) std::cout<<cn<<mn<<"pushed vrow to matrix bin_count= " << bin_count << std::endl;

   } //else if (debug) std::cout<<cn<<mn<<"Line does not start with a digit " << line.c_str() << std::endl;
  } //else if (debug)  std::cout<<cn<<mn<<"Line  " << line.c_str() << std::endl;
 }


 /*
 if (debug) {
  std::cout<<cn<<mn<<"Matrix  " << std::endl;
  for (int i=0; i<vmatrix.size(); i++) {
   vrow=vmatrix[i];
   for (int j=0; j<vrow.size(); j++) {
     std::cout<<" i=  " << i<<" j= "<<j<<" v= "<<vrow[j]<<std::endl;
   }
  }
 }
 */

 if (debug) {
  std::cout<<cn<<mn<<"  " << std::endl;
  if (isstat)  std::cout<<cn<<mn<<"Matrix " << filename<<" has statistical covariance/correllation matrix " << std::endl;
  if (istotal) std::cout<<cn<<mn<<"Matrix " << filename<<" has total covariance/correlation matrix" << std::endl;
 }


 if (isstat&&istotal) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Matrix " << filename<<" can either have statistical or total correllation; do not know what to do !";
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 }


 if (!isstat&&!istotal) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Matrix " << filename<<" has to have either statistical or total correlation; do not know what to do !";
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 }


 if (debug) {
  std::cout<<cn<<mn<<"  " << std::endl;
  if (iscorrelationmatrix) std::cout<<cn<<mn<<"Matrix " << filename<<" is correlation matrix " << std::endl;
  if (iscovariancematrix) std::cout<<cn<<mn<<"Matrix " << filename<<" is covariance matrix " << std::endl;
  std::cout<<cn<<mn<<"  " << std::endl;
 }


 if (iscorrelationmatrix&&iscovariancematrix) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Matrix " << filename<<" can either be correlation or covariance matrix; do not know what to do !";
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 }


 if (!iscorrelationmatrix&&!iscovariancematrix) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Matrix " << filename<<" has to be either correlation or covariance matrix; do not know what to do !";
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 }


 if (isstat&&cov_matrixstat==0) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Matrix " << filename<<" is statistical, but matrix is not provided !";
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 } 

 if (istotal&&cov_matrixtot==0) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Matrix " << filename<<" is total, but matrix is not provided !";
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 } 

 if (totalErrorGraph->GetN()!=vmatrix.size()) {
  std::ostringstream oss;
  oss <<cn<<mn<< "Mismatch in number of bins is total from cross-section table= "
              <<totalErrorGraph->GetN()<<" from covariance matrix= "<<vmatrix.size();
  std::cout<<oss.str()<<std::endl;
  throw SPXParseException(oss.str());
 } 

 //
 // now fill correlation and covariance matrix
 //

 for (int row_num=0; row_num<vmatrix.size(); row_num++) { 
  for (int col_num=0; col_num<vmatrix[row_num].size(); col_num++) {
   double  val=vmatrix[row_num][col_num];
   double cval=vmatrix[row_num][row_num]*vmatrix[col_num][col_num];

   if (cval<0) {
    std::cout<<cn<<mn<<"WARNING value can not be negative col_num="<<col_num
                        <<" row_num= "<<row_num<<std::endl;
    std::cerr<<cn<<mn<<"WARNING value can not be negative "<<std::endl;
    cval=0.; 
   } else {
    cval=sqrt(cval);
   }

   //if (debug) {
   //std::cout<<cn<<mn<<"row_num= "<<row_num<< " col_num "<<col_num<<" val= "<<val<<std::endl;
   //}

   // correlation matrix divided by total error in row/col
   double cov_val = 0., corr_val=0.;
   if (iscovariancematrix) {
    cov_val =val;
    //corr_val=val/(row_data_err*col_data_err);
    corr_val=val/(cval);
   }
   if (iscorrelationmatrix) {
    //cov_val =corr_val*row_data_err*col_data_err;
    cov_val =corr_val*cval;
    corr_val=val;
   } 

   /*
   if (debug) {
    std::cout<<cn<<mn<<"row_num= "<<row_num<< " col_num "<<col_num<<" val= "<<val
	             <<" cov_val= "<<cov_val<< " corr_val= "<<corr_val<<std::endl;
   }
   */

   if (istotal) {
    (*cov_matrixtot )(row_num, col_num) = cov_val;
    (*corr_matrixtot)(row_num, col_num)= corr_val;
   }

   if (isstat) {
    (*cov_matrixstat )(row_num, col_num) = cov_val;
    (*corr_matrixstat)(row_num, col_num)= corr_val;
   }
  }
 }

 // if (debug) {
  if (istotal) {
   std::cout<<"  " << std::endl;
   std::cout<<cn<<mn<<"Print total correlation matrix: " << std::endl;
   corr_matrixtot->Print();

   std::cout<<cn<<mn<<"Print total covariance matrix: " << std::endl;
   cov_matrixtot->Print();
  }

  if (isstat) {
   std::cout<<"  " << std::endl;
   std::cout<<cn<<mn<<"Print statistical correlation matrix: " << std::endl;
   corr_matrixstat->Print();

   std::cout<<cn<<mn<<"Print statistical covariance matrix: " << std::endl;
   cov_matrixstat->Print();
  }
// }

 return;
}

void SPXData::CalculateSystematicCovarianceMatrix() {
 std::string mn = "CalculateSystematicCovarianceMatrix: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 //if (!systematicErrorGraph)
 // throw SPXParseException(cn+mn+"Systematics data graph systematicErrorGraph not found !");

 int Nbin=this->GetNumberOfBins();

 if (debug) std::cout<<cn<<mn<<"Nbin= " << Nbin << std::endl;

 cov_matrixsyst = new TMatrixT<double>(Nbin, Nbin);
 corr_matrixsyst= new TMatrixT<double>(Nbin, Nbin);

 double covmatrixsyst[Nbin][Nbin];
 double covmatrixuncorrsyst[Nbin][Nbin];
 for (int ibin=0; ibin<Nbin; ibin++) {
  for (int jbin=0; jbin<Nbin; jbin++) {
   covmatrixsyst[ibin][jbin]=0.;
   covmatrixuncorrsyst[ibin][jbin]=0.;
  }
 }

 //std::cout<<cn<<mn<<" Print individualSystematics "<<std::endl;
 //this->PrintMap(individualSystematics);

 StringDoubleVectorMap_T symsystmap=SPXData::SymmetrizeSystematicUncertaintyMatrix(individualSystematics);

 if (debug) {
  std::cout<<cn<<mn<<"Print symsystmap"<<std::endl;
  this->PrintMap(symsystmap);
 }

 for(StringDoubleVectorMap_T::iterator it = symsystmap.begin(); it != symsystmap.end(); ++it) {

  const std::string   &name = it->first;
  std::vector<double> &syst = it->second;

  std::cout<<cn<<mn<<"Number of bins in systematics vector: "<<syst.size()  << std::endl;
  for (int i=0; i<syst.size(); i++) {
   std::cout<<cn<<mn<<" syst["<<i<<"]= "<<syst.at(i)  << std::endl;
  }

  if (syst.size()==0)
   std::cout<<cn<<mn<<"WARNING: no systematic components founds ! "<< std::endl;

  if (Nbin!=syst.size()) {
   std::ostringstream oss;
   oss <<cn<<mn<<"Nbin= " << Nbin <<" not consistent with syst compenents vector size= "<<syst.size();
   throw SPXParseException(oss.str());
  }

  bool systtype=this->GetSystematicCorrelationType(name);
  if (debug) std::cout<<cn<<mn<<"name= "<<name.c_str() << " type= "<<(systtype ? " CORRELATED" : " UNCORRELATED") << std::endl;

  //if (debug) std::cout<<cn<<mn<<"Loop over Nbin: "<<syst.size()  << std::endl;
  for ( int ibin=0; ibin<Nbin; ibin++ ){
   for ( int jbin=0; jbin<Nbin; jbin++ ){
    // if (debug) std::cout<<cn<<mn<<ibin<<" "<<jbin<<" systematic name= "<<name
    //		             <<" erri= "<<syst.at(ibin)<<" errj= "<<syst.at(jbin)
    //                       <<< (systtype ? " CORRELATED" : " UNCORRELATED")
    //                        <<std::endl;       
    if (systtype) {   
     covmatrixsyst[ibin][jbin]+=syst.at(ibin)*syst.at(jbin);          
    } else { 
     if (ibin==jbin) // uncorrelated systematic are in a diagonal matrix
      covmatrixuncorrsyst[ibin][jbin]+=syst.at(ibin)*syst.at(jbin);          
    }
   }
  }
 }

 // add up matrices of correlated and uncorrelated systematics
 for ( int ibin=0; ibin<Nbin; ibin++ ){
  for ( int jbin=0; jbin<Nbin; jbin++ ){
   covmatrixsyst[ibin][jbin]+=covmatrixuncorrsyst[ibin][jbin];          
  }
 }

 // Fill covariance matrix
 for (int ibin=0; ibin<Nbin; ibin++) {
  for (int jbin=0; jbin<Nbin; jbin++) {
   (*cov_matrixsyst)(ibin,jbin)=covmatrixsyst[ibin][jbin];
   double dia=covmatrixsyst[ibin][ibin]*covmatrixsyst[jbin][jbin];
   if (dia>0.) {
    dia=sqrt(dia);
    (*corr_matrixsyst)(ibin,jbin)=covmatrixsyst[ibin][jbin]/dia;   
   } else {    
    std::cout<<cn<<mn<<"WARNING Diagonal element should not be negative !"<<std::endl;
    std::cerr<<cn<<mn<<"WARNING Diagonal element should not be negative !"<<std::endl;
    (*corr_matrixsyst)(ibin,jbin)=0.;
   }

  }
 }
 
 if (debug) {
  std::cout<<"  " << std::endl;
  std::cout<<cn<<mn<<"Print systematic correlation matrix: " << std::endl;
  corr_matrixsyst->Print();

  std::cout<<cn<<mn<<"Print systematic covariance matrix: " << std::endl;
  cov_matrixsyst->Print();
 }

 return;
}

StringDoubleVectorMap_T SPXData::SymmetrizeSystematicUncertaintyMatrix(StringDoubleVectorMap_T systmap) {
 std::string mn = "SymmetrizeSystematicUncertaintyMatrix: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
 //
 // prepares map for calculation of covariance matrix
 //
 // - multiplies by total, i.e. converts relative to absolute error
 // - does symmetrization (see SPXpValue)
 //      

 bool debug2=false;

 if (debug2) std::cout<<cn<<mn<<"Loop over systematics Number of systematics= "<< systmap.size()  << std::endl;

 // declare map containing symmetrized systematic uncertainties
 std::map <std::string, std::vector <double> >  symsystmap;

 //
 // Loop over systematic uncertainty components
 //

 for(StringDoubleVectorMap_T::iterator it =systmap.begin(); it != systmap.end(); ++it) {

  const std::string   &name = it->first;
  std::vector<double> &syst = it->second;

  if (debug2) std::cout<<cn<<mn<<"Systematic name= "<< name  << std::endl;

  TString sname=name;
  if (name.find("+") != std::string::npos) {
   if (debug2) std::cout<<cn<<mn<<"Positive systematic name= "<< name  << std::endl;
   sname.ReplaceAll("+","-");
   std::vector <double> vtmp;
   vtmp.clear();

   if (systmap.count(std::string(sname.Data()))) {
    if (debug2) std::cout<<cn<<mn<<"Corresponding negative systematic found sname= "<< sname  << std::endl;
    std::vector <double> vtmp;
    for (int i=0; i<syst.size(); i++) {
     double sig=GetSigmaVector().at(i);
     //double err=0.5*(systmap[std::string(sname.Data())].at(i) + syst.at(i));
     double negi=systmap[std::string(sname.Data())].at(i);
     double posi= syst.at(i);
     double err=((posi-negi)/2.)*sig;

     if (posi*negi>0.) {
      err=(fabs(posi)+fabs(negi))/2.*sig;
     }

     std::cout<<cn<<mn<<i<<" name= "<<sname<<" sig= "<<sig<<" posi= "<<posi<<" negi= "<<negi<<" err= "<<err<<std::endl;
     
     vtmp.push_back(err);
    } 
    sname.ReplaceAll("-","");
    symsystmap[std::string(sname.Data())]=vtmp;
   } else {
    std::cout<<cn<<mn<<"INFO: Corresponding negative systematic sname= "<< sname <<" not found !" << std::endl;
   } 
  } else if (name.find("-") != std::string::npos) {
   TString sname=TString(name);
   sname.ReplaceAll("-","");
   if (symsystmap.count(std::string(sname.Data()))) {
    if (debug2) std::cout<<cn<<mn<<"Already averaged sname= "<< sname  << std::endl;
   } else {
    std::cout<<cn<<mn<<"WARNING: not yet in map, there is mismatch between +/- errors "<< sname  << std::endl;
   }
  } else {
   std::cout<<cn<<mn<<"WARNING: systematics sname= "<< sname <<" negative or positive ? " << std::endl;
   std::cout<<cn<<mn<<"WARNING: Do not know what to do....add to map " << std::endl;
   symsystmap[name]=syst;
  }
 }

 return symsystmap;

}

void SPXData::PrintSystematics(StringDoubleVectorMap_T syst) {
//Iterate over individual systematic errors
 std::string mn = "PrintSystematics: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
        
 std::cout<<cn<<mn<<"Number of systematics uncertainties= "<<syst.size()<<std::endl;

 for(StringDoubleVectorMap_T::iterator it = syst.begin(); it != syst.end(); it++) {
  const std::string   &syst_name  = it->first;
  std::vector<double> &systematic = it->second;

  if (systematic.size()==0) {
   std::cout<<cn<<mn<<"WARNING: systematic error vector is empty ! " << std::endl;
   if (RemoveXbins) {
    std::cout<<cn<<mn<<"WARNING: data_cut_xmax= " << DataCutXmax<<" data_cut_xmin= " << DataCutXmin << std::endl;
   } else {
    std::cout<<cn<<mn<<"INFO: systematic error vector is empty ! " << std::endl;
   }
  }

  bool systtype=this->GetSystematicCorrelationType(syst_name);

  std::cout << std::left << std::setw(24) << syst_name << "  ";
  std::cout << std::fixed;
  std::cout.precision(4);
  for(int j = 0; j < systematic.size(); j++) {
   std::cout.width(10);
   std::cout << systematic.at(j) << " ";
  }
  std::cout << " Type= "<<(systtype ? " CORRELATED" : " UNCORRELATED")<<std::endl;
  std::cout << std::endl;
 }

 std::cout << std::endl << std::endl;
 return;
}


TGraphAsymmErrors * SPXData::GetSystematicErrorGraph(int isyst){
 std::string mn ="GetSystematicErrorGraph: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
        
 if ( individualsystematicErrorGraph.size()==0) {
  std::vector <TGraphAsymmErrors *> vsyst=this->GetSystematicsErrorGraphs();
 }

 if (isyst>individualsystematicErrorGraph.size()){
  std::ostringstream oss;
  oss <<cn<<mn<<"isyst too large isyst= "<<isyst<<" graph size= " <<individualsystematicErrorGraph.size()<<std::endl;
  throw SPXParseException(oss.str());
 }

 return individualsystematicErrorGraph.at(isyst);

}


std::vector <TGraphAsymmErrors *>  SPXData::GetSystematicsErrorGraphs(void){
 std::string mn ="GetSystematicsErrorGraphs: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 if ( individualsystematicErrorGraph.size()==0) {
  if (debug) {
   std::cout<<cn<<mn<<std::endl;  
   std::cout<<cn<<mn<<"Fill individualsystematicErrorGraph vector from map"<<std::endl;  
   //std::cout<<cn<<mn<<"Copy values from graph= "<<systematicErrorGraph->GetName()<<std::endl;
   //systematicErrorGraph->Print();
  }

  std::vector<double> csystp;
  std::vector<double> csystn;

  for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); ++it) {

   // add negative systematic to positive systematics

   std::string syst_name = it->first;
   
   if (syst_name.find("-") != std::string::npos) continue;

   if (debug) std::cout<<cn<<mn<<"Fill individualsystematicErrorGraph syst_name= "<<syst_name<<std::endl;  

   csystp = it->second;

   csystn.clear();

   std::string syst_namen=SPXData::GetCorrespondingSystematicName(syst_name);
   if (debug)  std::cout<<cn<<mn<<"Found for syst_name "<<syst_name<<" syst_namen= "<<syst_namen<<std::endl;  
   if (syst_namen.empty()) {
    std::ostringstream oss;
    oss <<cn<<mn<<"Corresponding systematics vector not found for " <<syst_namen.c_str();
    throw SPXParseException(oss.str());
   }

   if (individualSystematics.count(syst_namen)) {
    if (debug) std::cout<<cn<<mn<<"Corresponding negative systematic found name= "<< syst_namen.c_str() << std::endl;
    csystn = (individualSystematics[syst_namen]);
   }

   if (csystp.size()==0) {
    std::ostringstream oss;
    std::cerr<<cn<<mn<<"Systematics vector csyst is empty ! " <<" corresponding to "<<syst_name.c_str();
    //oss <<cn<<mn<<"Systematics vector csyst is empty ! " <<" corresponding to "<<syst_name.c_str();
    //throw SPXParseException(oss.str());
   }

   if (csystn.size()==0) {
    std::ostringstream oss;
    std::cerr<<cn<<mn<<"Corresponding systematics vector not found for " <<syst_namen.c_str()<<" corresponding to "<<syst_name.c_str();
    //oss <<cn<<mn<<"Corresponding systematics vector not found for " <<syst_namen.c_str()<<" corresponding to "<<syst_name.c_str();
    //throw SPXParseException(oss.str());
   }

   if (csystp.size()!=csystn.size()) {
    std::ostringstream oss;
    oss <<cn<<mn<<"Different size positive "<<csystp.size()<<" and negative "<<csystn.size()<<" systematics vector ";
    throw SPXParseException(oss.str());
   }

   // first copy over from total systematic
   TGraphAsymmErrors *gsyst = new TGraphAsymmErrors(*systematicErrorGraph);
   if (!gsyst) {
    throw SPXParseException(cn+mn+"Problem creating graph gsyst !");
   }

   TString sname=syst_name;
   sname.ReplaceAll("+","");
   sname.ReplaceAll("-","");
   gsyst->SetName(sname);

   // Now fill high and low systematics
   if (debug) std::cout<<cn<<mn<<"Loop over systematic vector Nbin= "<<csystp.size()<<std::endl;

   for (int ibin=0; ibin<csystp.size(); ibin++) {
    double value=0., x;
    systematicErrorGraph->GetPoint(ibin, x,value);

    double eyh=csystp.at(ibin)/100*fabs(value);
    double eyl=csystn.at(ibin)/100*fabs(value);
  
    if ((eyh<0 && eyl<0) || (eyh>0 && eyl>0) ) {
     std::ostringstream oss;
     oss << cn<<mn<<"INFO: in "<<sname<<" ibin= "<<ibin<<" errors have same sign: eyh= "<<eyh<<" eyl= "<<eyl;  
     std::cout<<oss.str()<<std::endl;
     std::cerr<<oss.str()<<std::endl;
    }

    if (eyh<0 && eyl>0) {
     std::cout<<cn<<mn<<"INFO: in ibin= "<<ibin<<" eyh<0 and exl>0 errors have switched sign: eyh= "<<eyh<<" eyl= "<<eyl<<std::endl;
     //double tmp=eyh;
     //eyh=eyl;
     //eyl=tmp;
     //std::cout<<cn<<mn<<"INFO: in ibin= "<<ibin<<" sign switched:  eyh= "<<eyh<<" eyl= "<<eyl<<std::endl;
    }

    //if (debug) std::cout<<cn<<mn<<" ibin= "<<ibin<<" Value= "<<value<<" syst= "<<csystp.at(ibin)<<" "<<csystn.at(ibin)<<" eyh= "<<eyh<<" eyl= "<<eyl<<std::endl;

    gsyst->SetPointEYhigh(ibin,eyh);
    gsyst->SetPointEYlow (ibin,eyl);
   }
   //if (debug) gsyst->Print();

   individualsystematicErrorGraph.push_back(gsyst);
  }
 } 

 return individualsystematicErrorGraph;


}

std::string SPXData::GetCorrespondingSystematicName(std::string syst_name){
 std::string mn ="GetCorrespondingSystematicName: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 std::string newsystname="";
 TString sname=syst_name;

 if (syst_name.find("+") != std::string::npos) {
  //if (debug) std::cout<<cn<<mn<<"Positive systematic name= "<< syst_name  << std::endl;
  sname.ReplaceAll("+","-");
  if (individualSystematics.count(std::string(sname.Data()))) {
    //if (debug) std::cout<<cn<<mn<<"Corresponding negative systematic found in map sname= "<< sname.Data()  << std::endl;
   newsystname=sname;
   return newsystname;
  }
 } else if (syst_name.find("-") != std::string::npos) {
  if (debug) std::cout<<cn<<mn<<"Negative systematic name= "<< syst_name  << std::endl;
  sname.ReplaceAll("-","+");
  if (individualSystematics.count(std::string(sname.Data()))) {
   //if (debug) std::cout<<cn<<mn<<"Corresponding positive systematic found in map sname= "<< sname.Data()  << std::endl;
   newsystname=sname;
   return newsystname;
  }
 } else 
 std::cout<<cn<<mn<<"WARNING: systematics sname= "<< sname.Data() <<" negative or positive ? " << std::endl;
 
 return newsystname;
};

void SPXData::OpenDataFile(void) {
 std::string filepath = pci.dataSteeringFile.GetDataFile();

 if(filepath.empty()) {
  throw SPXFileIOException("Data Filepath is empty");
 }

 try {
  dataFile = new std::ifstream();
  dataFile->open(filepath.c_str());

  if(*dataFile) {
    if (debug) std::cout << "SPXData::OpenDataFile: Successfully opened data file: " << filepath << std::endl;
  } else {
   throw SPXFileIOException(filepath, "Unable to open data file");
  }
 } catch(const std::exception &e) {
  throw;
 }
}

void SPXData::CheckVectorSize(const std::vector<double> & vector, const std::string & name, unsigned int masterSize) {
 std::string mn ="kVectorSize: ";
 if(vector.size() != masterSize) {
  std::ostringstream oss;
  for (int i=0; i<vector.size(); i++) {
    std::cout<<cn<<mn<<" vector["<<i<<"]= "<<vector.at(i)<<std::endl;
  }
  oss << "Size error: \"" << name << "\" vector has different size (" << vector.size() << ") than master size (" << masterSize << ")" << std::endl;
  throw SPXParseException(pci.dataSteeringFile.GetDataFile(), oss.str());
 } else {
   if (debug) std::cout <<cn<<mn<< "SPXData::" << "CheckVectorSize: " << "\t -->  Success: \"" << name << "\" vector size matches master size" << std::endl;
 }
}


bool SPXData::GetSystematicCorrelationType(std::string name) {
 std::string mn ="GetSystematicCorrelationType: ";
 bool type=true;
 bool found=false;

 if (name.find("+") != std::string::npos || name.find("-") != std::string::npos) {
  if (individualSystematicsIsCorrelated.count(name)>0) {
   found=true;
   type=individualSystematicsIsCorrelated[name];
  }
 } else {
  if (individualSystematicsIsCorrelated.count(name)>0) {
   found=true;
   type=individualSystematicsIsCorrelated[name];
  } else {
   std::string namep=name+"+";  std::string namen=name+"-";
   if (individualSystematicsIsCorrelated.count(namep)>0) {
    found=true;
    type=individualSystematicsIsCorrelated[namep];
   }
   if (individualSystematicsIsCorrelated.count(namen)>0) {
    found=true;
    type=individualSystematicsIsCorrelated[namen];
   }
  }
 }

 /*
  if (debug) {
   std::cout<<cn<<"GetSystematicCorrelationType: name= "<<name.c_str()<< (found ? " found" : " not found")
                                            << (type  ? " CORRELATED" : " UNCORRELATED")<<std::endl; 
  }
 */
 if (!found) {
  throw SPXParseException(cn+"GetSystematicCorrelationType: Can not find systematics in individualSystematicsIsCorrelated map name= "+name);
 }

 return type;
}

void  SPXData::RemoveSystematicthatContains(std::string systclassname){
 std::string mn ="RemoveSystematicthatContains: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 int nremoved=0;

 StringDoubleVectorMap_T::iterator itr =  individualSystematics.begin();
 while (itr != individualSystematics.end()) {
  const std::string   &systname  = itr->first;
  //std::cout<<" key= "<< systname <<std::endl;
  TString mapname=TString(systname);

  //if (debug) std::cout <<cn<<mn<<"Check if systematics= "<<mapname.Data()<< " contains= "<<systclassname<<std::endl;

  if (mapname.Contains(systclassname,TString::kIgnoreCase)) {
   if (debug) std::cout<<cn<<mn<<"Systematics name class "<<systclassname<<" -> remove in map with systname= "<<mapname.Data()<<std::endl;
   individualSystematics.erase(itr++);
   nremoved++;
  } else {
   ++itr;
  }
 }
 
 if (nremoved>0) {
  if (debug) std::cout<<cn<<mn<<"Number of removed systematics nremoved= "<<nremoved<<std::endl;
 }

 UpdateSystematics();
 
 if (debug) {
  if (debug) std::cout<<cn<<mn<<"After UpdateSystematics PrintSpectrum "<<std::endl;
  PrintSpectrum();
 }

 return;
};

void  SPXData::KeepSystematicthatContains(std::string systclassname){
 std::string mn ="KeepSystematicthatContains: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);

 int nremoved=0;
 int nkeep=0;

 StringDoubleVectorMap_T::iterator itr =  individualSystematics.begin();
 while (itr != individualSystematics.end()) {
  const std::string   &systname  = itr->first;
  TString mapname=TString(systname);

  //if (debug) std::cout <<cn<<mn<<"Check if systematics= "<<mapname.Data()<< " contains= "<<systclassname<<std::endl;

  if (!mapname.Contains(systclassname,TString::kIgnoreCase)) {
   if (debug) std::cout<<cn<<mn<<"Systematics name class "<<systclassname<<" does not contain systname= "<<mapname.Data()<<std::endl;
   individualSystematics.erase(itr++);
   nremoved++;
  } else {
   if (debug) std::cout<<cn<<mn<<"Systematics name kept in class "<<systclassname<<" systname= "<<mapname.Data()<<std::endl;
   nkeep++;
   ++itr;
  }
 }
 
 if (nremoved>0) {
  if (debug) std::cout<<cn<<mn<<"Number of removed systematics nremoved= "<<nremoved<<std::endl;
 }

 if (nkeep>0) {
  if (debug) std::cout<<cn<<mn<<"Number of kept systematics nkeep= "<<nkeep<<std::endl;
 }


 UpdateSystematics();
 
 return;
};


void SPXData::UpdateSystematics(void){
 std::string mn ="UpdateSystematics: ";
 if(debug) SPXUtilities::PrintMethodHeader(cn, mn);
 //
 // recalculate total systematic error from individual errors components
 // update data map
 //
 const int nbin=data["xm"].size();
 if (debug) std::cout<<cn<<mn<<"nbin= "<<nbin<<std::endl;

 std::vector<double> vsysttotn(nbin,0.);
 std::vector<double> vsysttotp(nbin,0.);

 if ( individualSystematics.size()==0) {
  if (debug) std::cout<<cn<<mn<<"No systematics components found !"<<std::endl;
  data["syst_m"]=vsysttotp;
  data["syst_n"]=vsysttotn;
  return;
 }

 if (debug) std::cout<<cn<<mn<<"Recalculate total uncertainty: "<<std::endl;
 if (debug) if (TakeSignforTotalError) std::cout<<cn<<mn<<"Take sign into account for adding in quadrature "<<std::endl;  

 for(StringDoubleVectorMap_T::iterator it = individualSystematics.begin(); it != individualSystematics.end(); it++) {
  const std::string   &syst_name  = it->first;
  std::vector<double> &systematic = it->second;

  //if (debug) std::cout<<cn<<mn<<"syst_name= "<<syst_name<<std::endl;


  for (int ibin=0; ibin<nbin; ibin++) {
   if(syst_name.find("+") != std::string::npos) {

    if (TakeSignforTotalError) {
     if (systematic.at(ibin)>0) vsysttotp.at(ibin)+=pow(systematic.at(ibin),2);
     else                       vsysttotn.at(ibin)+=pow(systematic.at(ibin),2);
    } else                      vsysttotp.at(ibin)+=pow(systematic.at(ibin),2);

    //std::cout<<cn<<mn<<"bin= "<<ibin<<" "<<syst_name<<" up "<<systematic.at(ibin)
    //         <<" vsysttotp= "<<vsysttotp.at(ibin)<<" vsysttotn= "<<vsysttotn.at(ibin)<<std::endl;

   } else {
    if (TakeSignforTotalError) {
     if (systematic.at(ibin)>0) vsysttotp.at(ibin)+=pow(systematic.at(ibin),2);
     else                       vsysttotn.at(ibin)+=pow(systematic.at(ibin),2);
    } else                      vsysttotn.at(ibin)+=pow(systematic.at(ibin),2);

    //std::cout<<cn<<mn<<"bin= "<<ibin<<" "<<syst_name<<" dn "<<systematic.at(ibin)
    //         <<" vsysttotp= "<<vsysttotp.at(ibin)<<" vsysttotn= "<<vsysttotn.at(ibin)<<std::endl;
   }

   if (debug) {
    std::cout<<cn<<mn<<"bin= "<<ibin<<" "<<syst_name<<" up "<<systematic.at(ibin)<<" sum "<<vsysttotp.at(ibin)<<" sqrt= "<<sqrt(vsysttotp.at(ibin))<<std::endl;
    std::cout<<cn<<mn<<"bin= "<<ibin<<" "<<syst_name<<" dn "<<systematic.at(ibin)<<" sum "<<vsysttotn.at(ibin)<<" sqrt= "<<sqrt(vsysttotn.at(ibin))<<std::endl;
   }

  }
 }

 for (int ibin=0; ibin<nbin; ibin++) {
  vsysttotn.at(ibin)=sqrt(vsysttotn.at(ibin));
 }

 for (int ibin=0; ibin<nbin; ibin++) {
  vsysttotp.at(ibin)=sqrt(vsysttotp.at(ibin));
 }

 if (debug) { 
  for (int ibin=0; ibin<nbin; ibin++) {
   if (data["syst_p"][ibin]!=vsysttotp.at(ibin)) {
    std::cout<<cn<<mn<<"INFO: bin= "<<ibin<<"up old data[syst_p] "<<data["syst_p"][ibin]<<" new systtotp "<<vsysttotp.at(ibin)<<std::endl;
   }
  }

  for (int ibin=0; ibin<nbin; ibin++) { 
   if (data["syst_n"][ibin]!=vsysttotn.at(ibin)) {
    std::cout<<cn<<mn<<ibin<<"INFO: down old data[syst_n] "<<data["syst_n"][ibin]<<" new systtotn "<<vsysttotn.at(ibin)<<std::endl;
   }
  }
 }

 data["syst_p"]=vsysttotp;
 data["syst_n"]=vsysttotn;

 this->CalculateSystematicCovarianceMatrix();

 return;
};


void SPXData::PrintMap(StringDoubleVectorMap_T &m) {
 std::string mn = "PrintMap: ";
 SPXUtilities::PrintMethodHeader(cn, mn);

 std::cout<<cn<<mn<<"Map size= "<<m.size()<<std::endl;

 for(StringDoubleVectorMap_T::const_iterator it = m.begin(); it != m.end(); ++it) {
  std::cout<<" "<<it->first<<": ";
  for (int i=0; i<it->second.size();i++){
    std::cout << std::scientific << it->second.at(i) << " ";
  }
  std::cout<<std::endl;
 }
 return;

}

void SPXData::Draw(std::string opt){
 std::string mn = "Draw: ";
 SPXUtilities::PrintMethodHeader(cn, mn);

 this->totalErrorGraph->Draw(opt.c_str());

};
<<<<<<< HEAD
=======

bool SPXData::CheckCovarianceMatrix(double reltol){
 std::string mn = "CheckCovarianceMatrix: ";
 SPXUtilities::PrintMethodHeader(cn, mn);

 bool ok=true;

 TMatrixD *covdata=this->GetDataStatCovarianceMatrix();
 if (!covdata) {
  std::cout<<cn<<mn<<"INFO: Data covariance matrix can not be retrieved !"<<std::endl;
 }

 ok=this->CheckCovarianceMatrix(covdata, reltol);
 if (!ok) {
  std::cout<<cn<<mn<<"WARNING: Data covariance matrix "<<covdata->GetName()<<" is not ok!"<<std::endl;
 } 

 return ok;
}

bool SPXData::CheckCovarianceMatrix(TMatrixD *covdata, double reltol){
 std::string mn = "CheckCovarianceMatrix:TMatrixD: ";
 SPXUtilities::PrintMethodHeader(cn, mn);

 bool ok=true;

 if (!covdata) {
  throw SPXGeneralException(cn+mn+"Input covariance matrix not found !");
 }

 TGraphAsymmErrors * gdata=this->GetStatisticalErrorGraph();
 if (!gdata) {
  throw SPXGeneralException(cn+mn+"Data statistical graph not found !");
 }

 int nbin=gdata->GetN();

 if (nbin!=covdata->GetNrows() || nbin!=covdata->GetNcols()) {
  std::ostringstream oss;
  oss << "Number of bins nbin= "<<nbin<<" and cov matrix not ok ! matrix: nrows= "<< covdata->GetNrows() <<" ncol= "<< covdata->GetNcols() << std::endl;
  throw SPXGeneralException(oss.str());
 }

 for (int i=0; i<nbin; i++) {
  double estat=gdata->GetErrorYhigh(i);
  estat=(gdata->GetErrorYhigh(i)+gdata->GetErrorYlow(i))/2.;
    
  double valcov=(*covdata)[i][i];

  //if (debug) std::cout<<cn<<mn<<i<<" covdata= "<<valcov<<" stat err= "<<estat<<std::endl;
  
  double diff=fabs(valcov-estat*estat);
  if (estat!=0.) diff/=estat*estat;
  if (diff>reltol) {
   ok=false;
   std::cout<<cn<<mn<<"WARNING: Bin= "<<i<<" covariance matrix and data stat error are not the same within "<<std::fixed<<reltol<<std::scientific<<" : cov= "<<sqrt(valcov)<<" stat err= "<<estat<<" rel difference= "<<diff<<" ratio= "<< valcov/(estat*estat)<<std::endl;;
   //std::cout<<std::defaultfloat <<std::endl;

  } else {
   if (debug) std::cout<<cn<<mn<<"Bin i= "<<i<<" digaonal covariance matrix elements and data stat error consistent within= "<<diff<<std::endl;
   if (diff>0.02 && reltol>0.02) {
    std::ostringstream oss;
    oss <<cn<<mn<< std::scientific <<"WARNING: Bin= "<<i
	    <<" diagonal covariance matrix elements and data stat error are not the same within 2%: cov= "<<sqrt(valcov)<<" stat err= "<<estat<<" rel difference= "<<diff<<" ratio= "<< valcov/(estat*estat);
     std::cout<<oss.str()<<std::endl;
     std::cerr<<oss.str()<<std::endl;
    }
   }
  }

 return ok;
}
>>>>>>> release-0.98
