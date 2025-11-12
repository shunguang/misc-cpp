# gst-ffmpeg-evaluation
 This project compares the mux/enc performance differences between ffmpeg and gstreamer.
 
 For convenience to write/read the following instructions, let's define
```
     APP_ROOT=~\gst-ffmpeg-evaluation
```
  
## Build
  
 1. Edit the env variables defined in 
 ```
$(APP_ROOT)/src/makeFiles/Makefile_app_header.mak
```
to fit your machine

 2. Go to folder 
```
 $(APP_ROOT)/src/makeFiles
```
 then run,
```
    $make -f Makefile_util.mak
    $make -f Makefile_test.mak
```
    
	The following folders and relative files (inside them) will be created by make.
```
 	$(APP_ROOT)/build
 	$(APP_ROOT)/build/libs
 	$(APP_ROOT)/build/libUtil
 	$(APP_ROOT)/build/test
 	$(APP_ROOT)/build/bin
```

## Test Data
   Copy *.bin files from 
```
   ~\Box\MAPP shared files\MAPP-Edge\gst-ffmpeg-evaluation\dataset
```
   to 
```
   $(APP_ROOT)/build/bin
```

## Run
 1. working dir: $(APP_ROOT)/build/bin
 2. $cp ../../src/test/cfg/cfg_gst_ffmpeg.xml .
 3. $vi cfg_gst_ffmpeg.xml  (edit cfg file)
 4. $./test.out 
```
   ./test.out <flag> <cfgFileName>;
   examples:;
   ./test.out 1 cfgfile --- test_ffmpeg_demux_ts()
   ./test.out 2 cfgfile --- test_ffmpeg_mux_ts()
   ./test.out 3 cfgfile --- test_gst_mux_enc_v1()
   ./test.out 4 cfgfile --- test_gst_mux_enc_v2()
   ./test.out 5 cfgfile --- est_gst_demux_dec_v1()
   ./test.out 6 cfgfile --- est_gst_demux_dec_v2()
   ./test.out 7 cfgFile --- test_gst_enc_H264()
   ./test.out 8 -- test_small_util()
```

## License
W/A

