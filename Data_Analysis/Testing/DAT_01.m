clear;
clc;
close all;


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
        wifi.start = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        wifi.test_start = wifi.start-exp_config.start;
        ATAB_WIFI(table_wifi_config.idx,:) = {wifi.start, wifi.test_start,NaN,NaN};
    end
    if strcmp(key,"WIFI FIXED")
        wifi.end = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        wifi.test_end = wifi.end-exp_config.start;
        ATAB_WIFI(table_wifi_config.idx,3) = {wifi.test_end};
        ATAB_WIFI(table_wifi_config.idx,4) = {ATAB_WIFI.End(table_wifi_config.idx)-ATAB_WIFI.Start(table_wifi_config.idx)};   
    end
    if strcmp(key,"SKIP DETECTED")
        table_led_config.idx = table_led_config.idx+1;
        LED.time = datetime(extractBefore(TXT_webapp.S(TXT_webapp.txt_idx)," -- "),"InputFormat",DT_format.test);
        LED.test_time = LED.time-exp_config.start;
        ATAB_LED(table_led_config.idx,:) = {LED.time,LED.test_time,1};
    end
    
    TXT_webapp.txt_idx = TXT_webapp.txt_idx + 1;
end

clear "key"

ATAB_WIFI = convertvars(ATAB_WIFI, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
ATAB_WIFI = convertvars(ATAB_WIFI, {'Start','End'}, @(t) duration(t,'Format',"hh:mm:ss.SSS"));
ATAB_WIFI = convertvars(ATAB_WIFI, 'Duration', @(t) duration(t,'Format',"mm:ss.SSS"));

ATAB_LED = convertvars(ATAB_LED, "TimeStamp", @(t) datetime(t,'Format',"yyyy/MM/dd hh:mm:ss a"));
ATAB_LED = convertvars(ATAB_LED, 'Detect', @(t) duration(t,'Format',"hh:mm:ss.SSS"));



[TXT_ardu.file,TXT_ardu.location] = uigetfile('*.txt','Select the Arduino Datafile','');
if isequal(TXT_ardu.file,0)
   disp('User selected Cancel');
else
   disp(['User selected ', fullfile(TXT_ardu.location,TXT_ardu.file)]);
end

TXT_ardu.S = readlines(TXT_ardu.file,"WhitespaceRule","trim","EmptyLineRule","skip");

DT_format.config_ardu = "yyyy/M/d' -- 'H:mm:ss";
DT_format.test_ardu = "H:mm:ss";
ardu.init_datetime = datetime(extractBefore(TXT_ardu.S(2)," -- Starting"),"InputFormat",DT_format.config_ardu);
ardu.raw = {};
TXT_ardu.txt_idx = 3;
ardu.raw_cell_idx = 0;


table_ardu_config.sz = [1,3];
table_ardu_config.varNames = ["TimeStamp","Detect","Click"];
table_ardu_config.varTypes = ["datetime","duration","uint8"];
table_ardu.table = table('Size',table_ardu_config.sz,'VariableTypes',table_ardu_config.varTypes,'VariableNames',table_ardu_config.varNames);
table_ardu_config.idx = 1;
table_ardu.start = ardu.init_datetime;



while(TXT_ardu.txt_idx<length(TXT_ardu.S))
    if (~contains(TXT_ardu.S(TXT_ardu.txt_idx),"Loop"))
        table_ardu.ardutime = extractBefore(TXT_ardu.S(TXT_ardu.txt_idx)," -- ");
        key = extractAfter(TXT_ardu.S(TXT_ardu.txt_idx)," -- ");
        table_ardu.dt = datetime(extractBefore(table_ardu.ardutime,"-"),"InputFormat",DT_format.test_ardu);
        ardu_ms = str2double(extractAfter(table_ardu.ardutime,"-"));

        if strcmp(key,"Reset")
            if (ardu.raw_cell_idx~=0)
                ardu.raw{ardu.raw_cell_idx} = table_ardu.table;
                table_ardu.table = table('Size',table_ardu_config.sz,'VariableTypes',table_ardu_config.varTypes,'VariableNames',table_ardu_config.varNames);

            end
            ardu.raw_cell_idx = ardu.raw_cell_idx + 1;
            table_ardu.start = table_ardu.dt;
            table_ardu_config.idx = 1;
            
        end
        if (strcmp(key,"Skip"))
            table_ardu.dt_dt = table_ardu.dt - table_ardu.start;
            table_ardu.table(table_ardu_config.idx,:) = {table_ardu.dt,table_ardu.dt_dt,0};
            table_ardu_config.idx = table_ardu_config.idx + 1;
        end

    end

    TXT_ardu.txt_idx = TXT_ardu.txt_idx + 1;
end

ardu.raw{ardu.raw_cell_idx} = table_ardu.table;