function plotESResults(directory)

directory = '/home/fuchs/Desktop/ES';

data = [];

ESConfigs = dir(directory);
for i = 3:length(ESConfigs)
    
    groupFolder = fullfile(directory, ESConfigs(i).name, 'AIMEE');
   
    data = [data ; readGroupESBestResults(groupFolder)];
end


figure;
hold all;
box on;

set(gca,'YGrid','on');

%general plott settings
set(gca,'XTick',[1:1:length(data(:,1))])
set(gca,'XTickLabel', {'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 6';'ES 7';'ES 8';} )
ylabel('Error');


for i = 1:length(data(:,1))
    
    errorbar(i, mean(data(i,:)), std(data(i,:)), 'ok');
    
    
    %for j= 1:length(data(i,:))  
    %    plot(i, data(i,j),'*k');
    %end
    
    plot(i, min(data(i,:)),'*k');
end


end