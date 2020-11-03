labelPath='result_data_200101_60nm_median/Brain/XY/';
emPath='result_data_200101_60nm_median/EM/XY/';

resultPath='result_data_200101_60nm_median/Brain/MorphologicalFeature_Clean/';

%MaxCellIndex=999150;

files=dir([labelPath,'*1.block']);
fileNum=size(files,1);

tCellData=cell(fileNum,1);

EM=zeros(512*3,512*3,256*3,'uint8');
Label=zeros(512*3,512*3,256*3,'uint32');
%refinedLabel=zeros(512*3,512*3,256*3,'uint32');

%cluster = parcluster;
%parfor(i=1:fileNum,2)
for i=1:fileNum
    disp([num2str(i),': ',files(i).name]);

    curFile=[labelPath,files(i).name];
    t=str2double(split(files(i).name,["_","."]));
    Label(:)=0;
    EM(:)=0;
    %refinedLabel(:)=0;
    for dz=-1:1
        for dy=-1:1
            for dx=-1:1
                tName=[labelPath,num2str(t(1)+dz),'_',num2str(t(2)+dy),'_',num2str(t(3)+dx),'_1.block'];
                if isfile(tName)
                    tfileID = fopen(tName,'r');
                    tdata=uint32(reshape(fread(tfileID,'uint32'),512,512,512));
                    fclose(tfileID);
                    Label(512*(1+dx)+1:512*(2+dx),512*(1+dy)+1:512*(2+dy),256*(1+dz)+1:256*(2+dz))=tdata(:,:,1:2:512);  
                    
                    tName=[emPath,num2str(t(1)+dz),'_',num2str(t(2)+dy),'_',num2str(t(3)+dx),'_1.block'];
                    tfileID = fopen(tName,'r');
                    tdata=uint8(reshape(fread(tfileID,'uint8'),512,512,512));
                    fclose(tfileID);
                    EM(512*(1+dx)+1:512*(2+dx),512*(1+dy)+1:512*(2+dy),256*(1+dz)+1:256*(2+dz))=tdata(:,:,1:2:512);
                end              
            end
        end
    end
    disp([num2str(i),': data load finish']);
%    Label(400:1200,400:1200,200:600)=medfilt3(Label(400:1200,400:1200,200:600),[5,5,5]);
    processedLabel=Label(512*1+1:512*2,512*1+1:512*2,256*1+1:256*2);
    labels=unique(processedLabel(processedLabel(:)~=0));
    disp([num2str(i),': data refine start']);

    [~,refinedLabel] = ismember(Label,labels);
    refinedLabel=uint32(refinedLabel);
    
%    len=length(labels);
%    for l=1:len
%        tIndex=Label(512+1-100:512*2+100,512+1-100:512*2+100,256+1-100:256*2+100)==labels(l);
%        refinedLabel(512+1-100:512*2+100,512+1-100:512*2+100,256+1-100:256*2+100)=refinedLabel(512+1-100:512*2+100,512+1-100:512*2+100,256+1-100:256*2+100)+uint32(tIndex*l);
%    end
    disp([num2str(i),': data refine finish']);
%    stats = regionprops3(refinedLabel,EM,'Centroid','PrincipalAxisLength','Solidity','SurfaceArea','Volume','MeanIntensity');
    stats = regionprops3(refinedLabel,EM,'BoundingBox','Centroid','PrincipalAxisLength','SurfaceArea','Volume','MeanIntensity');
    stats.Index=labels;
    tCellData{i}={stats};
    disp([num2str(i),': get feature finish']);
end


