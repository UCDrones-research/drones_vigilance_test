clear;
clc;
close all;

DT_format.test = "HH:mm:ss.SSS";

[TXT_drift.file,TXT_drift.location] = uigetfile('*.txt','Select the Webapp Datafile','');
if isequal(TXT_drift.file,0)
   disp('User selected Cancel');
else
   disp(['User selected ', fullfile(TXT_drift.location,TXT_drift.file)]);
end

TXT_drift.S = readlines(TXT_drift.file,"WhitespaceRule","trim","EmptyLineRule","skip");

table_drift_config.sz = [1,4];
table_drift_config.varNames = ["TimeStamp","RTC","Offset","Key"];
table_drift_config.varTypes = ["datetime","datetime","duration","string"];
ATAB_drift = table('Size',table_drift_config.sz,'VariableTypes',table_drift_config.varTypes,'VariableNames',table_drift_config.varNames);
table_drift_config.idx = 0;

for i = 1:length(TXT_drift.S)
    drift.ts = datetime(extractBefore(TXT_drift.S(i)," -> "),"InputFormat",DT_format.test);
    drift.rtc = datetime(extractBetween(TXT_drift.S(i)," -> ", " -- "),"InputFormat",DT_format.test);
    drift.dur = drift.rtc-drift.ts;
    drift.key = extractAfter(TXT_drift.S(i)," -- ");
    ATAB_drift(i,:) = {drift.ts, drift.rtc,drift.dur,drift.key};
end

ATAB_drift = convertvars(ATAB_drift, {'TimeStamp','RTC'}, @(t) datetime(t,'Format',"HH:mm:ss.SSS"));
ATAB_drift = convertvars(ATAB_drift, 'Offset', @(t) duration(t,'Format',"mm:ss.SSS"));

drift.init = mean(ATAB_drift.Offset(1:27));
drift.final = mean(ATAB_drift.Offset(2383:2398));
drift.exp_len = duration(1,6,49,826);

drift.scalefactor = ((drift.final-drift.init)/drift.exp_len);




for i = 1:length(ATAB_drift.RTC)
    drift.dt = milliseconds(ATAB_drift.RTC(i)-ATAB_drift.RTC(1));
    drift.correction = milliseconds(drift.scalefactor*drift.dt)+drift.init;
    newT(i,1) = ATAB_drift.RTC(i)-drift.correction;
    corre(i,1) = newT(i,1)-ATAB_drift.TimeStamp(i);
end

figure;
plot(milliseconds(corre),'--');
hold on;
plot(milliseconds(ATAB_drift.Offset),'--');