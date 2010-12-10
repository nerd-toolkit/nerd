function plotAllMotionDiff(diffData)

errorbarSettings = 'ok';
worstPlotSettings = '*k';
withWorst = 1;

figure;
hold on;
box on;
set(gca,'YGrid','on');

set(gca,'XTick',[1:1:9])
set(gca,'XTickLabel', {'All';'Powell';'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 7';'ES 8'} )
axis([0 10 0 0.0006]);
ylabel('Error');

pos = 1;

% ALL

diffDataVec = cell2mat(diffData(:,3));
errorbar(pos, mean(diffDataVec), std(diffDataVec), errorbarSettings);
if(withWorst == 1)
   plot(pos, max(diffDataVec), worstPlotSettings); 
end

pos = pos + 1;

% POWELL

plotGroupMotionDiff(diffData, 'Powell', pos, errorbarSettings, withWorst,worstPlotSettings);
pos = pos + 1;

% ES
for i=1:8
    
    if(i ~= 6)
        
        plotGroupMotionDiff(diffData, num2str(i), pos, errorbarSettings, withWorst,worstPlotSettings);
    
        pos = pos + 1;
    end
end

hold off;

end