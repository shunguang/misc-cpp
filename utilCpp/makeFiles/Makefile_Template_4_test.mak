-include Makefile.inc

PROJ_NAME=test

include Makefile_app_header.mak


#the target binary name
TARGETFILE=$(ODIR_BIN)/test.out

#include flags


#-L$(SD_LIB) -L$(ROS_LIB)
#link flags and lib searching paths
LFLAGS	:= -Wall $(DEBUG) -L$(ODIR_LIB) -L$(FDNN_LIB) -L$(CV_LIB) -L$(BOOST_LIB) -L/usr/lib/aarch64-linux-gnu/tegra -L/usr/lib/aarch64-linux-gnu -L$(PLTF_LIB) 


#-L/usr/lib/aarch64-linux-gnu/tegra
# -lcv_bridge -lfast-dnn -lfdnn-vncxx  -lfmt  -limage_geometry \

# link libs
LIBS	:= -lRun -lEkf2 -lFdnn -lvncxx -lCap -lUtil \
	-lcv_bridge -lfast-dnn -lfmt -limage_geometry \
	-lboost_timer -lboost_filesystem -lboost_system -lboost_date_time -lboost_regex \
	-lboost_chrono -lboost_thread -pthread \
	-lopencv_stitching -lopencv_superres -lopencv_videostab \
	-lopencv_ml -lopencv_shape -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_xfeatures2d \
	-lopencv_highgui -lopencv_videoio -lopencv_flann -lopencv_imgcodecs -lopencv_imgproc -lopencv_core \
	-lv4l2 \
	-ldl -lm -lpthread -lrt 

#	-lroslib -lroscpp \
#	-lv4l2 -lEGL -lGLESv2 -lX11 \=
#	-lgthread-2.0 -lgstrtspserver-1.0 -lgstbase-1.0 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstapp-1.0 -lglib-2.0 -lpng -lz -lv4l2 \
#	-lavcodec -lavformat -lavutil -lswresample -lswscale -llzma -ldl -lm -lpthread -lrt 


OBJS = \
	$(ODIR_OBJ)/test_pnt_realtime.o \
	$(ODIR_OBJ)/test_main.o 

#	$(ODIR_OBJ)/test_px4_ekf.o \

default:  directories $(TARGETFILE)

directories:    
	mkdir -p $(ODIR_OBJ)
	mkdir -p $(ODIR_LIB)
	mkdir -p $(ODIR_BIN)
	
#the output binary file name is <$(TARGETFILE)>
$(TARGETFILE)	:	$(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) $(LIBS) -o $(TARGETFILE)


$(ODIR_OBJ)/test_main.o	:	$(SDIR_PROJ)/test_main.cpp
	$(CXX) -o $(ODIR_OBJ)/test_main.o $(CFLAGS_EXE) $(SDIR_PROJ)/test_main.cpp

$(ODIR_OBJ)/test_px4_ekf.o	:	$(SDIR_PROJ)/test_px4_ekf.cpp
	$(CXX) -o $(ODIR_OBJ)/test_px4_ekf.o $(CFLAGS_EXE) $(SDIR_PROJ)/test_px4_ekf.cpp

$(ODIR_OBJ)/test_pnt_realtime.o	:	$(SDIR_PROJ)/test_pnt_realtime.cpp
	$(CXX) -o $(ODIR_OBJ)/test_pnt_realtime.o $(CFLAGS_EXE) $(SDIR_PROJ)/test_pnt_realtime.cpp

clean:
	\rm $(ODIR_OBJ)/*.o $(TARGETFILE)

rm_pnt:
	\rm $(TARGETFILE)
            
