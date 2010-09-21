function plotBestESTime(directory, stepSize, timeLength, plotOptions)

bestESDirectory = searchBestES(directory);

plotSingleESTime(bestESDirectory, stepSize, timeLength, plotOptions);

end

function bestESDirectory = searchBestES(directory)

bestError = bitmax;
bestESDirectory = '';

individualFolders = dir(directory);
    
for j = 3:length(individualFolders)

    individualFolder = fullfile(directory, individualFolders(j).name);

    statFile = dir(fullfile(individualFolder,'*Statistics.log')); 
    
    if (~isempty(statFile))
        
        [labels, data] = readOrcsStatisticsValues(fullfile(individualFolder, statFile(1).name), 'SmallestError');

        minError = min(data);
        
        if(minError < bestError)
            bestError = minError;
            bestESDirectory = individualFolder;    
        end
    end
end

end