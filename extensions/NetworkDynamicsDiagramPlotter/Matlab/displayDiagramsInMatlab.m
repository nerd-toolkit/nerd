clear all;
close all;
clc;



%% fixed parameters

totalWidth = 800;	% constant figure width in px
gap = 0.2;		% plot-to-plot distance factor
axisRes = 15;		% number of visible axis values for a 1-plot figure



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
while ~strcmp(s,'all') & (isempty(str2num(s)) | size(str2num(s),1)>1 | ...
  ~isempty(find(str2num(s)>m)) | ~isempty(find(str2num(s)<1)))
	s = input('Chose images to plot. \n\n    ', 's');
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
while ~strcmp(s,'1') & ~strcmp(s,'2')
	s = input('Choose a color map.    1 - gray    2 - color \n\n    ', 's');
end
if strcmp(s,'1')
%	cMap = flipud(gray);
	cFlag = 1;
else
%	cMap = [1 1 1; jet];
	cFlag = 2;
end
%close;

fprintf('\n');

s = '';
while ~strcmp(s,'0') & ~length(str2num(s)==1)
	s = input('Choose an upper limit for color scaling or type "0" for auto fitting. \n\n    ', 's');
end
if strcmp(s,'0')
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
while isempty(str2num(s)) | str2num(s)>nPlots
	s = input('In how many rows do you want the images to be arranged? \n\n    ', 's');
end
nRows = str2num(s);

fprintf('\n');

% titles & numerations
numFlag = 0;
s = '';
while isempty(find(s=='y'|s=='Y'|s=='n'|s=='N'))
	s = input('Numerate images? \n\n    ', 's');
end
if ~isempty(find(s=='y'|s=='Y'))
	numFlag = 1;
end
fprintf('\n');
titleFlag = 0;
s = '';
while isempty(find(s=='y'|s=='Y')) & isempty(find(s=='n'|s=='N'))
	s = input('Display image titles? \n\n    ', 's');
end
if ~isempty(find(s=='y'|s=='Y'))
	titleFlag = 1;
end

fprintf('\n');



%% plot

nCols = ceil(nPlots/nRows);
totalHeight = totalWidth * resY/resX * nRows/nCols;

figure('Name', [fileName(1:find(fileName=='.')-1) '_' strrep(num2str(toPlot),'  ','')], ...
       'Position', [50 50 totalWidth totalHeight]);

stepX = ceil(resX/axisRes*nCols);
stepY = ceil(resY/axisRes*nCols);

for i=1:nPlots
	subplot(nRows,nCols,i);
	imagesc(matrices{toPlot(i)}(1:end-1,2:end),[0 totalMax]);
	set(gca, 'DataAspectRatio', [1 1 1]);
	set(gca, 'XTick', 1:stepX:resX, 'XLim', [1 resX], 'XTickLabel', round(10*matrices{toPlot(i)}(end,2:stepX:end))./10);
	set(gca, 'YTick', stepY:stepY:resY, 'YLim', [1 resY], 'YTickLabel', round(10*matrices{toPlot(i)}(stepY:stepY:end-1,1))./10);
	xlabel(names{toPlot(i)*2-1});
	ylabel(names{toPlot(i)*2});
	thisTitle = '';
	if numFlag
		thisTitle = ['(' char(96+i) ') '];
	end
	if titleFlag
		thisTitle = [thisTitle titles{toPlot(i)}];
	end
	title(thisTitle);
end

%colormap(cMap);

if cFlag==1
	colormap(flipud(gray(totalMax+1)));
else
	colormap([1 1 1; jet(totalMax)]);
end

colorbar('Position',[0.95 0.25 0.025 0.5],'YTick',totalMax/(2*(totalMax+1)):totalMax/(totalMax+1):totalMax,'YLim',[0 totalMax],'YTickLabel',0:totalMax);

