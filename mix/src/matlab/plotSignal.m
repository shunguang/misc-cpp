function plotSignal(s, flag, sigName)
    if strcmp( flag, 'time')
        plotSignalTimeDomain(s, sigName);
    else
        plotSignalFreqDomain(s, sigName);
    end
end

function plotSignalTimeDomain(s, sigName)
figure
subplot(2,1,1)
plot(s.time_sec, s.xI, 'g-')
ylabel('xI');
xlabel('time(sec)');
title(sigName)
if s.isComplex
    subplot(2,1,2)
    plot(s.time_sec, s.xQ, 'b-');
    ylabel('xQ');
    xlabel('time(sec)');
end
end


function plotSignalFreqDomain(s, sigName)
fs = s.samplingFreq_Hz;
T = 1/fs;
nFFT = s.numFftPts;
assert( 0==mod(nFFT,2), 'nFFT must be even' );

if s.isComplex
   x = s.xI + 1i * s.xQ;
else
   x = s.xI;
end
Y = fft(x, nFFT);
psd = abs(Y/nFFT);             %normlization by <nFFT>
freq = fs*(0:(nFFT-1))/nFFT;     %correspending freq bins


titleStr = [sigName, 'T=', num2str(T), 'sec, fs=', num2str(fs), 'Hz, nFFT=', num2str(nFFT), ', nSignaleSamples=', num2str(s.numSamplePts), ',\Delta f=', num2str(fs/nFFT), 'Hz'];
figure 
if s.isComplex
    plot(freq,psd, 'b-+');
    xlim([0,fs])
    xlabel('f (Hz)');
    ylabel('psd(f)|');
    title( ['complex signal: ', titleStr]);
else
    %using the symetry property do a one side plot
    m = nFFT/2+1;
    f2   = freq(1 : m);
    psd2 = psd(1 : m);    
    %psd(1) -- direct component
    psd2(2:end-1) = 2*psd2(2:end-1);  
    plot(f2,psd2, 'b-+');
    xlim([0, f2(end)])
    xlabel('f (Hz)');
    ylabel('psd(f)|');
    title( ['real signal: ', titleStr]);
end

end

