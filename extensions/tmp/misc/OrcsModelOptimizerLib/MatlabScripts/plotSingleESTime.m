function plotSingleESTime(directory, stepSize, timeLength, plotOptions)

data = readSingleESTime(directory, stepSize, timeLength);

plot([0:stepSize:timeLength], data, plotOptions);

end