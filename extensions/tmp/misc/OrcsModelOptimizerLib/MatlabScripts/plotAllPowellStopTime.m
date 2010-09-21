function plotAllPowellStopTime(directory, position, errorbarOptions)

% read all data
[data stopTime] = readAllPowellTime(directory, 1, 1);

errorbar(position, mean(stopTime), std(stopTime),errorbarOptions);

sprintf('Mean Stop Time from %s: %i', directory, mean(stopTime))

end