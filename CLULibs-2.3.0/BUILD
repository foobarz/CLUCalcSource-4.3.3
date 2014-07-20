#!/bin/sh
echo "----------------- CLUProject Build -------------------------"
echo " Uses the configure script and generates the libraries "
echo "------------------------------------------------------------"
if test $CLU_HOME; then 
  echo
  echo "->./configure --prefix=${CLU_HOME}"
  ./configure --prefix=${CLU_HOME}
  echo "->make"
  make
  echo "->make install"
  make install;
  echo
  echo
  echo "======================================================="
  echo "In order to use pkg-config with the CLUProject library,"
  echo "make sure to add the path ${CLU_HOME}/lib/pkgconfig "
  echo "to the PKG_CONFIG_PATH environment variable."
  echo "======================================================="
  echo
else
  echo
  echo "Your CLU_HOME environment variable is not set."
  echo "Set it to the directory where you want CLU to be installed." 
  echo
fi

