Assuming you work in the home directory
$ cd ~


1) Download and install Omnett++ (4.1 or higher?)
   A) Untar the omnet++ tarball and enter the resulting directory:
      $ tar -xvzf omnetpp-4.1-src.tgz
      $ cd omnetpp-4.1
   B) Run configure
      $ ./configure 
   C) Fix the things the come out of the configure (including the 'export' to be added to the .bashrc)
      ...
   D) Compile with make (if you have a multicore machine, use the option -j: 'make -j NUM_CORES + 1')
      $ make 
      

2) Download and install opencv 
   It depends on the Linux distribution you are using.
   For Debian (and Ubuntu) you can use apt-get (TODO: explain how) ...
   For other distributions you can use the opencv source code from ...

2bis) Install MTL library

3) Install MY simulator (assuming I am distributing the package with Castalia, is it legal?)
   A) ...
      $ tar -xvzf WISE_Castalia-3.0.tar.gz
      $ cd WISE_Castalia-3.0
4) Use MY simulator
   A) ...
      $ ./makemake
   B) ... (-j ...)
      $ make all
   C) Run IT
      
