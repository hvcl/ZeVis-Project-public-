function DownScaleZeVisCellData(prevPath,resultPath,factor)
    labelFileList=dir([resultPath,'XY/*_0.block']);
    len=length(labelFileList);
    curLabels=[];
    for i=1:len
        curFile=[resultPath,'XY/',labelFileList(i).name];
        fileID=fopen(curFile,'r');
        label=uint32(fread(fileID,[512*512 512],'uint32'));
        fclose(fileID);
        curLabels=[curLabels;unique(label(label~=0))];
    end
    curLabels=unique(curLabels);
    
    cellData=load([prevPath,'cell.dat']);
    f=fopen([resultPath,'cell.dat'],'w');
    
    for i=1:size(cellData,1)
        l=cellData(i,1);
        if isempty(find(curLabels==l))==false
            fprintf(f,"%d %d %d %d %d %d %d\n",cellData(i,1),round(cellData(i,2)*factor),round(cellData(i,3)*factor),round(cellData(i,4)*factor),round(cellData(i,5)*factor),round(cellData(i,6)*factor),round(cellData(i,7)*factor));
        end        
    end
    fclose(f);
    
    featureFiles=dir([prevPath,'MorphologicalFeature/*.dat']);
    for n=1:length(featureFiles)
        featureData=load([prevPath,'MorphologicalFeature/',featureFiles(n).name]);
        
        if strcmp(featureFiles(n).name,'position_x.dat')
            mul=factor;
        elseif strcmp(featureFiles(n).name,'position_y.dat')
            mul=factor;
        elseif strcmp(featureFiles(n).name,'position_z.dat')
            mul=factor;
        elseif strcmp(featureFiles(n).name,'surfacearea.dat')
            mul=factor*factor;
        elseif strcmp(featureFiles(n).name,'volume.dat')
            mul=factor*factor*factor;
        else
            mul=1;
        end
            
        
        f=fopen([resultPath,'MorphologicalFeature/',featureFiles(n).name],'w');
        for i=1:size(featureData,1)
            l=featureData(i,1);
            if isempty(find(curLabels==l))==false
                fprintf(f,"%d %f\n",featureData(i,1),featureData(i,2)*mul);
            end        
        end
        fclose(f);
    end
end