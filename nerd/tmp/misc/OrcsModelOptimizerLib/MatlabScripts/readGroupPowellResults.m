function errors = readGroupPowellResults(groupFolder)

errors = [];

individualFolders = dir(groupFolder);
    
for j = 3:length(individualFolders)

    individualFolder = fullfile(groupFolder, individualFolders(j).name);

    statFile = dir(fullfile(individualFolder,'*Statistics.log')); 
    
    if (~isempty(statFile))
        
        [labels, data] = readOrcsStatisticsValues(fullfile(individualFolder, statFile(1).name), 'ModelError');

        errors = [errors data(length(data))];    
    end
end

end