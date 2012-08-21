clear all;
close all;
clc;



%% initiate

% pick first file

[fileName filePath] = uigetfile('*.m', 'Pick data file');
totalPath = [filePath fileName];
run(totalPath);

m = length(matrices);


% choose plots of first file

s = '';
while ~strcmp(s,'all') && (isempty(str2num(s)) || size(str2num(s),1)>1 || ...
  ~isempty(find(str2num(s)>m, 1)) || ~isempty(find(str2num(s)<1, 1)))
	s = input('Select images.     ', 's');
end
if strcmp(s,'all')
	toPlot = 1:m;
else
	toPlot = str2num(s);
end

nPlots = length(toPlot);


% initialize unification variables

for i=1:nPlots
	matricesNew{i} = matrices{toPlot(i)};
	namesNew{2*i-1} = names{toPlot(i)*2-1};
	namesNew{2*i} = names{toPlot(i)*2};
	titlesNew{i} = titles{toPlot(i)};
end

fprintf([fileName ' - ' num2str(toPlot) ' added.\n']);


% continue?

s='';
while isempty(find(s=='y'|s=='Y'|s=='n'|s=='N', 1))
	s = input('Pick another file? ', 's');
end



%% continue

while ~isempty(find(s=='y'|s=='Y', 1))

	% choose next file

	[fileName filePath] = uigetfile('*.m', 'Pick data file');
	totalPath = [filePath fileName];
	run(totalPath);

	m = length(matrices);

	% choose plots of first file

	s = '';
	while ~strcmp(s,'all') && (isempty(str2num(s)) || size(str2num(s),1)>1 || ...
	  ~isempty(find(str2num(s)>m, 1)) || ~isempty(find(str2num(s)<1, 1)))
		s = input('Select images.     ', 's');
	end
	if strcmp(s,'all')
		toPlot = 1:m;
	else
		toPlot = str2num(s);
	end

	nPlots = length(toPlot);
    
    % unify plots

	for i=1:nPlots
		matricesNew = {matricesNew{:}, matrices{toPlot(i)}};
		namesNew = {namesNew{:} names{toPlot(i)*2-1} names{toPlot(i)*2}};
		titlesNew = {titlesNew{:} titles{toPlot(i)}};
    end

    fprintf([fileName ' - ' num2str(toPlot) ' added.\n']);
    fclose(fopen(totalPath));
    
    % go on?
    
	s='';
	while isempty(find(s=='y'|s=='Y'|s=='n'|s=='N', 1))
		s = input('Pick another file? ', 's');
    end
    
end

fprintf('\nWorking...');



%% save new file

m = length(matricesNew);
[y x] = size(matricesNew{1});

% create string

strNew = 'matrices = {';
for i=1:m
	strNew = [strNew '['];
	for j=1:y
		if j<y
			strNew = [strNew num2str(matricesNew{i}(j,:)) ';'];
		else
			strNew = [strNew num2str(matricesNew{i}(j,:))];
		end
	end
	if i<m
		strNew = [strNew '],'];
	else
		strNew = [strNew ']'];
	end
end
strNew = [strNew '};\nnames = {'];
for i=1:m
	strNew = [strNew char(39) namesNew{2*i-1} char(39) ' '];
	if i<m
		strNew = [strNew char(39) namesNew{2*i} char(39) ' '];
	else
		strNew = [strNew char(39) namesNew{2*i} char(39)];
	end
end
strNew = [strNew '};\ntitles = {'];
for i=1:m
	if i<m
		strNew = [strNew char(39) titlesNew{i} char(39) ' '];
	else
		strNew = [strNew char(39) titlesNew{i} char(39)];
	end
end
strNew = [strNew '};'];

strNew = strrep(strNew,'\','\\');
strNew = strrep(strNew,';\\n',';\n');

% choose destination and filename

[saveName savePath] = uiputfile('*.m', 'Select target');
totalPath = [savePath saveName];

% save

ID = fopen(totalPath, 'w');
fprintf(ID, strNew);

fprintf(['\n\nSaved as ' saveName '\n\n']);