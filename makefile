###############################################################
#															  #
#					Makefile for Spectrum 	  				  #
#															  #
###############################################################

CXX = g++

STD = $(shell echo $(CXX_STD))

CXXFLAGS += -g -O3 $(STD)

#ROOT
ROOTINCS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --glibs)
ROOTARCH = $(findstring -m64, $(ROOTINCS))

#APPLGrid
APPLCXXFLAGS = $(shell applgrid-config --cxxflags)
APPLCLIBS    = $(shell applgrid-config --ldcflags)
APPLFLIBS    = $(shell applgrid-config --ldflags)

#LHAPDF
LHAPDFINCS = -I$(shell lhapdf-config --prefix)/include
LHAPDFDIR  = $(shell lhapdf-config --prefix)/lib
LHAPDFLIBS = -L$(LHAPDFDIR) -lLHAPDF

CXXFLAGS += $(ROOTARCH) $(ROOTINCS) $(APPLCXXFLAGS) $(LHAPDFINCS)

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = .
TST_DIR = $(SRC_DIR)/test

RAW_SRC = 	Spectrum.cxx SPXSteeringFile.cxx SPXRatioStyle.cxx SPXDisplayStyle.cxx SPXOverlayStyle.cxx SPXPDFBandType.cxx \
			SPXPDFErrorType.cxx SPXPDFErrorSize.cxx SPXPlotConfiguration.cxx SPXPDFSteeringFile.cxx SPXGridSteeringFile.cxx \
			SPXDataSteeringFile.cxx SPXDataFormat.cxx SPXData.cxx SPXPlot.cxx SPXCrossSection.cxx SPXGrid.cxx SPXPDF.cxx \
			SPXRatio.cxx SPXPlotType.cxx SPXAtlasStyle.cxx

SRC = $(RAW_SRC:%.cxx=$(SRC_DIR)/%.cxx)
OBJ = $(RAW_SRC:%.cxx=$(OBJ_DIR)/%.o)
HDR = $(SRC_DIR)/*.h
INC = -I./inih/include -I$(SRC_DIR)
LIB_PATH = -L./inih/lib
LIB = -linih $(ROOTLIBS) $(APPLCLIBS) $(APPLFLIBS) $(LHAPDFLIBS)
BIN = $(BIN_DIR)/Spectrum

.SUFFIXES: .cxx .o .h

.PHONY: all dir clean

all: dir $(BIN)

dir:
	@mkdir -p $(SRC_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(TST_DIR)

$(BIN): $(OBJ) $(HDR)
	@echo
	@echo "============================"
	@echo " Building $(BIN) Executable "
	@echo "============================"
	@echo
	mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) -o $(BIN) $(INC) $(OBJ) $(LIB_PATH) $(LIB)
	@echo
	@echo " ---> Done"

$(OBJ_DIR)/%.o: $(SRC_DIR)%.cxx
	@echo
	@echo "Building $<"
	@$(CXX) $(CXXFLAGS) -o $@ $(INC) $< $(LIB_PATH) $(LIB)
	@echo " ---> Done"
	@echo
clean:
	rm -f $(BIN)
