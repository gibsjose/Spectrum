//************************************************************/
//
//	Plot Type Header
//
//	Outlines the SPXPlotType class, which defines the four
//	supported plot types:
//
//		1)	data, grid, pdf
//
//			ONE data file
//			ONE grid file
//			ONE pdf file
//
//		2)	data[], grid[], pdf
//
//			MANY data files
//			MANY grid files (1:1 w/ data files)
//			ONE pdf file
//
//		3) 	data, grid, pdf[]
//
//			ONE data file
//			ONE grid file
//			MANY pdf files
//
//		4)	data, grid[], pdf
//
//			ONE data file
//			MANY grid files
//			ONE pdf file
//                      
//              5)      contains data[] (data will be type1)
//                      (grid and pdf does not matter)
// 
//                      for data only
//                       
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN 
//	@Date:		27.10.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#ifndef SPXPLOTTYPE_H
#define SPXPLOTTYPE_H

#include <string>

//Bitfields for determining style type
const int PT_INVALID =		-1;
const int PT_DATA = 		(1 << 0);	//0b00000001
const int PT_GRID = 		(1 << 1);	//0b00000010
const int PT_PDF = 		(1 << 2);	//0b00000100

//A '0' represents a SINGLE instance
//A '1' represents MULTIPLE instances
//i.e. data[], grid[], pdf will have the type:
//			0b00000011  =  3
//		   		   ^^^
//		   		   |||
// ONE PDF -------/ ||
// MULT Grids -----/ |
// MULT Data -------/

class SPXPlotType {

public:
	SPXPlotType () : type(0) {}
	explicit SPXPlotType(std::string s);

	void Parse(std::string s);
	void Print(void);
	std::string ToString(void);
	bool IsValid(void);

	void Clear(void) {
		type = 0;
	}

	static bool GetDebug(void) {
		return debug;
	}

	static void SetDebug(bool b) {
		debug = b;
	}

	int GetType(void) {
		return (int)type;
	}

	//Type 1 = Single Data, Single Grid, Single PDF
	bool IsType1(void) {
		if(ContainsSingleData() && ContainsSingleGrid() && ContainsSinglePDF()) {
	                if (debug) std::cout<<"SPXType IsType1 TRUE"<<std::endl;
			return true;
		}

	        if (debug) std::cout<<"SPXType IsType1 FALSE"<<std::endl;
		return false;
	}

	bool IsType2(void) {
		if(ContainsMultipleData() && ContainsMultipleGrids() && ContainsSinglePDF()) {
	                if (debug) std::cout<<"SPXType IsType2 TRUE"<<std::endl;
			return true;
		}
	        if (debug) std::cout<<"SPXType IsType2 FALSE"<<std::endl;
		return false;
	}

	bool IsType3(void) {
		if(ContainsSingleData() && ContainsSingleGrid() && ContainsMultiplePDFs()) {
	                if (debug) std::cout<<"SPXType IsType3 TRUE"<<std::endl;
			return true;
		}

	        if (debug) std::cout<<"SPXType IsType3 FALSE"<<std::endl;
		return false;
	}

	bool IsType4(void) {
		if(ContainsSingleData() && ContainsMultipleGrids() && ContainsSinglePDF()) {
	                if (debug) std::cout<<"SPXType IsType4 TRUE"<<std::endl;
			return true;
		}
	        if (debug) std::cout<<"SPXType IsType4 FALSE"<<std::endl;
		return false;
	}

	bool IsType5(void) {
          if (debug) {
	   if (ContainsSingleData()   ) std::cout<<"SPXType ContainsSingleData() TRUE"<<std::endl;
	   if (ContainsMultipleData() ) std::cout<<"SPXType ContainsMultipleData() TRUE"<<std::endl;
	   if (ContainsMultipleGrids()) std::cout<<"SPXType ContainsMultipleGrids() TRUE"<<std::endl;
	   if (ContainsMultiplePDFs() ) std::cout<<"SPXType ContainsMultiplePDFs() TRUE"<<std::endl;
	   if (ContainsSingleGrid()   ) std::cout<<"SPXType ContainsSingleGrid()  TRUE"<<std::endl;
	   if (ContainsSinglePDF()    ) std::cout<<"SPXType ContainsSinglePDF() ) TRUE"<<std::endl;
          }
	  if(ContainsMultipleData() && ContainsSingleGrid() && ContainsSinglePDF()) {
	    // if( (ContainsSingleData() || ContainsMultipleData() ) 
	    // && !(ContainsMultipleGrids() || ContainsMultiplePDFs() ||  
            // ContainsSingleGrid()    || ContainsSinglePDF() )
            //) {
	                if (debug) std::cout<<"SPXType IsType5 TRUE"<<std::endl;
			return true;
	      }

	        if (debug) std::cout<<"SPXType IsType5 FALSE"<<std::endl;
		return false;
	}

	bool ContainsSingleData(void) {
		return ContainsSingle(PT_DATA);
	}

	bool ContainsSingleGrid(void) {
		return ContainsSingle(PT_GRID);
	}

	bool ContainsSinglePDF(void) {
		return ContainsSingle(PT_PDF);
	}

	bool ContainsMultipleData(void) {
		return ContainsMultiple(PT_DATA);
	}

	bool ContainsMultipleGrids(void) {
		return ContainsMultiple(PT_GRID);
	}

	bool ContainsMultiplePDFs(void) {
		return ContainsMultiple(PT_PDF);
	}

	bool ContainsSingle(int mask) {
		return !ContainsMultiple(mask);
	}

	bool ContainsMultiple(int mask) {
		if(type & mask) {
			return true;
		}

		return false;
	}

private:
	static bool debug;
	unsigned int type;
};

#endif
