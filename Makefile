CXX = g++
CXXFLAGS = -Wall -Werror -O2 -Wextra -Wno-unused-local-typedefs -Wno-deprecated-declarations -std=c++11 -g

ifeq "$(GCCVERSION)" "1"
  CXXFLAGS += -Wno-error=misleading-indentation
endif

ifndef LEARNGENJETSDIR
$(error LEARNGENJETSDIR is not set at all. Do 'source setLearnGenJets.sh')
endif

MKDIR_BIN=mkdir -p $(PWD)/bin
MKDIR_OUTPUT=mkdir -p $(PWD)/output
MKDIR_PDF=mkdir -p $(PWD)/pdfDir

INCLUDE=-I$(LEARNGENJETSDIR)
ROOT=`root-config --cflags --glibs`
PYTHIA8=-I$(PYTHIA8PATH)/include -O2 -pedantic -W -Wall -Wshadow -fPIC -L$(PYTHIA8PATH)/lib -Wl,-rpath,$(PYTHIA8PATH)/lib -lpythia8 -ldl
FASTJET=`fastjet-config --cxxflags --plugins --libs`
FJCONTRIB=-lRecursiveTools
FASTJETINCLUDE=-I$(FASTJETPATH)

all: mkdirBin mkdirOutput mkdirPdf bin/simplePYTHIA.exe bin/advancedPYTHIA.exe bin/simpleFastjet.exe bin/simpleFastjetWithZg.exe bin/plotZG.exe bin/removeDuplicateTNamed.exe

mkdirBin:
	$(MKDIR_BIN)

mkdirOutput:
	$(MKDIR_OUTPUT)

mkdirPdf:
	$(MKDIR_PDF)

bin/simplePYTHIA.exe: src/simplePYTHIA.C
	$(CXX) $(CXXFLAGS) src/simplePYTHIA.C $(ROOT) $(PYTHIA8) $(INCLUDE) -o bin/simplePYTHIA.exe

bin/advancedPYTHIA.exe: src/advancedPYTHIA.C
	$(CXX) $(CXXFLAGS) src/advancedPYTHIA.C $(ROOT) $(PYTHIA8) $(INCLUDE) -o bin/advancedPYTHIA.exe

bin/simpleFastjet.exe: src/simpleFastjet.C
	$(CXX) $(CXXFLAGS) src/simpleFastjet.C $(ROOT) $(FASTJET) $(INCLUDE) $(FASTJETINCLUDE) -o bin/simpleFastjet.exe

bin/simpleFastjetWithZg.exe: src/simpleFastjetWithZg.C
	$(CXX) $(CXXFLAGS) src/simpleFastjetWithZg.C $(ROOT) $(FJCONTRIB) $(FASTJET) $(INCLUDE) $(FASTJETINCLUDE) -o bin/simpleFastjetWithZg.exe

bin/plotZG.exe: src/plotZG.C
	$(CXX) $(CXXFLAGS) src/plotZG.C $(ROOT) $(INCLUDE) -o bin/plotZG.exe

bin/removeDuplicateTNamed.exe: src/removeDuplicateTNamed.C
	$(CXX) $(CXXFLAGS) src/removeDuplicateTNamed.C $(ROOT) $(INCLUDE) -o bin/removeDuplicateTNamed.exe

clean:
	rm -f ./*~
	rm -f ./#*#
	rm -f bin/*.exe
	rm -rf bin
	rm -f include/*~
	rm -f include/#*#
	rm -f src/*~
	rm -f src/#*#