M = containers.Map('KeyType','uint32','ValueType','any');
IndexNumber=0;
features=zeros(14,1);
for i=1:fileNum
    tt=str2double(split(files(i).name,["_","."]));
    startZ= (tt(1)-1)*512;
    startY= (tt(2)-1)*512*2;
    startX= (tt(3)-1)*512*2;
    t=tCellData{i}{1};
    len=height(t);
    for j=1:len
        if ~M.isKey(uint32(t.Index(j)))
            if t.Volume(j)*8>20
                features(1)=t.MeanIntensity(j);
                features(2)=t.Centroid(j,2)*2+startX;
                features(3)=t.Centroid(j,1)*2+startY;
                features(4)=t.Centroid(j,3)*2+startZ;
                features(5)=t.SurfaceArea(j)*4;
                features(6)=t.Volume(j)*8;
                features(7)=sqrt(1-(t.PrincipalAxisLength(j,3)*t.PrincipalAxisLength(j,3))/(t.PrincipalAxisLength(j,1)*t.PrincipalAxisLength(j,1)));
                features(8)=4*pi*power((3*t.Volume(j))/(4*pi),2.0/3.0) /  t.SurfaceArea(j);
                
                features(9)=t.BoundingBox(j,1)*2+startX;
                features(10)=t.BoundingBox(j,2)*2+startY;
                features(11)=t.BoundingBox(j,3)*2+startZ;
                features(12)=t.BoundingBox(j,4)*2+features(9);
                features(13)=t.BoundingBox(j,5)*2+features(10);
                features(14)=t.BoundingBox(j,6)*2+features(11);
                M(uint32(t.Index(j)))=features;
            end
        end
    end
end


indexs=M.keys();
features=M.values();
fileID=fopen([resultPath,'intensity.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(1));
end
fclose(fileID);

fileID=fopen([resultPath,'position_x.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(2));
end
fclose(fileID);

fileID=fopen([resultPath,'position_y.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(3));
end
fclose(fileID);

fileID=fopen([resultPath,'position_z.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(4));
end
fclose(fileID);

fileID=fopen([resultPath,'surfacearea.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(5));
end
fclose(fileID);

fileID=fopen([resultPath,'volume.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(6));
end
fclose(fileID);

fileID=fopen([resultPath,'eccentricity.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(7));
end
fclose(fileID);

fileID=fopen([resultPath,'sphericity.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %f\n',indexs{i},features{i}(8));
end
fclose(fileID);

fileID=fopen([resultPath,'cell.dat'],'w');
for i=1:M.length()
    fprintf(fileID,'%d %d %d %d %d %d %d\n',indexs{i},features{i}(9),features{i}(10),features{i}(11),features{i}(12),features{i}(13),features{i}(14));
end
fclose(fileID);




% intensity, position_x, position_y, position_z, surfacearea, volume,
% eccentricity, sphericity, solidity
% features=zeros(MaxCellIndex,9,'single');
% for i=1:fileNum
%     tt=str2double(split(files(i).name,["_","."]));
%     startZ= (tt(1)-1)*512;
%     startY= (tt(2)-1)*512*2;
%     startX= (tt(3)-1)*512*2;
%     
%     t=tCellData{i}{1};
%     len=height(t);
%     for j=1:len
%         features(t.Index(j),1)=t.MeanIntensity(j);
%         features(t.Index(j),2)=t.Centroid(j,2)*2+startX;
%         features(t.Index(j),3)=t.Centroid(j,1)*2+startY;
%         features(t.Index(j),4)=t.Centroid(j,3)*2+startZ;
%         features(t.Index(j),5)=t.SurfaceArea(j)*4;
%         features(t.Index(j),6)=t.Volume(j)*8;
%         features(t.Index(j),7)=sqrt(1-(t.PrincipalAxisLength(j,3)*t.PrincipalAxisLength(j,3))/(t.PrincipalAxisLength(j,1)*t.PrincipalAxisLength(j,1)));
%         features(t.Index(j),8)=4*pi*power((3*t.Volume(j))/(4*pi),2.0/3.0) /  t.SurfaceArea(j);
%         %features(t.Index(j),9)=t.Solidity(j);        
%     end
% end

% fileID=fopen([resultPath,'intensity.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,1));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'position_x.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,2));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'position_y.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,3));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'position_z.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,4));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'surfacearea.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,5));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'volume.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,6));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'eccentricity.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,7));
% end
% fclose(fileID);
% 
% fileID=fopen([resultPath,'sphericity.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,8));
% end
% fclose(fileID);

% fileID=fopen([resultPath,'solidity.dat'],'w');
% for i=1:MaxCellIndex
%     fprintf(fileID,'%d %f\n',i,features(i,9));
% end
% fclose(fileID);

