function LP2Plot(figNum)
%LP2PLOT Plot SVF lowpass output for different resonances 
%Cutoff should be unit cutoff (i.e. 1)
W = logspace(-1, 1, 10000);

for R = [0.01, 0.4, 1, 2]
if (R < 1)
yLP = 1./(sqrt((W+sqrt(1-R^2)).^2 + R^2));
yLP = yLP./(sqrt((W-sqrt(1-R^2)).^2 + R^2));
else
yLP = 1./(sqrt(W.^2 + (R+sqrt(R^2+1))^2));
yLP = yLP./(sqrt(W.^2 + (R-sqrt(R^2+1))^2));
end

yLPdB = gainTodB(yLP);

figure(figNum);

if (R ~= 0.01)
    hold on
end

if (R == 0.01)
    semilogx(W, yLPdB, '-' , 'LineWidth', 2);
end

if (R == 0.4)
    semilogx(W, yLPdB, '-.' , 'LineWidth', 2);
end

if (R == 1)
    semilogx(W, yLPdB, '--', 'LineWidth', 2);
end
      
if (R == 2)
    semilogx(W, yLPdB, ':', 'LineWidth', 2);
end

if (R ~= 0.01)
    hold off
end
end

set(gca, 'FontSize', 24);
xlabel('\Omega');
ylabel('|H(j\Omega)|, dB');
title('State-Variable Filter Low-Pass Magnitude Response');
legend('R = 0.1', 'R = 0.5', 'R = 1', 'R = 2');

end
