function [timeData stopPositions] = readAllPowellTime(directory, stepSize, timeLength)

stopPositions = [];
timeData = [];

groups = dir(directory);
for i = 3:length(groups)
    
    groupFolder = fullfile(directory, groups(i).name);
   
    [groupData stopData] = readGroupPowellTime(groupFolder, stepSize, timeLength);
    
    timeData = [timeData ; groupData];
    stopPositions = [stopPositions; stopData];
end

end