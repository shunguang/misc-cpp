classdef ReadSignal
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    properties
        timeDuration_sec=1;
        numSamplePts=100;
        numFftPts=100;
        samplingFreq_Hz=100;
        isComplex=1;
        freqRng_Hz=[10, 50];
        time_sec=[];
        xI=[];
        xQ=[];
    end

    methods
        function obj = readBaseband( obj, txtFilePath )
            %read header
            %todo: instead of hard coded, read header from txt file
            obj.timeDuration_sec=1;
            obj.numSamplePts=100;
            obj.numFftPts=100;
            obj.samplingFreq_Hz=100;
            obj.isComplex=1;
            obj.freqRng_Hz=[10, 50];

            %sampling time
            T = 1/obj.samplingFreq_Hz;
            obj.time_sec = (0 : (obj.numSamplePts-1))*T;

            %read xI and xQ
            nHeaderLines = 8;
            x = readmatrix(txtFilePath, "NumHeaderLines", nHeaderLines,"Delimiter",',');
            obj.xI = x(:,1);
            obj.xQ = x(:,2);
        end

        function [obj] = readCarrier( obj, txtFilePath )
            %read header
            %todo: instead of hard coded, read header from txt file
            obj.timeDuration_sec=1;
            obj.numSamplePts=2000;
            obj.numFftPts=2000;
            obj.samplingFreq_Hz=2000;
            obj.isComplex=1;
            obj.freqRng_Hz=[500, 2000];

            %todo: instead of hard coded, read header from txt file
            T = 1/obj.samplingFreq_Hz;
            obj.time_sec = (0 : (obj.numSamplePts-1))*T;

            %read xI and xQ
            nHeaderLines = 8;
            x = readmatrix(txtFilePath, "NumHeaderLines", nHeaderLines,"Delimiter",',');
            obj.xI = x(:,1);
            obj.xQ = x(:,2);
        end
        
        function [obj] = readMixed( obj, txtFilePath )
            %read header
            %todo: instead of hard coded, read header from txt file
            obj.timeDuration_sec=1;
            obj.numSamplePts=2000;
            obj.numFftPts=2000;
            obj.samplingFreq_Hz=2000;
            obj.isComplex=1;
            obj.freqRng_Hz=[500, 2000];

            %todo: instead of hard coded, read header from txt file
            T = 1/obj.samplingFreq_Hz;
            obj.time_sec = (0 : (obj.numSamplePts-1))*T;

            %read xI and xQ
            nHeaderLines = 16;
            x = readmatrix(txtFilePath, "NumHeaderLines", nHeaderLines,"Delimiter",',');
            obj.xI = x(:,1);
            obj.xQ = x(:,2);
        end
    end
end