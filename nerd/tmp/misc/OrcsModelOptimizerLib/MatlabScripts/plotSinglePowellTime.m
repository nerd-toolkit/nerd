function plotSinglePowellTime(directory, stepSize, timeLength, plotOptions)

[data stop] = readSinglePowellTime(directory, stepSize, timeLength);

stopPosition = fix(stop / stepSize) + 1;

plot([0:stepSize:((stopPosition * stepSize)-1)], data(1:stopPosition), plotOptions);

end