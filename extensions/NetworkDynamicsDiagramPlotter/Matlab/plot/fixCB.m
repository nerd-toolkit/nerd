cb = findobj(gcf,'tag','Colorbar');
set(cb,'YTick',totalMax/(2*(totalMax+1)):totalMax/(totalMax+1):totalMax, ...
'YLim',[0 totalMax],'YTickLabel',0:totalMax);
