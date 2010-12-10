function [timeData stopPositions] = readGroupPowellTime(groupFolder, stepSize, timeLength)

stopPositions = [];
timeData = [];

individualFolders = dir(groupFolder);
    
for j = 3:length(individualFolders)

    individualFolder = fullfile(groupFolder, individualFolders(j).name);

    [singelData stopPosition]= readSinglePowellTime(individualFolder, stepSize, timeLength);
        
    if(~isempty(singelData)) 
        timeData = [timeData; singelData];
        stopPositions = [stopPositions ; stopPosition];
    end
end


end