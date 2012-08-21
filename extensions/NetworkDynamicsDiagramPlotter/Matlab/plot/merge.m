clear all;
close all;
clc;



%% get image data

% select background file

[fileName filePath] = uigetfile('*.m', 'Pick background data file');
totalPath = [filePath fileName];
run(totalPath);

m = length(matrices);
sizeBG = size(matrices{1});

% select background plot

s = '';
while isempty(str2num(s)) || length(str2num(s))>1 || ...
        ~isempty(find(str2num(s)>m, 1)) || ~isempty(find(str2num(s)<1, 1))
	s = input('Select background image.     ', 's');
end
bgIndex = str2num(s);
bgMatrix = matrices{bgIndex};

newNames = {names{2*bgIndex-1} names{2*bgIndex}};
newTitle = titles{bgIndex};

% select foreground file

[fileName filePath] = uigetfile('*.m', 'Pick foreground data file');
totalPath = [filePath fileName];
run(totalPath);

m = length(matrices);
sizeFG = size(matrices{1});

if sum(sizeBG~=sizeFG)>0
    fprintf('\nMatrix dimension mismatch.\n');
    break;
end

% select foreground plot

s = '';
while isempty(str2num(s)) || length(str2num(s))>1 || ...
        ~isempty(find(str2num(s)>m, 1)) || ~isempty(find(str2num(s)<1, 1))
	s = input('Select foreground image.     ', 's');
end
fgIndex = str2num(s);
fgMatrix = matrices{fgIndex};

% specify transparent value

s = '';
while isempty(str2num(s)) || length(str2num(s))>1 || ...
        str2num(s)>max(max(fgMatrix(1:end-1,2:end))) || ...
        str2num(s)<min(min(fgMatrix(1:end-1,2:end)))
    s = input('Specify transparent value of foreground image.     ', 's');
end
tValue = str2num(s);



%% merge plots

vMax = max(max(bgMatrix(1:end-1,2:end)));
vMin = min(min(bgMatrix(1:end-1,2:end)));

newMatrix = bgMatrix;

for i=1:sizeBG(1)-1
    for j=2:sizeBG(2)
        if fgMatrix(i,j)~=tValue
            if newMatrix(i,j) < vMax/2
                newMatrix(i,j) = vMax - (vMax==tValue);
            else
                newMatrix(i,j) = vMin + (vMin==tValue);
            end
        end
    end
end



%% save new file

% create string

strNew = 'matrices = {[';

for i=1:sizeBG(1)
	if i<sizeBG(1)
		strNew = [strNew num2str(newMatrix(i,:)) ';'];
	else
		strNew = [strNew num2str(newMatrix(i,:))];
	end
end

strNew = [strNew ']};\nnames = {' char(39) newNames{1} char(39) ' ' ...
    char(39) newNames{2} char(39) '};\ntitles = {' ...
    char(39) newTitle char(39) '};'];

strNew = strrep(strNew,'\','\\');
strNew = strrep(strNew,';\\n',';\n');

% choose destination and filename

[saveName savePath] = uiputfile('*.m', 'Select target');
totalPath = [savePath saveName];

% save

ID = fopen(totalPath, 'w');
fprintf(ID, strNew);

fprintf(['\nSaved as ' saveName '\n\n']);
