function plotGroupESStopTime(directory, timeLength, position, errorbarOptions)

% read all data
[data stopTime] = readGroupESTime(directory, timeLength, timeLength);

errorbar(position, mean(stopTime), std(stopTime), errorbarOptions);

end