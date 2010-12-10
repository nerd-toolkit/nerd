function plotGroupMotionDiff(diffData, group, position, errorbarOptions, withWorst, worstOptions)

% calculate mean motionDiff
groupData = [];
for i = 1 : length(diffData)
    
    % check if both comapred files are in the specified group 
    if(strcmp(diffData{i,1}.group, group) && strcmp(diffData{i,2}.group, group) )    
        groupData = [groupData diffData{i,3}];
    end
end

fprintf('Compared %d files for group %s.\n',length(groupData), group);

if(length(groupData) > 1)
    errorbar(position, mean(groupData), std(groupData), errorbarOptions);
end
if(withWorst == 1)
   plot(position, max(groupData), worstOptions); 
end

end