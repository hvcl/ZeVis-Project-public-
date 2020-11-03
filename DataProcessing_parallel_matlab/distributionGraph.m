labelPath='E:/Zevis/BrainLabel_60nm/';

cellData=readmatrix('our_label_volume_sphe_eccen.csv'); % volume sphericity eccentricity

label=cellData(:,1);
volume=cellData(:,2) * 0.0564 * 0.0564 * 0.06;
sphericity=cellData(:,3);
eccentricity=cellData(:,4);

knownCells=readmatrix('our_known_cells_volume_sphe_eccen.csv'); % volume sphericity eccentricity

k=4;

value1=knownCells(k,1) * 0.0564 * 0.0564 * 0.06;
value2=knownCells(k,2); % spheri
value3=knownCells(k,3); % eccen

value1=42.1979-(70.6799-42.1976);

fig=figure;

%subplot(3,1,1)
fig.Color=[1 1 1];
fig.Position=[100 50 1500 900];
t=histogram(volume(volume<500 & volume>0.05 & sphericity<=1.0),300,'Normalization','countdensity');
%set(gca,'yscale','log')
hold on
t2=xline(value1);
t2.Color=[1,0,0];
t2.LineWidth=2;
%t2.Label='Mauthner L';
t=title('Volume');
t.FontSize=15;

disp(['Volume: ',num2str(value1),'um (',num2str(sum(volume(:)<value1)/length(volume(:))*100),'%)']);


t2=xline(42.1979);
t2.Color=[0,0,1];
t2.LineWidth=2;

subplot(3,1,2);
t=histogram(sphericity(volume<500 & volume>0.05 & sphericity<=1.0),300,'Normalization','countdensity');
set(gca,'yscale','log')
hold on
t2=xline(value2);
t2.Color=[1,0,0];
t2.LineWidth=2;
%t2.Label='Mauthner L';
t=title('Sphericity');
t.FontSize=15;

disp(['Sphericity: ',num2str(value2),' (',num2str(sum(sphericity(:)<value2)/length(sphericity(:))*100),'%)']);

subplot(3,1,3);
t=histogram(eccentricity(volume<500 & volume>0.05 & sphericity<=1.0),300,'Normalization','countdensity');
set(gca,'yscale','log')
hold on
t2=xline(value3);
t2.Color=[1,0,0];
t2.LineWidth=2;
%t2.Label='Mauthner L';
t=title('Eccentricity');
t.FontSize=15;

disp(['Eccentricity: ',num2str(value3),' (',num2str(sum(eccentricity(:)<value3)/length(eccentricity(:))*100),'%)']);



