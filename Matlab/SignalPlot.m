function SignalPlot(audiofile, figNum, lineWidth)
%SIGNALPLOT Plot samples of audiofile

%Get y and t vectors and the figName
[y, fs] = audioread(audiofile);
xInfo = audioinfo(audiofile);
[~, figName,~] = fileparts(xInfo.Filename);
T = 1/fs;
t = 0:T:xInfo.Duration-T;

%Plot with y limits equal to [-1, 1]
figure(figNum);
hold on
xlabel('t (seconds)');
ylabel('value');
title(figName);

set(gca, 'FontSize', 24);
ylim([-1, 1]);

plot(t, y, 'LineWidth', lineWidth);
hold off
end