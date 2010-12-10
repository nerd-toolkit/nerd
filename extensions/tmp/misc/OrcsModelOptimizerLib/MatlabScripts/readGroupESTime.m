function [timeData minTime]= readGroupESTime(groupFolder, stepSize, timeLength)

timeData = [];
minTime = [];

individualFolders = dir(groupFolder);
    
for j = 3:length(individualFolders)

    individualFolder = fullfile(groupFolder, individualFolders(j).name);

    [singelData singleMinTime] = readSingleESTime(individualFolder, stepSize, timeLength);
        
    if(~isempty(singelData)) 
        timeData = [timeData; singelData];
        minTime  = [minTime; singleMinTime];
    end
end


end