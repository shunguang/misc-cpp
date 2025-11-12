#this Makefile outputs image as a static lib for <PLTF> machine

PROJ_NAME=libUtil
include Makefile_app_header.mak

OBJS = \
	$(ODIR_OBJ)/AppLog.o \
	$(ODIR_OBJ)/FileUtil.o \
	$(ODIR_OBJ)/UtilFuncs.o
	
default:  directories $(TARGETFILE)

directories:    
	mkdir -p $(ODIR_OBJ)
	mkdir -p $(ODIR_LIB)

#the output lib file name is <$(TARGETFILE)>
$(TARGETFILE) : $(OBJS)
	ar rcs $(TARGETFILE) $(OBJS)

$(ODIR_OBJ)/AppLog.o: $(SDIR_PROJ)/AppLog.cpp $(SDIR_PROJ)/AppLog.h
	$(CXX) -o $(ODIR_OBJ)/AppLog.o $(CFLAGS) $(SDIR_PROJ)/AppLog.cpp

$(ODIR_OBJ)/FileUtil.o: $(SDIR_PROJ)/FileUtil.cpp $(SDIR_PROJ)/FileUtil.h
	$(CXX) -o $(ODIR_OBJ)/FileUtil.o $(CFLAGS) $(SDIR_PROJ)/FileUtil.cpp

$(ODIR_OBJ)/UtilFuncs.o: $(SDIR_PROJ)/UtilFuncs.cpp $(SDIR_PROJ)/UtilFuncs.h
	$(CXX) -o $(ODIR_OBJ)/UtilFuncs.o $(CFLAGS) $(SDIR_PROJ)/UtilFuncs.cpp

clean:
	\rm -r $(ODIR_OBJ)/*.o $(TARGETFILE)

