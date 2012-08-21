clear all;
close all;
clc;



%% get attractor data

% select file

[fileName filePath] = uigetfile('*.m', 'Pick data file');
totalPath = [filePath fileName];
run(totalPath);

m = length(matrices);
mSize = size(matrices{1});

% select plot

s = '';
while isempty(str2num(s)) || length(str2num(s))>1 || ...
        ~isempty(find(str2num(s)>m, 1)) || ~isempty(find(str2num(s)<1, 1))
	s = input('Select attractor plot.     ', 's');
end
index = str2num(s);
matrix = matrices{index};

newNames = {names{2*index-1} names{2*index}};
newTitle = titles{index};

% specify transparent value

s = '';
while isempty(str2num(s)) || length(str2num(s))>1 || ...
        str2num(s)>max(max(matrix(1:end-1,2:end))) || ...
        str2num(s)<min(min(matrix(1:end-1,2:end)))
    s = input('Specify transparent value.     ', 's');
end
tValue = str2num(s);



%% extrapolate attractors

tmpMatrix = tValue*ones(mSize(1)+3,mSize(2)+3);
tmpMatrix(3:end-2,3:end-2) = matrix(1:end-1,2:end);

for i=1:mSize(1)-1
    for j=1:mSize(2)-1
        if matrix(i,j+1)~=tValue
            v = matrix(i,j+1);
            for k=[-1,1]
                for l=[-1,1]
                    tmpMatrix(i+2+k,j+2+l) = v;
                    tmpMatrix(i+2+2*k,j+2+2*l) = v;
                end
            end
        end
    end
end

matrix(1:end-1,2:end) = tmpMatrix(3:end-2,3:end-2);



%% save new file

% create string

strNew = 'matrices = {[';

for i=1:mSize(1)
	if i<mSize(1)
		strNew = [strNew num2str(matrix(i,:)) ';'];
	else
		strNew = [strNew num2str(matrix(i,:))];
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
