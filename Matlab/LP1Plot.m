function LP1Plot(figNum)
%LP1PLOT Plot One-Pole lowpass output for different resonances 
%Cutoff should be unit cutoff (i.e. 1)
R = 0.1;
W = logspace(-1, 1, 10000);

yLP = 1./sqrt(W.^2+1);
yLPdB = gainTodB(yLP);


semilogx(W, yLPdB, 'LineWidth', 2);
set(gca, 'FontSize', 24);
xlabel('\Omega');
ylabel('|H(j\Omega)|, dB');
title('One-Pole Low-Pass Magnitude Response');

end
