#!/bin/sh
echo "----------------- CLUCalc Build -------------------------"
echo " use the configure script and generate the libs and bins "
echo "---------------------------------------------------------"
if test $CLU_HOME; then 
  echo
  echo "->./configure --prefix=${CLU_HOME}"
  ./configure --prefix=${CLU_HOME}
  echo "->make"
  make
  echo "->make install"
  make install;
else
  echo
  echo "Your CLU_HOME environment variable is not set." 
  echo ;
fi

