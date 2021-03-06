%%%%%%%%%%%% Dynamics Plotter %%%%%%%%%%%

%% Load data matrix from file
% Loads the data matrix from a file into cell arrays. User can either pick file with a
% dialog or specify the direkt path

clc % clean command window
fprintf('\n Welcome! This is the script of the dynamics plotter in matlab.');

reply1 = ''; % empty variable to enter loop
while isempty(reply1)
    reply1 = input('\n Please specify the path to the data file or type "x" to open a dialog:\n', 's');
    if reply1 == 'x'
        [fileName filePath] = uigetfile('*.txt', 'Open data file'); % user picks file with dialog
        totalPath = [filePath fileName];   
    else             
        totalPath = (reply1); % load file from user specified path     
    end
end

fileID = fopen(totalPath);
tempL = fgetl(fileID);
j = 1;
clear t;
while ischar(tempL)  
    tempC = textscan(tempL, '%q'); % get cell array from input
    tempL = fgetl(fileID); % next line of input

    t(1, j) = tempC; 
    j = j + 1; 
end


%get axes descriptions and name from matrix
stringInfos = t{1}{1};
index = findstr(stringInfos, ';;');
if length(index) == 2
    calcName = stringInfos(1:index(1) - 1);
    xDescr = stringInfos(index(1) + 2:index(2) - 1);
    yDescr = stringInfos(index(2) + 2:end);
elseif length(index) == 3
    calcName = stringInfos(1:index(1) - 1);
    xDescr = stringInfos(index(1) + 2:index(2) - 1);
    yDescr = stringInfos(index(2) + 2:index(3));
    zDescr = stringInfos(index(3) + 2:end);
elseif isempty(index)
    calcName = stringInfos;
    xDescr = 'x';
    yDescr = 'y';
end

t{1}{1} = '0';
calcName = strrep(calcName, '_', '\_');
    
%size of data matrix                              
width = 0;
height = 0;
depth = 0;

dataStart = zeros(1, 10);



for j = 1:length(t)
    if ~isempty(strfind(t{j}{2}, '{')) %search for opening brackets { 
        depth = depth + 1;
        width = length(t{j}) - 1; 
        dataStart(depth) = j;
        t{j}{2} = strrep(t{j}{2}, '{', '');
    end
    if ~isempty(strfind(t{j}{end}, '}')) 
        height = j - dataStart(depth) + 1;
        t{j}{end} = strrep(t{j}{end}, '}', '');
    end
end

xAxis = zeros(1, width);
yAxis = zeros(1, height);
zAxis = zeros(1, depth);

for j = 2:width + 1
    xAxis(j - 1) = str2double(t{1}{j});
end
for j = 2:height + 1
   yAxis(j - 1) = str2double(t{j}{1}); 
end
for j = 1:depth - 1
   zAxis(j) = str2double(t{j * height + 1 + 1}{1}); 
end

matrix = zeros(width, height, depth);
for j = 2:width + 1
    for l = 1:depth
        for k = 1:height
            matrix(j - 1, k, l) = str2double(t{k + (l - 1) * height + 1}{j});
        end
    end
end

fclose(fileID);
% figure
if depth == 1
    imHandle = imagesc(xAxis, yAxis, matrix');
    %create color map
    cmap = rand(3, 33);
    cmap(1:3, 1:9) = [1 1 1; 0 0 0; 1 0 0; 0 0 1; 1 1 0.2; 0.6 0.6 0.5; 0 1 0; 0.3 1 0.6; 0.5 0 0]';
    cmp(1:3, 17) = [0.25 0 0]';
    colormap(cmap');
    caxis([0 32]);
    colorbar
    
else
    %for three-dimensional plot
end

title(calcName);
set(get(get(gcf,'CurrentAxes'), 'XLabel'), 'String', xDescr)
set(get(get(gcf,'CurrentAxes'), 'YLabel'), 'String', yDescr)


