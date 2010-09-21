function [min max] = findMinMaxDiff(motionDiff)

min = {'','',bitmax};
max = {'','',-1.0};


for i = 1:length(motionDiff)
    
    curDiff = motionDiff(i,:);
    
    if curDiff{3} < min{3}
       min = curDiff; 
    end
    
    if curDiff{3} > max{3};
        max = curDiff;
    end
end

fprintf('Min:\n\tGroup1: %s\n\tFile1: %s\n\n\tGroup2: %s\n\tFile2: %s\n\n\tError: %d\n\n', ...
         min{1}.group, min{1}.paraset, min{2}.group, min{2}.paraset, min{3});
     
fprintf('Max:\n\tGroup1: %s\n\tFile1: %s\n\n\tGroup2: %s\n\tFile2: %s\n\n\tError: %d\n\n', ...
         max{1}.group, max{1}.paraset, max{2}.group, max{2}.paraset, max{3});


end