function plotQualitativeResults


data = [4 1
        3 1
        1 1
        3 1
        6 0
        4 3
        1 0
        9 0
        8 2];

%data = [4 1
%        8 2
%        9 0
%        1 0
%        4 3
%        6 0
%        3 1
%        3 1
%        3 1];

figure;
hold on;

ylabel('Number of Successful Parameter Sets');
width = 0.3;

set(gca,'XTick',[1:1:9])
set(gca,'XTickLabel', {'Powell';'ES 1'; 'ES 2';'ES 3';'ES 4';'ES 5';'ES 6';'ES 7';'ES 8';} )
%set(gca,'YTickLabel', {'ES 8';'ES 7';'ES 6';'ES 5';'ES 4';'ES 3';'ES 2';'ES 1';'Powell'} )
axis([0 10 0 10])
box off;

bar1 = bar(data, width, 'stack');

set(bar1(1),'FaceColor',[0.502 0.502 0.502],'DisplayName',      'only motion "Stand Up" correct');
set(bar1(2),'FaceColor',[0.9412 0.9412 0.9412],'DisplayName',   'both motions correct' );
legend ('only motion "Stand Up" correct','both motions correct',2) ;
hold off;

end