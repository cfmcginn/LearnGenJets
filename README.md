# LearnGenJets
Repository for simple PYTHIA8 and FastJet examples

The repository requires PYTHIA8 and FastJet software packages. You can find PYTHIA8 (as of date of creation, PYTHIA 8.240) at

http://home.thep.lu.se/~torbjorn/Pythia.html

and FastJet (as of creation date FastJet 3.3.2) at

http://fastjet.fr/

You will also want to grab the fastjet contrib, version 1.041 as of creation.

**** PYTHIA INSTALL ****

With pythia8240.tar.gz in appropriate subdirectory (recommend keeping packages for physics in single well defined directory) do

tar -xzvf pythia8240.tar.gz #or whatever the version name is
cd pythia8240
./configure --prefix=/PATH/TO/TAR/pythia8-build --arch=<SEE README>

where you should give the path you run the tar command from and for arch select appropriate to your OS (I would pick "Linux"). With configure complete do:

make -j4
make install

Finally with install complete go ahead and add the pythia build directory to your path

export PATH=$PATH:/PATH/TO/TAR/pythia8-build/bin

Recommend adding above to your .bash_profile or .bashrc file so it is done w/ each fresh terminal session.

**** FastJet INSTALL ****

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

**** Running this repository ****

