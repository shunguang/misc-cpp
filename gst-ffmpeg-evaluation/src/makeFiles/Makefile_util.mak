#this Makefile outputs image as a static lib for <PLTF> machine

PROJ_NAME=libUtil
include Makefile_app_header.mak

OBJS = \
	$(ODIR_OBJ)/AppLog.o \
	$(ODIR_OBJ)/AppUtil.o \
	$(ODIR_OBJ)/Cfg.o \
	$(ODIR_OBJ)/Uint8Stream.o \
	$(ODIR_OBJ)/HostKlvFrm.o \
	$(ODIR_OBJ)/MuxEncUserData.o \
	$(ODIR_OBJ)/MuxEncUserData2.o \
	$(ODIR_OBJ)/GstMyAppMeta.o \
	$(ODIR_OBJ)/HostYuvFrm.o
	
default:  directories $(TARGETFILE)

directories:    
	mkdir -p $(ODIR_ROOT)
	mkdir -p $(ODIR_OBJ)
	mkdir -p $(ODIR_LIB)


#the output lib file name is <$(TARGETFILE)>
$(TARGETFILE) : $(OBJS)
	ar rcs $(TARGETFILE) $(OBJS)

$(ODIR_OBJ)/AppLog.o: $(SDIR_PROJ)/AppLog.cpp $(SDIR_PROJ)/AppLog.h
	$(CXX) -o $(ODIR_OBJ)/AppLog.o $(CFLAGS) $(SDIR_PROJ)/AppLog.cpp

$(ODIR_OBJ)/Cfg.o: $(SDIR_PROJ)/Cfg.cpp $(SDIR_PROJ)/Cfg.h
	$(CXX) -o $(ODIR_OBJ)/Cfg.o $(CFLAGS) $(SDIR_PROJ)/Cfg.cpp

$(ODIR_OBJ)/Uint8Stream.o: $(SDIR_PROJ)/Uint8Stream.cpp $(SDIR_PROJ)/Uint8Stream.h
	$(CXX) -o $(ODIR_OBJ)/Uint8Stream.o $(CFLAGS) $(SDIR_PROJ)/Uint8Stream.cpp

$(ODIR_OBJ)/HostYuvFrm.o: $(SDIR_PROJ)/HostYuvFrm.cpp $(SDIR_PROJ)/HostYuvFrm.h
	$(CXX) -o $(ODIR_OBJ)/HostYuvFrm.o $(CFLAGS) $(SDIR_PROJ)/HostYuvFrm.cpp

$(ODIR_OBJ)/HostKlvFrm.o: $(SDIR_PROJ)/HostKlvFrm.cpp $(SDIR_PROJ)/HostKlvFrm.h
	$(CXX) -o $(ODIR_OBJ)/HostKlvFrm.o $(CFLAGS) $(SDIR_PROJ)/HostKlvFrm.cpp

$(ODIR_OBJ)/MuxEncUserData.o: $(SDIR_PROJ)/MuxEncUserData.cpp $(SDIR_PROJ)/MuxEncUserData.h
	$(CXX) -o $(ODIR_OBJ)/MuxEncUserData.o $(CFLAGS) $(SDIR_PROJ)/MuxEncUserData.cpp

$(ODIR_OBJ)/MuxEncUserData2.o: $(SDIR_PROJ)/MuxEncUserData2.cpp $(SDIR_PROJ)/MuxEncUserData2.h
	$(CXX) -o $(ODIR_OBJ)/MuxEncUserData2.o $(CFLAGS) $(SDIR_PROJ)/MuxEncUserData2.cpp

$(ODIR_OBJ)/GstMyAppMeta.o: $(SDIR_PROJ)/GstMyAppMeta.cpp $(SDIR_PROJ)/GstMyAppMeta.h
	$(CXX) -o $(ODIR_OBJ)/GstMyAppMeta.o $(CFLAGS) $(SDIR_PROJ)/GstMyAppMeta.cpp

$(ODIR_OBJ)/AppUtil.o: $(SDIR_PROJ)/AppUtil.cpp $(SDIR_PROJ)/AppUtil.h
	$(CXX) -o $(ODIR_OBJ)/AppUtil.o $(CFLAGS) $(SDIR_PROJ)/AppUtil.cpp

clean:
	\rm -r $(ODIR_OBJ)/*.o $(TARGETFILE)

