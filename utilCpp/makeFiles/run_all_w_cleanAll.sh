#!/bin/bash

#clean all
# make -f Makefile_vn_sdk.mak clean
# make -f Makefile_is_sdk2.mak clean
make -f Makefile_util.mak clean
make -f Makefile_mak.mak clean
make -f Makefile_test.mak  clean

#---------------------------------------


make -f Makefile_util.mak -j 3
make -f Makefile_mak.mak -j 3
make -f Makefile_test.mak  -j 3

#---eof---

