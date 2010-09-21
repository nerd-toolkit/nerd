function plotGroupPowellTime(directory, stepSize, timeLength, plotOptions, errorbarStepLength, errorbarOptions)

% read all data
[data stopPositions] = readGroupPowellTime(directory, stepSize, timeLength);

plot([0:stepSize:timeLength], mean(data),plotOptions);

for i=1:length(data(1,:))
   
    if(mod(i, errorbarStepLength) == 0)
        errorbar((i-1)*stepSize, mean(data(:,i)), std(data(:,i)), errorbarOptions);
    end
end

end