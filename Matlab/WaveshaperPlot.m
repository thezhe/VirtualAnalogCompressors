function WaveshaperPlot(audiofile1, audiofile2, figNum, dB, dotSize)
%WAVESHAPERPLOT Plot samples of audiofile1 vs audiofile2

x = audioread(audiofile1);
y = audioread(audiofile2);
xInfo = audioinfo(audiofile1);
yInfo = audioinfo(audiofile2);
[~, xName,~] = fileparts(xInfo.Filename);
[~, yName,~] = fileparts(yInfo.Filename);

if (dB)
   x = gainTodB(x); 
   y = gainTodB(y); 
   xlim([-60, 0]);
   ylim([-60, 0]);
   
end

minLength = min(length(x), length(y));
figure(figNum);
hold on
set(gca, 'FontSize', 24);
if(dB)
xlab = [xName, '.wav (dB)'];
ylab = [yName, '.wav (dB)'];
else
xlab = [xName, '.wav'];
ylab = [yName, '.wav'];
end
xlabel(xlab);
ylabel(ylab);
title(yName);
scatter(x(1:minLength), y(1:minLength), dotSize, 'filled');
hold off
end

