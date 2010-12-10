function diff = calcMotionDiff

% calculates the difference between the motions
% of different parameter sets

directory = '/home/fuchs/Desktop/Validation_Parameters_one_motion';

% array with motions which should be compared
motions = {'sitUp'; 'standUp'};

% values which should be compared
compareValues = {'Simulation Data - /Sim/ASeries/Left/AnklePitch/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/AnkleYaw/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/Elbow/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/HipPitch/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/HipRoll/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/HipYaw/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/Knee/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/ShoulderPitch/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Left/ShoulderYaw/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/AnklePitch/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/AnkleYaw/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/Elbow/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/HipPitch/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/HipRoll/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/HipYaw/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/Knee/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/ShoulderPitch/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Right/ShoulderYaw/JointAngle'; ...
                 'Simulation Data - /Sim/ASeries/Waist/JointAngle'};

% get list off all parameterSets
paraSetList = createParameterSetList(directory, motions);

% get combination of every possible parameterSet
order = nchoosek(1:length(paraSetList),2);
paraSetCombinations = paraSetList(order);

diff = {};
fprintf('Number of Combinations: %d\n\n',length(paraSetCombinations));

for i=1:length(paraSetCombinations)
    
    fprintf('Calculate diff of combination: %d\n',i);
   
    diffvalue = calculateParaSetDiff(directory, paraSetCombinations{i,1}, paraSetCombinations{i,2}, motions, compareValues);
    
    diffEntry = {paraSetCombinations{i,1}, paraSetCombinations{i,2}, diffvalue}; 
    
    diff = cat(1,diff,diffEntry); 
    
end


end


function parameterSetList = createParameterSetList(directory, motions)

parameterSetList = {};

search = char( strcat('*', motions(1), '*.stat') ); 

groupFolders = dir(directory); 

for j=3:length(groupFolders)
    
    curGroupFolder = groupFolders(j).name;

    files = dir(fullfile(directory,curGroupFolder,search)); 
    
    for i=1:length(files)

        curFileName = files(i).name;

        tokens = regexp(curFileName, char( strcat(motions(1),'_(?<paraSetName>.*).stat') ), 'tokens');

        curData = struct('group', curGroupFolder, 'paraset',char(tokens{:}) );
        
        parameterSetList = cat(1, parameterSetList, curData);
    end
end
end


function diffvalue = calculateParaSetDiff(directory, paraSetOne, paraSetTwo, motions, compareValues)

diffvalue = 0;

for i=1:length(motions)
    
    % find first file
    searchName = strcat('*',motions(i),'*',paraSetOne.paraset,'*');
    
    files = dir(fullfile(directory, paraSetOne.group, char(searchName)));
    
    if(length(files) ~= 1)
        error('Could not find file for ParaSet %s and motion %s', paraSetOne.paraset, char(motions(i)));
    end
    
    paraSetOneFile = fullfile(paraSetOne.group,files.name);
        
    %find second file
    
    searchName = strcat('*',motions(i),'*',paraSetTwo.paraset,'*');
    
    files = dir(fullfile(directory, paraSetTwo.group, char(searchName)));
    
    if(length(files) ~= 1)
        error('Could not find file for ParaSet %s and motion %s', paraSetTwo.paraset, char(motions(i)));
    end
    
    paraSetTwoFile = fullfile(paraSetTwo.group,files.name);


    % calculte diff for files
    diffvalue = diffvalue + calculateMotionStatDiff(directory, paraSetOneFile, paraSetTwoFile, compareValues);
    
end

diffvalue = diffvalue / length(motions);

end