clear;
clc;
close all;

load User-Albert1-202412121317.mat

x1 = Results.wifi.raw;
y1 = Results.leds.raw;

load User-Kyra1-202412121355.mat

x2 = Results.wifi.raw;
y2 = Results.leds.raw;

x_users = categorical([repmat("Albert",1,length(x1)),repmat("Kyra",1,length(x2))]);
x_alldata = [x1',x2'];

figure;
boxchart(x_users,x_alldata);
title('Wifi Fix Response Time');
ylabel('Seconds');

y_users = categorical([repmat("Albert",1,length(y1)),repmat("Kyra",1,length(y2))]);
y_alldata = [y1',y2'];

figure;
boxchart(y_users,y_alldata);
title('Skip Detect Response Time');
ylabel('Seconds');