labelPath='result_data_200101/Brain/XY/';
resultPath='result_data_200101/Brain/cell.dat';

files=dir([labelPath,'*0.block']);
fileNum=size(files,1);

totalMaxLabel=0;
tCellData=cell(fileNum,1);
tLabelNum=zeros(fileNum,1,'uint32');

%cluster = parcluster;
parfor(i=1:fileNum,20)
    i
    curFile=[labelPath,files(i).name];
    t=str2double(split(files(i).name,["_","."]));
    startZ= uint32(t(1)*512);
    startY= uint32(t(2)*512);
    startX= uint32(t(3)*512);
    
    fileID = fopen(curFile,'r');
    Label=fread(fileID,'uint32');
    fclose(fileID);
    Label=uint32(Label);
    
    labels=unique(Label(Label(:)~=0));
    len=length(labels);
    
    tLabelNum(i)=len;
    tttData=zeros(len,7,'uint32');
    for j=1:len
        matches=uint32(find(Label(:)==labels(j)))-1;
        if isempty(matches)
            continue;
        end
        x=mod(matches,512)+startX;
        y=mod(idivide(matches,512,'floor'),512)+startY;
        z=idivide(matches,512*512,'floor')+startZ;
        tttData(j,:)=[labels(j),min(x(:)),min(y(:)),min(z(:)),max(x(:)),max(y(:)),max(z(:))];
    end
    tCellData{i}={tttData};
end


for i=1:fileNum
    t=tCellData{i}{1};
    for j=1:tLabelNum(i,1)
        if totalMaxLabel<t(j,1)
            totalMaxLabel=t(j,1);
        end
    end
end
cellData=zeros(totalMaxLabel,6,'uint32');
for i=1:fileNum
    t=tCellData{i}{1};
    for j=1:tLabelNum(i,1)
        for k=1:3
            if cellData(t(j,1),k)==0
                cellData(t(j,1),k)=t(j,k+1);
            elseif cellData(t(j,1),k)>t(j,k+1)
                cellData(t(j,1),k)=t(j,k+1);
            end
            
            if cellData(t(j,1),k+3)==0
                cellData(t(j,1),k+3)=t(j,k+4);
            elseif cellData(t(j,1),k+3)<t(j,k+4)
                cellData(t(j,1),k+3)=t(j,k+4);
            end
        end
    end
end

fileID=fopen(resultPath,'w');
for i=1:totalMaxLabel
    fprintf(fileID,'%d %d %d %d %d %d %d\n',i,cellData(i,1),cellData(i,2),cellData(i,3),cellData(i,4),cellData(i,5),cellData(i,6));
end
fclose(fileID);
