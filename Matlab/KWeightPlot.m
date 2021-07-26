%https://www.mathworks.com/help/audio/ref/weightingfilter-system-object.html

wF = weightingFilter('K-weighting');

fvtool(wF,'FrequencyScale','Log','Fs',wF.SampleRate)
