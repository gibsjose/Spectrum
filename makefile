###############################################################
#															  #
#					Makefile for Spectrum 	  				  #
#															  #
###############################################################

CXX = g++
CXXFLAGS += -g -O3

#ROOT
ROOTINCS = $(shell root-config --cflags) 
ROOTLIBS = $(shell root-config --glibs) 
ROOTARCH = $(findstring -m64, $(ROOTINCS))

CXXFLAGS += $(ROOTARCH) $(ROOTINCS)

SRC_DIR = ./src
BIN_DIR = .
TST_DIR = $(SRC_DIR)/test

SRC = $(SRC_DIR)/Spectrum.cxx $(SRC_DIR)/SPXSteeringFile.cxx $(SRC_DIR)/SPXRatioStyle.cxx $(SRC_DIR)/SPXDisplayStyle.cxx \
		$(SRC_DIR)/SPXOverlayStyle.cxx $(SRC_DIR)/SPXPDFBandType.cxx $(SRC_DIR)/SPXPDFErrorType.cxx $(SRC_DIR)/SPXPDFErrorSize.cxx \
		$(SRC_DIR)/SPXPlotConfiguration.cxx $(SRC_DIR)/SPXPDFSteeringFile.cxx $(SRC_DIR)/SPXGridSteeringFile.cxx $(SRC_DIR)/SPXDataSteeringFile.cxx \
		$(SRC_DIR)/SPXDataFormat.cxx $(SRC_DIR)/SPXData.cxx
HDR = $(SRC_DIR)/*.h
INC = -I./inih/include -I$(SRC_DIR)
LIB_PATH = -L./inih/lib
LIB = -linih $(ROOTLIBS)
BIN = $(BIN_DIR)/Spectrum

.SUFFIXES: .cxx .o

.PHONY: clean test

# all: $(BIN) test

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(INC) $(SRC) $(LIB_PATH) $(LIB)

test: $(TST_DIR)/TestSPXRatioStyle $(TST_DIR)/TestSPXDisplayStyle $(TST_DIR)/TestSPXOverlayStyle

$(TST_DIR)/TestSPXRatioStyle: $(SRC_DIR)/SPXRatioStyle.cxx $(TST_DIR)/TestSPXRatioStyle.cxx
	$(CXX) $(CXXFLAGS) -o $@ $(INC) $^ $(LIB_PATH) $(LIB)

$(TST_DIR)/TestSPXDisplayStyle: $(SRC_DIR)/SPXDisplayStyle.cxx $(TST_DIR)/TestSPXDisplayStyle.cxx
	$(CXX) $(CXXFLAGS) -o $@ $(INC) $^ $(LIB_PATH) $(LIB)

$(TST_DIR)/TestSPXOverlayStyle: $(SRC_DIR)/SPXOverlayStyle.cxx $(TST_DIR)/TestSPXOverlayStyle.cxx
	$(CXX) $(CXXFLAGS) -o $@ $(INC) $^ $(LIB_PATH) $(LIB)

clean:
	rm -f $(BIN) $(TST) 
