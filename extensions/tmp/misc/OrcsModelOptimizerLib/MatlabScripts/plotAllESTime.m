function plotAllESTime

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


% helper variables
subplotX = 4;
subplotY = 2;
subplotPos = 1;

%%
directory = '/home/fuchs/Desktop/ES/1/AIMEE';
titlename = 'ES 1';


plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/2/AIMEE';
titlename = 'ES 2';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/3/AIMEE';
titlename = 'ES 3';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/4/AIMEE';
titlename = 'ES 4';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/5/AIMEE';
titlename = 'ES 5';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/6/AIMEE';
titlename = 'ES 6';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/7/AIMEE';
titlename = 'ES 7';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

%%
directory = '/home/fuchs/Desktop/ES/8/AIMEE';
titlename = 'ES 8';

plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
            directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest);
subplotPos = subplotPos + 1;

end

function plotSubPlot(subplotX,subplotY,subplotPos,titlename,xLabelText,yLabelText,plotLength, yAxisMin, yAxisMax, ...
                     directory, stepSize,plotOptions, errorbarStepSize, errorbarOptions, bestPlotOptions,withBest)

h = subplot(subplotX,subplotY,subplotPos);

p = get(h,'pos');

p(2) = p(2) - 0.025;
p(4) = p(4) - 0.05;
set(h,'pos',p);

hold on;
title(titlename);
xlabel(xLabelText);
ylabel(yLabelText);
axis([0 plotLength yAxisMin yAxisMax])
plotGroupESTime(directory, stepSize, plotLength,plotOptions, errorbarStepSize, errorbarOptions);
if(withBest == 1)
   plotBestESTime(directory, stepSize, plotLength,bestPlotOptions); 
end
hold off;

end