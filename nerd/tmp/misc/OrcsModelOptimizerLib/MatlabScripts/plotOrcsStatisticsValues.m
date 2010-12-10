function dataLength = plotOrcsStatisticsValues(fileName, values, exactMatch, plotOptions)

[labels, x] = readOrcsStatisticsValues(fileName, values, exactMatch);

plot(x, plotOptions);

%axis([0,length(x), 0, 1]);
%xlabel('Simulationsteps');
%legend(labels);

dataLength = length(x);

end