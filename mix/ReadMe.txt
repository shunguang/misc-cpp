about this app
 mixing baseband signal with carrier signal from both time-domain and freq-domain algorithms

1. Assume you unzip mix.zip into 
   MIX = C:\temp\mix
   
2. Here are the files and folders
   $MIX/ReadMe.txt -- thsi readme file
   $MIX/data       -- some dample data files which can be plot by matlab script
   $MIX/src/cpp    -- cpp src file folder
   $MIX/src/matlab -- matlab script file folder
   $MIX/src/vs2019 -- vs2019 solution, prop, and project files
   
3. build in VS2019
   a) go inot folder: $MIX/src/vs2019
   b) copy open_app_vs2019_template.bat into my_open_app_vs2019.bat
   c) edit my_open_app_vs2019.bat for your environment
   d) double click my_open_app_vs2019.bat
   e) by default the building results will be in 
      $MIX/build-vs2019-x64
	  
   Note: only x64 debug is tested, you may need to set porjetc "test" as a startup project

4. build in linux
   todo:
   
5. using Matlab plot and analizing the signals
   $MIX/src/matlab/main_plotSignals.m     -- main function starting from here
   $MIX/src/matlab/ReadSignal.m           -- load the txt files dumped in C++
   $MIX/src/matlab/plotSignal.m           -- plot signal in both freq and time domain  
   
-------------eof ----------
   
   
   
   
   
   
