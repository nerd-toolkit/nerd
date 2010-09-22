function diff = calculateMotionStatDiff(directory, fileOne, fileTwo, parameter)

diff = 0;

% read data from fileOne
[labels, dataOne] = readOrcsStatisticsValues(fullfile(directory, fileOne),parameter,1);

if(length(labels) ~= length(parameter))
   error('Number of found parameters (#%d) in file %s is unequal to number of provided parameters (#%d) !',length(labels), fileOne, length(parameter)); 
end

% read data from file two
[labels, dataTwo] = readOrcsStatisticsValues(fullfile(directory, fileTwo),parameter,1);

if(length(labels) ~= length(parameter))
   error('Number of found parameters (#%d) in file %s is unequal to number of provided parameters (#%d) !',length(labels), fileTwo, length(parameter)); 
end

% check data length

if(length(dataOne(:,1)) ~= length(dataTwo(:,1)))
   error('Length of the two files %s and %s is different!',fileOne, fileTwo); 
end


% calculate difference between data 

motionDiff = 0;

for time=1:length(dataOne(:,1))
    
    paraDiff = 0;
    
    for para=1:length(dataOne(1,:))
       
        paraDiff = paraDiff + ( 0.5 * (dataOne(time,para) - dataTwo(time,para))^2 );
        
    end
    
    paraDiff = paraDiff / length(dataOne(1,:));
    
    motionDiff = motionDiff + paraDiff;
end

diff = motionDiff / length(dataOne(:,1));

end