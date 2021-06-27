function PulseGen(value1, time1, value2, time2, filename, fs)
%PULSEGEN Generates a wav with value 1 for time1 and value 2 for time 2
n = (time1 + time2) * fs;
y = zeros(1, n);
y(1, 1:time1*fs) = value1;
y(1, time1*fs + 1:end) = value2;
audiowrite(filename, y, fs);
end
