-include Makefile.inc

#----------------------------------------------------------
# env variables need to defined in /home/nvidia/.bashrc 
#
# export APP_ROOT=/home/nvidia/suas_wami
# export XIMEA_INC=/opt/XIMEA/include
# export XIMEA_LIB=/usr/lib
# export CUDA_INC=/usr/local/cuda-10.0/targets/aarch64-linux/include
# export CUDA_LIB=/usr/local/cuda-10.0/targets/aarch64-linux/lib
#
#build intermediat output paths
APP_ROOT=/media/winshare/app

SDIR_ROOT=$(APP_ROOT)/src
SDIR_PROJ=$(APP_ROOT)/src/$(PROJ_NAME)

ODIR_ROOT=/home/debian/app/build
ODIR_OBJ=$(ODIR_ROOT)/$(PROJ_NAME)
ODIR_LIB=$(ODIR_ROOT)/libs
ODIR_BIN=$(ODIR_ROOT)/bin


#include and lib paths of the platform
PLTF_INC=/usr/include
PLTF_LIB=/usr/lib
BOOST_INC=/usr/include
BOOST_LIB=/usr/lib

CV_INC=/opt/kmt/include/opencv4
CV_LIB=/opt/kmt/lib

ROS_INC=/opt/ros/melodic/include
ROS_LIB=/opt/ros/melodic/lib

FDNN_INC=/opt/sd_jhuapl/install/include
FDNN_LIB=/opt/sd_jhuapl/install/lib

EIG3_INC=/opt/pkg/include/eigen3
EIG3_LIB=/opt/pkg/lib


#GST_INC=-I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include
#GST_LIB=/usr/lib/aarch64-linux-gnu/gstreamer-1.0

CC = /usr/bin/gcc
CXX = /usr/bin/g++

#DEBUG = -g
DEBUG = -DNDEBUG -g
#DEBUG = -DDEBUG -g

#include flags for building libs
CFLAGS = -Wall -static -c $(DEBUG) -DECL_STANDALONE=1 -DqDNGDebug=1 -D__xlC__=1 -DNO_FCGI_DEFINES=1 -DqDNGUseStdInt=0 -DUNIX_ENV=1 -D__LITTLE_ENDIAN__=1 -DqMacOS=0 -DqWinOS=0 -std=gnu++14 \
	-I$(SDIR_PROJ) -I$(SDIR_ROOT) $(GST_INC) -I$(EIG3_INC) -I$(CV_INC) -I$(BOOST_INC) -I$(PLTF_INC)

#include flags for building executable
CFLAGS_EXE	:= -Wall -c $(DEBUG) -DECL_STANDALONE=1 -DqDNGDebug=1 -D__xlC__=1 -DNO_FCGI_DEFINES=1 -DqDNGUseStdInt=0 -DUNIX_ENV=1 -D__LITTLE_ENDIAN__=1 -DqMacOS=0 -DqWinOS=0 -std=gnu++14 \
	-I$(SDIR_PROJ) -I$(SDIR_ROOT) -I$(EIG3_INC) -I$(CV_INC) -I$(BOOST_INC) -I$(PLTF_INC)

TARGETFILE=$(ODIR_LIB)/$(PROJ_NAME).a

