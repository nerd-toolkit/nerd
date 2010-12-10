function plotBestPowellTime(directory, stepSize, timeLength, plotOptions)

bestPowellDirectory = searchBestPowell(directory, stepSize, timeLength);

plotSinglePowellTime(bestPowellDirectory, stepSize, timeLength, plotOptions);

end

function bestPowellDirectory = searchBestPowell(directory, stepSize, timeLength)

bestError = bitmax;
bestPowellDirectory = '';

individualFolders = dir(directory);

for i = 3:length(individualFolders)

    individualFolder = fullfile(directory, individualFolders(i).name);

    [data stopPositions] = readSinglePowellTime(individualFolder, stepSize, timeLength);

    if(~isempty(data))

        minError = min(data);

        if(minError < bestError)
            bestError = minError;
            bestPowellDirectory = individualFolder;    
        end            
    end
end

end