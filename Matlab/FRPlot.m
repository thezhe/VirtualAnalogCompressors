function FRPlot(audiofile, figNum)
%FR Plot the fft 

%Get samples and filename 
[x, fs] = audioread(audiofile);
xInfo = audioinfo(audiofile);
[~, figName,~] = fileparts(xInfo.Filename);

y = abs(fft(x)/length(x));
y = y(1:(length(y)/2)+1);
y(2:end-1) = y(2:end-1)*2;
y = gainTodB(y);

step = (fs/2)/length(y);
f = 0:step:fs/2-step;

figure(figNum)
semilogx(f, y, 'LineWidth', 2);
hold on
set(gca, 'FontSize', 24);
title(figName);
xlabel('frequency (Hz)');
ylabel('magnitude (dB)')
ylim([-90, 0]);
xlim([0, (fs/2)]);
hold off

end

