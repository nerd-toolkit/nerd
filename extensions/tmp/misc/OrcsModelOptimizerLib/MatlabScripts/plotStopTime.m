function plotStopTime(PowellDirectory, ESDirectory)


PowellDirectory = '/home/fuchs/Desktop/Powell/';
ESDirectory = '/home/fuchs/Desktop/ES/';

figure;
hold on;
box on;

% general settings
%title('Average needed Simulations');
ylabel('Simulations');
set(gca,'YGrid','on');
errorbarSettings = 'ok';
timeLength = 10000;

set(gca,'XTick',[1:1:9])
set(gca,'XTickLabel', {'Powell';'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 6';'ES 7';'ES 8';} )
axis([0 10 0 11000])

pos = 1;
plotGroupPowellStopTime(PowellDirectory, pos, errorbarSettings);

pos = pos + 1;

for i=1:8
    ESGroup = fullfile(ESDirectory, num2str(i), 'AIMEE');
    
    plotGroupESStopTime(ESGroup, timeLength, pos, errorbarSettings);
    
    pos = pos + 1;
end

hold off;
end