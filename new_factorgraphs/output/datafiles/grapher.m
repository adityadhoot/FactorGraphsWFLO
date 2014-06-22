A = xlsread ('T40_10_10_WR1.xls');
B = A(:,1);
C = A(:,2);
plot (B,C, 'b--o'); 
%axis ([0,60,0,100]);
title ('WR1 - 40 Turbines');
xlabel ('Number of Generations');
ylabel ('Farm Efficiency');
