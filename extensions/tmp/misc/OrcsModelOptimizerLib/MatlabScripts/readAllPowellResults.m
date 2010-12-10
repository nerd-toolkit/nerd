function errors = readAllPowellResults(folder)


errors = [];

groups = dir(folder);
for i = 3:length(groups)
    
    groupFolder = fullfile(folder, groups(i).name);
   
    errors = [errors readGroupPowellResults(groupFolder)];
end

end