function plotSuccessfulBests(PowellDirectory, ESDirectory)

PowellDirectory = '/home/fuchs/Desktop/Powell/';
ESDirectory = '/home/fuchs/Desktop/ES/';

figure;
hold all;

% general settings
ylabel('Successfull Parameter Sets');
color = 'g';
width = 0.3;
border = 0.0005;

set(gca,'XTick',[1:1:9])
set(gca,'XTickLabel', {'Powell';'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 6';'ES 7';'ES 8';} )
axis([0 10 0 10])
box off;

pos = 1;
plotSuccessfulPowell(PowellDirectory, pos, border, width, color);
pos = pos + 1;

for i=1:8
    ESGroup = fullfile(ESDirectory, num2str(i), 'AIMEE');
    
    plotSuccessfulES(ESGroup, pos, border, width, color);
    
    pos = pos + 1;
end



end