dim = get(gcf, 'Position');
w = dim(3);
h = dim(4);

set(gcf,'PaperUnits','points');
set(gcf,'PaperSize',round([0.7*w 0.7*h+0.05*w]));
set(gcf,'PaperPosition',[0 0 get(gcf,'PaperSize')]);
