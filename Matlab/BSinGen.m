function BSinGen(filename, f1,f2)
%MODSINGEN Generate a one second sin with frequency f1 biased by another sin with
%frequency f2 and 5 times the amplitude
% Based on Fig. 4 of https://dafx2019.bcu.ac.uk/papers/DAFx2019_paper_3.pdf
Fs = 192000;
T = 1/Fs;
n = 1:Fs;

A1 = 1/6;
A2 = 5/6;

wd1 = 2*pi*T*f1;
wd2 = 2*pi*T*f2;


sine = sin(wd1*n);
bias = cos(wd2*n); 

y = A1*sine + A2*bias;

audiowrite(filename, y, Fs);

end

