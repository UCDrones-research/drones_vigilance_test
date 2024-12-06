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
    xardu.ts = datetime(extractBefore(TXT_ardu.S(TXT_ardu.txt_idx)," -- "),"InputFormat",DT_format.test_ardu);
    [xardu.ts.Year,xardu.ts.Month,xardu.ts.Day] = ymd(xardu.init_datetime);
    xardu.dur = xardu.ts-ATAB_ARDU.TimeStamp(1);
    xardu.key = extractAfter(TXT_ardu.S(TXT_ardu.txt_idx)," -- ");
    ATAB_ARDU(xardu.tab_idx,:) = {xardu.ts, xardu.dur,xardu.key};

    xardu.tab_idx = xardu.tab_idx + 1;
    TXT_ardu.txt_idx = TXT_ardu.txt_idx + 1;
end

ATAB_ARDU = convertvars(ATAB_ARDU, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
ATAB_ARDU = convertvars(ATAB_ARDU, 'Duration', @(t) duration(t,'Format',"hh:mm:ss.SSS"));

%% Syncronize

P1a = exp_config.start;
P2a = ATAB_LED.TimeStamp(end);

P1b = ATAB_ARDU.TimeStamp(1);
P2b = ATAB_ARDU.TimeStamp(end);

P1_offset = P1b-P1a;
P2_offset = P2b-P2a;

a_duration = P2a-P1a;
b_duration = P2b-P1b;

scalefactor = ((P2_offset-P1_offset)/a_duration);

for i = 1:length(ATAB_ARDU.TimeStamp)
    dt = ATAB_ARDU.TimeStamp(i)-ATAB_ARDU.TimeStamp(1);
    correction = scalefactor*dt+P1_offset;
    newT(i,1) = ATAB_ARDU.TimeStamp(i)-correction;
end


TT1 = timetable(newT,int8(strcmp(ATAB_ARDU.Key,"Skip")),'VariableNames',{'Skip'});
TT2 = timetable(ATAB_LED.TimeStamp,ATAB_LED.Click,'VariableNames',{'Det'});
TTT = synchronize(TT1,TT2,'union','fillwithconstant','Constant',8);

Results.FP = 0;
Results.Missed = 0;
Results.table = table('Size',[1,2],'VariableTypes',["datetime","duration"],'VariableNames',["TimeStamp","ResponseTime"]);
Results.table_idx = 1;

for i = 1:length(TTT.newT)
    if (TTT.Skip(i)==1) %if skip issued
        if (TTT.Det(i+1)==1) %look for detect
            Results.duration = TTT.newT(i+1)-TTT.newT(i);
            Results.table(Results.table_idx,:) = {TTT.newT(i),Results.duration};
            Results.table_idx = Results.table_idx + 1;
        else %no detect found
            Results.Missed = Results.Missed+1;
        end


    end

end

Results.table = convertvars(Results.table, 'ResponseTime', @(t) duration(t,'Format',"hh:mm:ss.SSS"));