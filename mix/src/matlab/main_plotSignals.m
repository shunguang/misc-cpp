function main_plotSignals
close all
clearvars;

f="../../data/baseband.txt";

readSig = ReadSignal();
[bb] = readSig.readBaseband(f);

%[bb] = readBasebandSignal( f);
%bb.isComplex = false;
plotSignal(bb, 'time', 'baseband');
plotSignal(bb, 'freq', 'baseband');

f="../../data/carrier.txt";
[cs] = readSig.readCarrier( f);
plotSignal(cs, 'time', 'carrier');
plotSignal(cs, 'freq', 'carrier');

f="../../data/mixed_0.txt";
[mixed] = readSig.readMixed( f);
plotSignal(mixed, 'time', 'mixed');
plotSignal(mixed, 'freq', 'mixed');

end
