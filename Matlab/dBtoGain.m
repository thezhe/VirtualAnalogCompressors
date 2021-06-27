function y = dBtoGain(x)
x(x<-100) = -100;
y = 10.^(x/20);
end

