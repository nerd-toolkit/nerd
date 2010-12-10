function [timeData minTime] = readSingleESTime(ESFolder, stepSize, timeLength)

timeData = [];
minTime = -1;
statFile = dir(fullfile(ESFolder,'*Statistics.log')); 

if (~isempty(statFile))

    [labels, data] = readOrcsStatisticsValues(fullfile(ESFolder, statFile(1).name),{ 'SimulationRuns'; 'SmallestError' });

    % sicherstellen, dass daten in richtiger reihenfolge sind
    % falls nicht werden sie umformatiert

    if(length(labels) ~= 2)
        disp('Error: Not correct amount of data read!');
    end

    if(strfind(labels{1}, 'SimulationRuns') <= 0)
        data = [data(:,2); data(:,1)];
    end


    % create vector which has correct timestep format
    sourcePos = 1;                  % current pos in data
    minValue = data(sourcePos,2);   % minimum value found so far in data
    for t=0:stepSize:timeLength

        if(sourcePos <= length(data(:,1)))

            % go trough sourcepos till timepoint which is wanted
            % save minValue found so far
            while( sourcePos <= length(data(:,1))  && data(sourcePos,1) <= t)
                sourcePos = sourcePos + 1;
                    if(sourcePos <= length(data(:,1)))
                        
                        if(data(sourcePos,2) < minValue)
                           minValue = data(sourcePos,2);
                           minTime = data(sourcePos,1);
                        end
                    end
            end

        end

        timeData = [timeData minValue];
    end
end