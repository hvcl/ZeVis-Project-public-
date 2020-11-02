emPath='result_data_191224/EM/XY/';
brainPath='result_data_191224/Brain/XY/';

resultEmPath='result_data_191224_rotation/EM/XY/';
resultBrainPath='result_data_191224_rotation/Brain/XY/';

files=dir([emPath,'*.block']);
fileNum=size(files,1);
basePath=emPath;
resultPath=resultEmPath;
parfor(i=1:fileNum,20)
    i
    curFile=[basePath,files(i).name];
    t=split(files(i).name,["_","."]);
    
    fileID = fopen(curFile,'r');
    data=fread(fileID,'uint8');
    fclose(fileID);
    data=uint8(reshape(data,[512 512 512]));
    resData=permute(data,[2 1 3]);
    resultFile=[resultPath,t{1},'_',t{3},'_',t{2},'_',t{4},'.block'];
    
    fileID = fopen(resultFile,'w');
    fwrite(fileID,resData,'uint8');
    fclose(fileID);

end




files=dir([brainPath,'*.block']);
fileNum=size(files,1);
basePath=brainPath;
resultPath=resultBrainPath;
parfor(i=1:fileNum,20)
    i
    curFile=[basePath,files(i).name];
    t=split(files(i).name,["_","."]);
    
    fileID = fopen(curFile,'r');
    data=fread(fileID,'uint32');
    fclose(fileID);
    data=uint32(reshape(data,[512 512 512]));
    resData=permute(data,[2 1 3]);
    resultFile=[resultPath,t{1},'_',t{3},'_',t{2},'_',t{4},'.block'];
    
    fileID = fopen(resultFile,'w');
    fwrite(fileID,resData,'uint32');
    fclose(fileID);

end

