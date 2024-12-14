%% Init
clear;
clc;
close all;

%% Webfile Parse
[TXT_webapp.file,TXT_webapp.location] = uigetfile('*.txt','Select the Webapp Datafile','');
if isequal(TXT_webapp.file,0)
   disp('User selected Cancel');
else
   disp(['User selected ', fullfile(TXT_webapp.location,TXT_webapp.file)]);
end

TXT_webapp.S = readlines(TXT_webapp.file,"WhitespaceRule","trim","EmptyLineRule","skip");


exp_config.pilot = extractAfter(TXT_webapp.S(2),4);

DT_format.config = "yyyy/MM/dd' - 'h:mm:ss a";
exp_config.datetime = datetime(extractAfter(TXT_webapp.S(3),"Setup: "),"InputFormat",DT_format.config);


DT_format.test = "h:mm:ss a SSS";
exp_config.start = datetime(extractBefore(TXT_webapp.S(5)," -- Launch"),"InputFormat",DT_format.test);
[exp_config.start.Year, exp_config.start.Month, exp_config.start.Day] = ymd(exp_config.datetime); 

table_wifi_config.sz = [1,4];
table_wifi_config.varNames = ["TimeStamp","Start","End","Duration"];
table_wifi_config.varTypes = ["datetime","duration","duration","duration"];
ATAB_WIFI = table('Size',table_wifi_config.sz,'VariableTypes',table_wifi_config.varTypes,'VariableNames',table_wifi_config.varNames);
table_wifi_config.idx = 0;

table_led_config.sz = [1,3];
table_led_config.varNames = ["TimeStamp","Detect","Click"];
table_led_config.varTypes = ["datetime","duration","uint8"];
ATAB_LED = table('Size',table_led_config.sz,'VariableTypes',table_led_config.varTypes,'VariableNames',table_led_config.varNames);
table_led_config.idx = 0;

key = "Start";
TXT_webapp.txt_idx = 6;
while(~strcmp(key,"LAND"))
    key = extractAfter(TXT_webapp.S(TXT_webapp.txt_idx)," -- ");
    if strcmp(key,"SIGNAL DOWN")
        table_wifi_config.idx = table_wifi_config.idx + 1;
        xwifi.start = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        [xwifi.start.Year,xwifi.start.Month,xwifi.start.Day] = ymd(exp_config.datetime);
        xwifi.test_start = xwifi.start-exp_config.start;
        ATAB_WIFI(table_wifi_config.idx,:) = {xwifi.start, xwifi.test_start,NaN,NaN};
    end
    if strcmp(key,"WIFI FIXED")
        xwifi.end = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        [xwifi.end.Year,xwifi.end.Month,xwifi.end.Day] = ymd(exp_config.datetime);
        xwifi.test_end = xwifi.end-exp_config.start;
        ATAB_WIFI(table_wifi_config.idx,3) = {xwifi.test_end};
        ATAB_WIFI(table_wifi_config.idx,4) = {ATAB_WIFI.End(table_wifi_config.idx)-ATAB_WIFI.Start(table_wifi_config.idx)};   
    end
    if strcmp(key,"SKIP DETECTED")
        table_led_config.idx = table_led_config.idx+1;
        xLED.time = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        [xLED.time.Year,xLED.time.Month,xLED.time.Day] = ymd(exp_config.datetime);
        xLED.test_time = xLED.time-exp_config.start;
        ATAB_LED(table_led_config.idx,:) = {xLED.time,xLED.test_time,1};
    end
    if strcmp(key,"LAND")
        table_led_config.idx = table_led_config.idx+1;
        xLED.time = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        [xLED.time.Year,xLED.time.Month,xLED.time.Day] = ymd(exp_config.datetime);
        xLED.test_time = xLED.time-exp_config.start;
        ATAB_LED(table_led_config.idx,:) = {xLED.time,xLED.test_time,0};
    end
    
    TXT_webapp.txt_idx = TXT_webapp.txt_idx + 1;
end




clear key

