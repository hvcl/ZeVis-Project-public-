function labelClear(basePath)
    sphericityData=load([basePath,'MorphologicalFeature/sphericity.dat']);
    removeLabels=sphericityData(sphericityData(:,2)>1.0,1);
    

    cellData=load([basePath,'cell.dat']);
    f=fopen([basePath,'cell.dat'],'w');
    for i=1:size(cellData,1)
        l=cellData(i,1);
        if isempty(find(removeLabels==l))
            fprintf(f,"%d %d %d %d %d %d %d\n",cellData(i,1),cellData(i,2),cellData(i,3),cellData(i,4),cellData(i,5),cellData(i,6),cellData(i,7));
        end        
    end
    fclose(f);
    
    featureFiles=dir([basePath,'MorphologicalFeature/*.dat']);
    for n=1:length(featureFiles)
        featureData=load([basePath,'MorphologicalFeature/',featureFiles(n).name]);
        f=fopen([basePath,'MorphologicalFeature/',featureFiles(n).name],'w');
        for i=1:size(featureData,1)
            l=featureData(i,1);
            if isempty(find(removeLabels==l))
                fprintf(f,"%d %f\n",featureData(i,1),featureData(i,2));
            end        
        end
        fclose(f);
    end    

    
    labelFileList=dir([basePath,'XY/*.block']);
    len=length(labelFileList);
    for i=1:len
        curFile=[basePath,'XY/',labelFileList(i).name];
        display(curFile);
        fileID=fopen(curFile,'r');
        label=uint32(fread(fileID,512*512*512,'uint32'));
        fclose(fileID);
        t=unique(label(label~=0));
        t=intersect(t,removeLabels);
        for j=1:length(t)
            label(label==t(j))=0;
        end
        
        fileID=fopen(curFile,'w');
        fwrite(fileID,label,'uint32');
        fclose(fileID);
    end
    
    labelFileList=dir([basePath,'YZ/*.block']);
    len=length(labelFileList);
    for i=1:len
        curFile=[basePath,'YZ/',labelFileList(i).name];
        display(curFile);
        fileID=fopen(curFile,'r');
        label=uint32(fread(fileID,512*512*512,'uint32'));
        fclose(fileID);
        t=unique(label(label~=0));
        t=intersect(t,removeLabels);
        for j=1:length(t)
            label(label==t(j))=0;
        end
        
        fileID=fopen(curFile,'w');
        fwrite(fileID,label,'uint32');
        fclose(fileID);
    end
    

    labelFileList=dir([basePath,'ZX/*.block']);
    len=length(labelFileList);
    for i=1:len
        curFile=[basePath,'ZX/',labelFileList(i).name];
        display(curFile);
        fileID=fopen(curFile,'r');
        label=uint32(fread(fileID,512*512*512,'uint32'));
        fclose(fileID);
        t=unique(label(label~=0));
        t=intersect(t,removeLabels);
        for j=1:length(t)
            label(label==t(j))=0;
        end
        
        fileID=fopen(curFile,'w');
        fwrite(fileID,label,'uint32');
        fclose(fileID);
    end

end