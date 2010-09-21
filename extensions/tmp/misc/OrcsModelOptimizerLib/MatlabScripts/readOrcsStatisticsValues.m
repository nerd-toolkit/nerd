function [labels, x] = readOrcsStatisticsValues(fileName, values, exactMatch)

if(nargin == 2)
   exactMatch = 0;
end

labels = {};
pos = [];
x = [];

if ischar(values) == 1
   values = {values}; 
end

fid = fopen(fileName);
if fid < 0
   error(['Could not open ',fileName,' for input!']);
end

%%

%read value names
strLine = fgetl(fid);
curPos = 1;
while ~isempty(strLine) && strLine(1) == '#'
    
    % go through all values which should be read out
    for i = 1:length(values)
       
        found = 0;
        
        if(exactMatch == 0)
            if ~isempty( strfind(strLine, values{i} )) 
                found = 1;
            end
        else
            if(strcmp(values{i}, strLine(3:length(strLine))))
               found = 1; 
            end
        end
        
        if found == 1
            labels = [labels strLine(2:length(strLine))];
            pos = [pos curPos];
        end
    end
    curPos = curPos + 1;
    strLine = fgetl(fid);
end

%%

% read values

% loop through empty lines
while ischar(strLine)
    if(~isempty(strLine))
        break;
    end
    
    strLine = fgetl(fid);
end

if(~isempty(strLine))
    curValues = regexp(strLine, ';', 'split');
    
    %curValues = sscanf(strLine, '%f;');
end
while ~isempty(curValues)
    
    tmp = [];
    for i = 1:length(pos)
        tmp = [tmp str2double(curValues(pos(i)))];
    end
    
    x = [x tmp'];
    
    
    strLine = fgetl(fid);
    curValues = {};
    if(ischar(strLine) )
        %curValues = sscanf(strLine, '%f;');
        curValues = regexp(strLine, ';', 'split');
    end
end

x = x';

fclose(fid);

end