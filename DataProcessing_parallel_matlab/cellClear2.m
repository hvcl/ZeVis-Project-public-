function cellClear2(basePath)
%    basePath='E:/Zevis/BrainLabel_60nm/';
    sphericityData=load([basePath,'MorphologicalFeature/sphericity.dat']);
    removeLabels=sphericityData(sphericityData(:,2)>1.0,1);
    

%    cellData=load([basePath,'cell.dat']);
%    f=fopen([basePath,'cell.dat'],'w');
%    for i=1:size(cellData,1)
%        l=cellData(i,1);
%        if isempty(find(removeLabels==l))
%            fprintf(f,"%d %d %d %d %d %d %d\n",cellData(i,1),cellData(i,2),cellData(i,3),cellData(i,4),cellData(i,5),cellData(i,6),cellData(i,7));
%        end        
%    end
%    fclose(f);
    
    featureFiles=dir([basePath,'MorphologicalFeature/*.dat']);
    for n=1:length(featureFiles)
        if strcmp(featureFiles(n).name,'sphericity.dat')
            featureData=load([basePath,'MorphologicalFeature/',featureFiles(n).name]);
            f=fopen([basePath,'MorphologicalFeature/',featureFiles(n).name],'w');
            for i=1:size(featureData,1)
                l=featureData(i,1);
                if isempty(find(removeLabels==l))
                    fprintf(f,"%d %f\n",featureData(i,1),featureData(i,2));
                else
                    fprintf(f,"%d 0.0\n",featureData(i,1));
                end        
            end
            fclose(f);
        end
    end    
end