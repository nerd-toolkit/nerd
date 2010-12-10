function plotSuccessfulES(directory, position, border, width, color)

data = readGroupESBestResults(directory);

successfull = 0;

for i = 1:length(data)
    if(data(i) <= border)
        successfull = successfull + 1;
    end 
end

bar(position, successfull, width, color);

end