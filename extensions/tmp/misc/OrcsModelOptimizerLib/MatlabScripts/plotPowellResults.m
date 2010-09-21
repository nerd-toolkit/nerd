function plotPowellResults(bestDirectory, allGroupsDirectory)

bestDirectory = '/home/fuchs/Desktop/Powell_01/Best';
allGroupsDirectory = '/home/fuchs/Desktop/Powell_01/Groups';

bestGroupIndividuals = readGroupPowellResults(bestDirectory);

meanBestGroupIndividuals = mean(bestGroupIndividuals);
stdBestGroupIndividuals = std(bestGroupIndividuals);

allIndividuals = readAllPowellResults(allGroupsDirectory);

meanAllIndividuals = mean(allIndividuals);
stdAllIndividuals = std(allIndividuals);

figure;
hold all;
box on;

set(gca,'YGrid','on');

%general plott settings
set(gca,'XTick',[1 2])
set(gca,'XTickLabel', {'Best Group Individuals (n=10)'; 'All Individuals(n=100)'} )


errorbar(1, meanBestGroupIndividuals, stdBestGroupIndividuals, 'ok');
%hold
plot(1, min(bestGroupIndividuals),'*k');

%hold
errorbar(2, meanAllIndividuals, stdAllIndividuals, 'ok');
%hold
plot(2, min(allIndividuals),'*k');
%hold


end