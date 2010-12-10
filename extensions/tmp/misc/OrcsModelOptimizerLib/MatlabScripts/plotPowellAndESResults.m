function plotPowellAndESResults(powellDirectory, ESDirectory)

powellDirectory = '/home/fuchs/Desktop/Powell/';
ESDirectory = '/home/fuchs/Desktop/ES';
manualError = 0.000422691221629806;

errorbarSettings = 'ok';
bestPlotSettings = '*k';

figure;
hold all;
box on;
set(gca,'YGrid','on');

%title('Modelerror of the best from each Group');

set(gca,'XTick',[1:1:10])
set(gca,'XTickLabel', {'Manual';'Powell';'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 6';'ES 7';'ES 8'} )
axis([0 11 0 0.002]);

%set(gca,'XTick',[1:1:14])
%set(gca,'XTickLabel', {'Powell';'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 6';'ES 7';'ES 8';'ES 9';'ES 10';'ES 11';'ES 12';'ES 13'} )
%axis([0 15 0 0.002]);

ylabel('Error');

pos = 1;
plot(pos, manualError, bestPlotSettings);

pos = pos + 1;


% POWELL

data = readGroupPowellResults(powellDirectory);

errorbar(pos, mean(data), std(data), errorbarSettings);
plot(pos, min(data),bestPlotSettings);
pos = pos + 1;

% ES
for i=1:8
    ESGroup = fullfile(ESDirectory, num2str(i), 'AIMEE');
    
    data = readGroupESBestResults(ESGroup);
    
    errorbar(pos, mean(data), std(data),errorbarSettings);    
    plot(pos, min(data),bestPlotSettings);
    
    pos = pos + 1;
end



end