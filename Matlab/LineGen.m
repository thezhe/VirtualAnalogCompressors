function LineGen(mindB, maxdB, tmax, filename, fs)
%SIGNALGEN Generate a dc or ramp signal from mindB to maxdB

n = floor(tmax*fs);

y = linspace(mindB, maxdB, n);
y = dBtoGain(y);

audiowrite(filename, y, fs);
end

