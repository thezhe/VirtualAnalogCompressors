function y = gainTodB(x)
x(x<1.000e-05) = 1.000e-05;
y = 20.*log10(x);
end

