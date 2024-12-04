clear;
clc;
close all;


[file,location] = uigetfile('*.txt','Select the Webapp Datafile','');
if isequal(file,0)
   disp('User selected Cancel');
else
   disp(['User selected ', fullfile(location,file)]);
end

S = readlines(file,"WhitespaceRule","trim","EmptyLineRule","skip");

test_pilot = extractAfter(S(2),4);
test_string_datetime = extractAfter(S(3),"Setup: ");

infmt_config = "yyyy/MM/dd' - 'h:mm:ss a";
test_datetime = datetime(test_string_datetime,"InputFormat",infmt_config);

infmt_test = "h:mm:ss a SSS";
launch_start = datetime(extractBefore(S(5)," -- Launch"),"InputFormat",infmt_test);

outfmt_wifi = "hh:mm:ss.SSS";

table_wifi_sz = [1,4];
varNames = ["TimeStamp","Start","End","Duration"];
varTypes = ["datetime","duration","duration","duration"];
table_wifi = table('Size',table_wifi_sz,'VariableTypes',varTypes,'VariableNames',varNames);
table_wifi_idx = 0;

key = "Start";
test_idx = 6;
while(~strcmp(key,"LAND"))
    key = extractAfter(S(test_idx)," -- ");
    if strcmp(key,"SIGNAL DOWN")
        table_wifi_idx = table_wifi_idx + 1;
        wifi_start = datetime(extractBefore(S(test_idx)," -- "),"InputFormat",infmt_test);
        wifi_test_start = wifi_start-launch_start;
        table_wifi(table_wifi_idx,:) = {wifi_start, wifi_test_start,NaN,NaN};
    end
    if strcmp(key,"WIFI FIXED")
        wifi_end = datetime(extractBefore(S(test_idx)," -- "),"InputFormat",infmt_test);
        wifi_test_end = wifi_end-launch_start;
        table_wifi(table_wifi_idx,3) = {wifi_test_end};
        temp = table_wifi.End(table_wifi_idx)-table_wifi.Start(table_wifi_idx);
        %temp.Format = outfmt_wifi;
        table_wifi(table_wifi_idx,4) = {temp};   
    end
    
    test_idx = test_idx + 1;
end

test = convertvars(table_wifi, @isduration, @(t) duration(t,'Format',outfmt_wifi));