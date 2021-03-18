function PlotDir(title_, xlabel_, ylabel_, sampleRate, seconds)
%PLOTDIR Plots all wav files in function directory

%get wav files
files = dir('*.wav');
nFiles = length(files);
lengthSamples = seconds*sampleRate;

%allocate for data
data = zeros(lengthSamples,length(files));
names = strings(nFiles,1);

%load data
for f = 1:nFiles
    names(f) = files(f).name;
    data(:,f) = audioread(files(f).name);
end
    %plot data
t = 0:lengthSamples-1;
t = t/lengthSamples;

hold on
title(title_);
xlabel(xlabel_);
ylabel(ylabel_);
for f = 1:2
    plot(t, data(:,f));
end
legend(names);
hold off


