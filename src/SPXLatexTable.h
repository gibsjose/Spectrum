//************************************************************/
//
// Dumps latex tables
//
//	@Author: 	T. Carli B Malaescu
//	@Date:		2.4.2015
//	@Email:		
//
//************************************************************/

#ifndef SPXLatexTable_H
#define SPXLatexTable_H

#include <vector>
#include <set>
#include <map>

#include "SPXSteeringFile.h"

#include "SPXCrossSection.h"
#include "SPXData.h"

#include "SPXException.h"

class SPXLatexTable {

public:

  SPXLatexTable(std::vector<SPXData*> data, std::vector<SPXCrossSection> cross, SPXSteeringFile *steeringFile);

  void DumpTable(void);

  static bool GetDebug(void) {
  return debug;
 }

 static void SetDebug(bool b) {
  debug = b;
  }

 //void SetPlotNumber(unsigned int &iplot) {
 // id = iplot;
 // }


private:
 static bool debug;					// Flag indicating debug mode
 SPXSteeringFile *steeringFile;				// Fully parsed steering file
 unsigned int id;					// Plot number ID (0-based)

 std::vector<SPXData*> data;				// Vector of data
 std::vector<SPXCrossSection> crossSections;		// Vector of cross sections

};

#endif
