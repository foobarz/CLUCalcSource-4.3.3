#!/bin/sh
echo "-------------------- CLUfltk Build -------------------------"
echo " Uses the configure script and generates the libraries "
echo "------------------------------------------------------------"
if test $FLTK_HOME; then 
  echo
  echo "->./configure --prefix=${FLTK_HOME}"
  ./configure --prefix=${FLTK_HOME}
  echo "->make install"
  make install;
  echo "->make"
  make
  echo
  echo
else
  echo
  echo "Your FLTK_HOME environment variable is not set."
  echo "Set it to the directory where you want CLUfltk"
  echo "to be installed." 
  echo
fi

