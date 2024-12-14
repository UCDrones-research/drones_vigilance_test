clear;
clc;
close all;


data_list = dir("*.mat");
x=[];
y=[];
x_users=[];
y_users=[];
detectRates = [];
for i = 1:length(data_list)
    load(data_list(i).name);
    uname = string(data_list(i).name(6));
    x = cat(1,x,Results.wifi.raw);
    x_users = cat(1,x_users,categorical(repmat(uname,1,length(Results.wifi.raw)))');
    y= cat(1,y,Results.leds.raw);
    y_users = cat(1,y_users,categorical(repmat(uname,1,length(Results.leds.raw)))');
    detectRates = cat(1,detectRates,[sum(~isnan(Results.leds.table.ResponseTime)),Results.leds.Missed,Results.leds.FP]);

end

figure;
subplot(1,2,1);
boxchart(categorical(x_users),x);
grid on;
title('Wifi Fix Response Time');
ylabel('Seconds');
set(gca,'XTickLabel',{'Big','Laptop','Small'});

subplot(1,2,2);
boxchart(categorical(y_users),y);
grid on;
title('LED Detect Response Time');
ylabel('Seconds');
set(gca,'XTickLabel',{'Big','Laptop','Small'});

figure;
bar(detectRates,'stacked');
set(gca,'XTickLabel',{'Big','Laptop','Small'});
legend('Success','Missed','False Positive');
ylabel('Count');
title('LED Detect Counts');