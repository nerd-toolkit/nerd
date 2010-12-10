function plotPowellTime(directory)

directory='/home/fuchs/Desktop/Powell/';

% general Options
withBest = 1;

stepSize = 50;
plotLength = 10000;
plotOptions = 'k';

bestPlotOptions = ':k';

errorbarStepSize = 16;
errorbarOptions = 'k';


xLabelText = 'Simulations';
yLabelText = 'Error';

yAxisMax = 0.002;
yAxisMin = 0.0;

figure;
hold all;

%title('Powell Time Course');


xlabel(xLabelText);
ylabel(yLabelText);
axis([0 plotLength yAxisMin yAxisMax])
plotGroupPowellTime(directory, stepSize, plotLength,plotOptions, errorbarStepSize, errorbarOptions);
if(withBest == 1)
   plotBestPowellTime(directory, stepSize, plotLength,bestPlotOptions); 
end