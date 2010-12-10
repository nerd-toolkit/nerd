function plotGroupESTime(directory, stepSize, timeLength, plotOptions, errorbarStepLength, errorbarOptions)

% read all data
groupData = readGroupESTime(directory, stepSize, timeLength);


plot([0:stepSize:timeLength], mean(groupData),plotOptions);

for i=1:length(groupData(1,:))
   
    if(mod(i, errorbarStepLength) == 0)
        errorbar((i-1)*stepSize, mean(groupData(:,i)), std(groupData(:,i)), errorbarOptions);
    end
end

end