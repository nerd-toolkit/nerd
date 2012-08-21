clear all;
close all;
clc;



%% fixed parameters

totalWidth = 1000;	% constant figure width in px
axisRes = 5;		% number of visible axis values for a 1-plot full-window figure



%% pick data file

[fileName filePath] = uigetfile('*.m', 'Open data file');
totalPath = [filePath fileName];
run(totalPath);

% get data dimensions
m = length(matrices);
res = size(matrices{1})-1;
resY = res(1);
resX = res(2);



%% user parameters

% images to plot
s = '';
while ~strcmp(s,'all') && (isempty(str2num(s)) || size(str2num(s),1)>1 || ...
  ~isempty(find(str2num(s)>m, 1)) || ~isempty(find(str2num(s)<1, 1)))
	s = input('Choose images to plot. \n\n    ', 's');
end
if strcmp(s,'all')
	toPlot = 1:m;
else
	toPlot = str2num(s);
end

nPlots = length(toPlot);

fprintf('\n');

% colormap
s = '';
while ~strcmp(s,'0') && ~strcmp(s,'1') && ~strcmp(s,'2')
	s = input('Choose a color map.    0 - w/b/r    1 - gray    2 - color \n\n    ', 's');
end
if strcmp(s,'0')
	cFlag = 0;
elseif strcmp(s,'1')
	cFlag = 1;
else
	cFlag = 2;
end

fprintf('\n');

s = '';
while ~strcmp(s,'0') && ~~isempty(str2num(s)==1)
	s = input('Choose an upper limit for color scaling or type "0" for auto fitting. \n\n    ', 's');
end
if strcmp(s,'0')
    singleMax = zeros(1,nPlots);
	for i=1:nPlots
		singleMax(i) = max(max(matrices{toPlot(i)}(1:end-1,2:end)));
	end
	totalMax = max(singleMax);
else
	totalMax = str2num(s);
end

fprintf('\n');

% arrangement
s = '';
while isempty(str2num(s)) || str2num(s)>nPlots
	s = input('In how many rows do you want the images to be arranged? \n\n    ', 's');
end
nRows = str2num(s);

fprintf('\n');

% titles, numerations, colorbar
numFlag = 0;
s = '';
while isempty(find(s=='y'|s=='Y'|s=='n'|s=='N', 1))
	s = input('Numerate images? \n\n    ', 's');
end
if ~isempty(find(s=='y'|s=='Y', 1))
	numFlag = 1;
end
fprintf('\n');
titleFlag = 0;
s = '';
while isempty(find(s=='y'|s=='Y', 1)) && isempty(find(s=='n'|s=='N', 1))
	s = input('Display image titles? \n\n    ', 's');
end
if ~isempty(find(s=='y'|s=='Y', 1))
	titleFlag = 1;
end
fprintf('\n');
cbFlag = 0;
s = '';
while isempty(find(s=='y'|s=='Y'|s=='n'|s=='N', 1))
	s = input('Show colorbar? \n\n    ', 's');
end
if ~isempty(find(s=='y'|s=='Y', 1))
	cbFlag = 1;
end
fprintf('\n');



%% plot

nCols = ceil(nPlots/nRows);
if nPlots==1
    totalHeight = round(totalWidth/2)+1;
else
    totalHeight = round(totalWidth*resY/resX*nRows/nCols)+1;
end

figure('Name', [fileName(1:find(fileName=='.')-1) '_' strrep(num2str(toPlot),'  ','')], ...
       'Position', [50 250 totalWidth totalHeight]);

nTicks = max(2,round(axisRes/max(2,nCols)));

stepX = (resX-1)/(nTicks-1);
stepY = (resY-1)/(nTicks-1);
stepXLabel = (resX-1)/(nTicks-1);
stepYLabel = (resY-1)/(nTicks-1);

for i=1:nPlots
	subplot(nRows,nCols,i);
	imagesc(matrices{toPlot(i)}(1:end-1,2:end),[0 totalMax]);
	set(gca, 'XTick', 1:stepX:resX, 'XLim', [0 resX+1], 'XTickLabel', ...
        round(10*matrices{toPlot(i)}(end,round(2:stepXLabel:resX+1)))./10);
	set(gca, 'YTick', 1:stepY:resY, 'YLim', [0 resY+1], 'YTickLabel', ...
        round(10*matrices{toPlot(i)}(round(1:stepYLabel:resY),1))./10);
    set(gca,'DataAspectRatio',[1 1 1]);
    xlabel(names{toPlot(i)*2-1}, 'FontName', 'Times New Roman', 'FontSize', 12);
	ylabel(names{toPlot(i)*2}, 'FontName', 'Times New Roman', 'FontSize', 12);
	thisTitle = '';
	if numFlag
		thisTitle = ['(' char(96+i) ') '];
	end
	if titleFlag
		thisTitle = [thisTitle titles{toPlot(i)}];
	end
	title(thisTitle, 'FontWeight', 'bold');
end

if cFlag==0
	colormap([1 1 1; 0 0 0; 1 0 0]);
elseif cFlag==1
	colormap(flipud(gray(totalMax+1)));
else
	colormap([1 1 1; jet(totalMax)]);
end

if cbFlag==1
    cb = colorbar('Position',[0.95 0.25 0.025 0.5], ...
        'YTick',totalMax/(2*(totalMax+1)):totalMax/(totalMax+1):totalMax, ...
        'YLim',[0 totalMax],'YTickLabel',0:totalMax);
end

set(gcf,'PaperUnits','points');
set(gcf,'PaperSize',round([0.7*totalWidth 0.7*totalHeight+0.05*totalWidth]));
set(gcf,'PaperPosition',[0 0 get(gcf,'PaperSize')]);
