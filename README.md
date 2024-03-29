# LearnGenJets
Repository for simple PYTHIA8 and FastJet examples

The repository requires PYTHIA8 and FastJet software packages. You can find PYTHIA8 (as of date of creation, PYTHIA 8.240) at

http://home.thep.lu.se/~torbjorn/Pythia.html

and FastJet (as of creation date FastJet 3.3.2) at

http://fastjet.fr/

You will also want to grab the fastjet contrib, version 1.041 as of creation.

In addition to PYTHIA8 and FastJet, this code will use ROOT6 classes for storage. Source code and instructions for installation are beyond this repo, but you can look here if installation is necessary: https://root.cern.ch/downloading-root

If you dont intend to install to your laptop then there are public builds available on lxplus here:

/afs/cern.ch/work/c/cmcginn/public/Packages/ForLearning/

Can be used on lxplus or on svmithi02

**** PYTHIA INSTALL ****

Grab tar with

curl -O http://home.thep.lu.se/~torbjorn/pythia8/pythia8240.tgz

With pythia8240.tgz in appropriate subdirectory (recommend keeping packages for physics in single well defined directory) do

tar -xzvf pythia8240.tgz #or whatever the version name is
cd pythia8240
./configure --prefix=/PATH/TO/TAR/pythia8-build --arch=<SEE README>

where you should give the path you run the tar command from and for arch select appropriate to your OS (I would pick "Linux"). With configure complete do:

make -j4
make install

Finally with install complete go ahead and add the pythia build directory to your path

export PATH=$PATH:/PATH/TO/TAR/pythia8-build/bin

Recommend adding above to your .bash_profile or .bashrc file so it is done w/ each fresh terminal session.

On svmithi02 or lxplus do

export PATH=$PATH:/afs/cern.ch/work/c/cmcginn/public/Packages/ForLearning/PYTHIA8/pythia8-build/bin/


**** FastJet INSTALL ****

Get FastJet and contrib with

curl -O http://fastjet.fr/repo/fastjet-3.3.2.tar.gz
wget http://fastjet.hepforge.org/contrib/downloads/fjcontrib-1.041.tar.gz

With fastjet-3.3.2.tar.gz in appropriate subdirectory (recommend keeping packages for physics in single well defined directory) do

tar -xzvf fastjet-3.3.2.tar.gz
cd fastjet-3.3.2
./configure --prefix=/PATH/TO/TAR/fastjet-install
make
make check
make install

As soon as install is complete, go right into contrib installation

cd ../
tar -xzvf fjcontrib-1.041.tar.gz
cd fjcontrib-1.041
./configure --fastjet-config=/PATH/TO/TAR/fastjet-install/bin/fastjet-config
make
make check
make install

Finally with install complete go ahead and add the fastjet build directory to your path

export PATH=$PATH:/PATH/TO/TAR/fastjet-install/bin

if on svmithi02 or lxplus do

export PATH=$PATH:/afs/cern.ch/work/c/cmcginn/public/Packages/ForLearning/FastJet/fastjet-install/bin/

**** Running this repository ****

For the convenience of the Makefile, start by editing setLearnGenJets.sh for the paths appropriate to your system. Doing this explicitly makes potential version conflicts easier to handle. Once you have added the full set of paths appropriate to your system, do

source setLearnGenJets.sh

This will set those environmental variables for your session. If they are not set, running make will fail with request you run source.

with all paths set properly, run

make

if build is successful, go ahead and run

./bin/simplePYTHIA.exe

if this fails, check the global observable PYTHIA8DATA; if that is mismatched just set it to empty

This will create a file w/ ttree containing particle content of pythia8 pp events at sqrts = 5.02 TeV, a very simple implementation of LHC like conditions with bias to hard scatterings of transverse energy of 80 GeV

the file can be found under output/TODAYSDATE

If successful, we can then run jet clustering:

./bin/simpleFastjet.exe output/TODAYSDATE/PYTHIAFILENAME

or substitute your own pythia file

this will create an output in output/TODAYSDATE with a jet TTree

For studying the splitting function, we can switch to sipleFastjetWithZg.exe, which adds components of fastjet code from the contrib,

./bin/simpleFastjet.exe output/TODAYSDATE/PYTHIAFILENAME

this will create an output in output/TODAYSDATE with a jet TTree that also includes softdropped jets and splitting function observables for beta = 0 and zcut = 0.1

finally we can run a plotter and look at our results with

./bin/plotZG.exe <FASTJETFILEWITHZG>

this will produce plots under pdfDir/TODAYSDATE, which you can dump into beamer .tex slides with

bash bashDir/allDirToTex.sh pdfDir/TODAYSDATE 1

and then look at in the corresponding locally created pdf of slides.
