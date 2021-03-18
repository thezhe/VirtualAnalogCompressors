function PulseGen(value1, time1, value2, time2, samplingRate, fileName)
%REFFX Generates a wav with value 1 for time1 and value 2 for time 2
length = (time1+time2)*samplingRate;
pulse = zeros(1, length);
pulse(1, 1:time1*samplingRate) = value1;
pulse(1, time1*samplingRate + 1:end) = value2;
audiowrite(fileName, pulse, samplingRate);
end
