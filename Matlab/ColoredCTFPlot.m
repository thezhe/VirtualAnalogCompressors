H = -0.5;
W = 1;
thrdB = -20;
R = 5;

xdB = -40:0.01:0;

CTF = zeros(1, length(xdB));

for i = 1:length(xdB)
    color = H/(1+((xdB(i)-thrdB)/W)^2);
    if (xdB(i) <= thrdB)
        CTF(i) = color + xdB(i);
    else
        CTF(i) = color + thrdB + (xdB(i)-thrdB)/R;
    end
end

figure(1)
hold on
set(gca, 'FontSize', 24);
title('Algebraic Colored CTF');
xlabel('x_{dB}');
ylabel('CTF(x_{dB})')

if (H==1)
plot(xdB, CTF, '-', 'LineWidth', 2);
end

if (H==-3)
plot(xdB, CTF, '--', 'LineWidth', 2);
end

if (H==-0.5)
plot(xdB, CTF, ':', 'LineWidth', 2);
end

legend('H=1, W=1','H=-3, W=2', 'H=-0.5, W=1')
hold off
    