ATAB_WIFI = convertvars(ATAB_WIFI, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
ATAB_WIFI = convertvars(ATAB_WIFI, {'Start','End'}, @(t) duration(t,'Format',"hh:mm:ss.SSS"));
ATAB_WIFI = convertvars(ATAB_WIFI, 'Duration', @(t) duration(t,'Format',"mm:ss.SSS"));

ATAB_LED = convertvars(ATAB_LED, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
ATAB_LED = convertvars(ATAB_LED, 'Detect', @(t) duration(t,'Format',"hh:mm:ss.SSS"));

Results.wifi.table = ATAB_WIFI;
Results.wifi.raw = milliseconds(ATAB_WIFI.Duration)/1000;
[Results.wifi.norm.h,Results.wifi.norm.p,Results.wifi.norm.adstat,Results.wifi.norm.cv] = adtest(Results.wifi.raw);
Results.wifi.dist.mean = mean(Results.wifi.raw);
Results.wifi.dist.std = std(Results.wifi.raw);
Results.wifi.dist.var = var(Results.wifi.raw);
Results.wifi.dist.skew = skewness(Results.wifi.raw);
Results.wifi.dist.kurt = kurtosis(Results.wifi.raw);
Results.wifi.box.min = min(rmoutliers(Results.wifi.raw,'quartiles'));
Results.wifi.box.q1 = quantile(Results.wifi.raw,0.25);
Results.wifi.box.median = median(Results.wifi.raw);
Results.wifi.box.q3 = quantile(Results.wifi.raw,0.75);
Results.wifi.box.max = max(rmoutliers(Results.wifi.raw,'quartiles'));
Results.wifi.box.outliers = Results.wifi.raw(isoutlier(Results.wifi.raw,'quartiles'));
temp.a = lognfit(Results.wifi.raw);
Results.wifi.logndist.mu = temp.a(1);
Results.wifi.logndist.sigma = temp.a(2);




%% Arduino Parse

[TXT_ardu.file,TXT_ardu.location] = uigetfile('*.txt','Select the Arduino Datafile','');
if isequal(TXT_ardu.file,0)
   disp('User selected Cancel');
else
   disp(['User selected ', fullfile(TXT_ardu.location,TXT_ardu.file)]);
end

TXT_ardu.S = readlines(TXT_ardu.file,"WhitespaceRule","trim","EmptyLineRule","skip");

DT_format.config_ardu = "yyyy-MM-dd'T'HH:mm:ss";
DT_format.test_ardu = "HH:mm:ss.SSS";
xardu.init_datetime = datetime(TXT_ardu.S(2),"InputFormat",DT_format.config_ardu);

TXT_ardu.txt_idx = 5;
xardu.tab_idx = 2;


table_ardu_config.sz = [1,3];
table_ardu_config.varNames = ["TimeStamp","Duration","Key"];
table_ardu_config.varTypes = ["datetime","duration","string"];
ATAB_ARDU = table('Size',table_ardu_config.sz,'VariableTypes',table_ardu_config.varTypes,'VariableNames',table_ardu_config.varNames);
table_ardu_config.idx = 1;
table_ardu_config.start = xardu.init_datetime;

xardu.start = datetime(extractBefore(TXT_ardu.S(4)," -- "),"InputFormat",DT_format.test_ardu);
[xardu.start.Year,xardu.start.Month,xardu.start.Day] = ymd(xardu.init_datetime);

ATAB_ARDU(1,:) = {xardu.start,duration(0,0,0),"Starting Experiment"};

while(TXT_ardu.txt_idx<=length(TXT_ardu.S))
    xardu.key = extractAfter(TXT_ardu.S(TXT_ardu.txt_idx)," -- ");
    if ~strcmp(xardu.key,"Loop")
        xardu.ts = datetime(extractBefore(TXT_ardu.S(TXT_ardu.txt_idx)," -- "),"InputFormat",DT_format.test_ardu);
        [xardu.ts.Year,xardu.ts.Month,xardu.ts.Day] = ymd(xardu.init_datetime);
        xardu.dur = xardu.ts-ATAB_ARDU.TimeStamp(1);
        ATAB_ARDU(xardu.tab_idx,:) = {xardu.ts, xardu.dur,xardu.key};
        xardu.tab_idx = xardu.tab_idx + 1;
    end
    TXT_ardu.txt_idx = TXT_ardu.txt_idx + 1;
end

ATAB_ARDU = convertvars(ATAB_ARDU, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
ATAB_ARDU = convertvars(ATAB_ARDU, 'Duration', @(t) duration(t,'Format',"hh:mm:ss.SSS"));

%% Syncronize

sync.P1a = exp_config.start;
sync.P2a = ATAB_LED.TimeStamp(end);

sync.P1b = ATAB_ARDU.TimeStamp(1);
sync.P2b = ATAB_ARDU.TimeStamp(end);

sync.P1_offset = sync.P1b-sync.P1a;
sync.P2_offset = sync.P2b-sync.P2a;

sync.a_duration = sync.P2a-sync.P1a;
sync.b_duration = sync.P2b-sync.P1b;

sync.scalefactor = ((sync.P2_offset-sync.P1_offset)/sync.a_duration);

for i = 1:length(ATAB_ARDU.TimeStamp)
    sync.dt = ATAB_ARDU.TimeStamp(i)-ATAB_ARDU.TimeStamp(1);
    sync.correction = sync.scalefactor*sync.dt+sync.P1_offset;
    newT(i,1) = ATAB_ARDU.TimeStamp(i)-sync.correction;
end


TT1 = timetable(newT,int8(strcmp(ATAB_ARDU.Key,"Skip")),'VariableNames',{'Skip'});
TT2 = timetable(ATAB_LED.TimeStamp,ATAB_LED.Click,'VariableNames',{'Det'});
TTT = synchronize(TT1,TT2,'union','fillwithconstant','Constant',8);

%%
Results.leds.FP = 0;
Results.leds.Missed = 0;
Results.leds.table = table('Size',[1,4],...
    'VariableTypes',["datetime","duration","duration","duration"],...
    'VariableNames',["TimeStamp","Skip","Detect","ResponseTime"]);


Results.leds.table(1,:) = {TTT.newT(1),duration(0,0,0),duration(0,0,0),NaN}; %Start
Results.leds.start = TTT.newT(1);

Results.table_idx = 2;
i=2;
while i < length(TTT.newT)
    if (TTT.Skip(i)==1) %if skip issued
        if (TTT.Det(i+1)==1) %look for detect
            temp.duration = TTT.newT(i+1)-TTT.newT(i);
            if (temp.duration<duration(0,0,8)) % only count if next detect is within 8 seconds
                Results.leds.table(Results.table_idx,:) = {TTT.newT(i), ...
                                                           TTT.newT(i)-Results.leds.start,...
                                                           TTT.newT(i+1)-Results.leds.start,...
                                                           temp.duration};
                Results.table_idx = Results.table_idx + 1;
                i = i+1; %also skip next line
            else %skip issued and no detect within 8 seconds
                Results.leds.table(Results.table_idx,:) = {TTT.newT(i), ...
                                                           TTT.newT(i)-Results.leds.start,...
                                                           NaN,...
                                                           NaN};
                Results.table_idx = Results.table_idx + 1;
                Results.leds.Missed = Results.leds.Missed+1;
            end
        else %skip issued, no detect
            Results.leds.table(Results.table_idx,:) = {TTT.newT(i), ...
                                                       TTT.newT(i)-Results.leds.start,...
                                                       NaN,...
                                                       NaN};
            Results.table_idx = Results.table_idx + 1;
            Results.leds.Missed = Results.leds.Missed+1;
        end
    else %no skip issued
          if (TTT.Det(i)==1) %Detect with no skip
                Results.leds.table(Results.table_idx,:) = {TTT.newT(i),...
                                                           NaN,...
                                                           TTT.newT(i)-Results.leds.start,...
                                                           NaN};
                Results.table_idx = Results.table_idx + 1;
                Results.leds.FP = Results.leds.FP+1;
           end
    end
    i = i+1;
end

Results.leds.table = convertvars(Results.leds.table, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
Results.leds.table = convertvars(Results.leds.table, 'ResponseTime', @(t) duration(t,'Format',"hh:mm:ss.SSS"));
Results.leds.raw = milliseconds(Results.leds.table.ResponseTime(~isnan(Results.leds.table.ResponseTime)))/1000;
%%
[Results.leds.norm.h,Results.leds.norm.p,Results.leds.norm.adstat,Results.leds.norm.cv] = adtest(Results.leds.raw);
Results.leds.dist.mean = mean(Results.leds.raw);
Results.leds.dist.std = std(Results.leds.raw);
Results.leds.dist.var = var(Results.leds.raw);
Results.leds.dist.skew = skewness(Results.leds.raw);
Results.leds.dist.kurt = kurtosis(Results.leds.raw);
Results.leds.box.min = min(rmoutliers(Results.leds.raw,'quartiles'));
Results.leds.box.q1 = quantile(Results.leds.raw,0.25);
Results.leds.box.median = median(Results.leds.raw);
Results.leds.box.q3 = quantile(Results.leds.raw,0.75);
Results.leds.box.max = max(rmoutliers(Results.leds.raw,'quartiles'));
Results.leds.box.outliers = Results.leds.raw(isoutlier(Results.leds.raw,'quartiles'));
temp.a = lognfit(Results.leds.raw);
Results.leds.logndist.mu = temp.a(1);
Results.leds.logndist.sigma = temp.a(2);

answer = inputdlg('Enter UserKey');

fname = string(['User-',answer{1},'-',char(exp_config.datetime,'yyyyMMddHHmm'),'.mat']);

save(fname,"Results");