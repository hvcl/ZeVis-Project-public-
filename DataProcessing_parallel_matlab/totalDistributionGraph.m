cellData=readmatrix('our_label_volume_sphe_eccen.csv'); % volume sphericity eccentricity

label=cellData(:,1);
volume=cellData(:,2) * 0.0564 * 0.0564 * 0.06;
sphericity=cellData(:,3);
eccentricity=cellData(:,4);

knownCells=readmatrix('our_known_cells_volume_sphe_eccen.csv'); % volume sphericity eccentricity


fig=figure;
subplot(3,1,1)
fig.Color=[1 1 1];
fig.Position=[100 50 1500 900];
t=histogram(volume(volume<500 & volume>0.05 & sphericity<=1.0),300,'Normalization','countdensity');
set(gca,'yscale','log')
hold on

for k=1:length(knownCells)
    value1=knownCells(k,1) * 0.0564 * 0.0564 * 0.06;
    t2=xline(value1);
    t2.Color=[0,0,1];
    t2.LineWidth=2;
end
%t2.Label='Mauthner L';


t=title('Volume');
t.FontSize=15;

%disp(['Volume: ',num2str(value1),'um (',num2str(sum(volume(:)<value1)/length(volume(:))*100),'%)']);



subplot(3,1,2);
t=histogram(sphericity(volume<500 & volume>0.05 & sphericity<=1.0),300,'Normalization','countdensity');
set(gca,'yscale','log')
hold on
for k=1:length(knownCells)
    value2=knownCells(k,2); % spheri
    t2=xline(value2);
    t2.Color=[0,0,1];
    t2.LineWidth=2;
end
%t2.Label='Mauthner L';
t=title('Sphericity');
t.FontSize=15;

%disp(['Sphericity: ',num2str(value2),' (',num2str(sum(sphericity(:)<value2)/length(sphericity(:))*100),'%)']);

subplot(3,1,3);
t=histogram(eccentricity(volume<500 & volume>0.05 & sphericity<=1.0),300,'Normalization','countdensity');
set(gca,'yscale','log')
hold on
for k=1:length(knownCells)
    value3=knownCells(k,3); % eccen
    t2=xline(value3);
    t2.Color=[0,0,1];
    t2.LineWidth=2;
end
%t2.Label='Mauthner L';
t=title('Eccentricity');
t.FontSize=15;

%disp(['Eccentricity: ',num2str(value3),' (',num2str(sum(eccentricity(:)<value3)/length(eccentricity(:))*100),'%)']);

t=knownCells;
t(:,1)=t(:,1) * 0.0564 * 0.0564 * 0.06;
[m1,ind1]=min(t);
[m2,ind2]=max(t);

num2str(sum(volume(:)<t(20,1))/length(volume)*100)

num2str(sum(volume(:)<13.7156)/length(volume)*100)
num2str(sum(volume(:)<42.1979)/length(volume)*100)

num2str(sum(volume(:)<236.775457)/length(volume)*100)


num2str(sum(sphericity(:)<0.748882)/length(sphericity)*100)

num2str(sum(eccentricity(:)<0.501100)/length(eccentricity)*100)

