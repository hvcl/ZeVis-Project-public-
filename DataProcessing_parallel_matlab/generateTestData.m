fromPath='E:\Zevis\BrainLabel_60nm\';
toPath='E:\Zevis\test_data_240nm\BrainLabel\';


for k=2:5
    path=sprintf('%sXY\\*_%d.block',fromPath,k);
    list=dir(path);

    for i=1:size(list,1)
        fromFileName=[list(i).folder,'\',list(i).name];
        toFileName=[toPath,'XY\',list(i).name];
        copyfile(fromFileName,toFileName);
    end
end

for k=2:5
    path=sprintf('%sYZ\\*_%d.block',fromPath,k);
    list=dir(path);

    for i=1:size(list,1)
        fromFileName=[list(i).folder,'\',list(i).name];
        toFileName=[toPath,'YZ\',list(i).name];
        copyfile(fromFileName,toFileName);
    end
end

for k=2:5
    path=sprintf('%sZX\\*_%d.block',fromPath,k);
    list=dir(path);

    for i=1:size(list,1)
        fromFileName=[list(i).folder,'\',list(i).name];
        toFileName=[toPath,'ZX\',list(i).name];
        copyfile(fromFileName,toFileName);
    end
end


fromPath='E:\Zevis\EM_60nm\';
toPath='E:\Zevis\test_data_240nm\EM\';


for k=3:5
    path=sprintf('%sXY\\*_%d.block',fromPath,k);
    list=dir(path);

    for i=1:size(list,1)
        fromFileName=[list(i).folder,'\',list(i).name];
        toFileName=[toPath,'XY\',list(i).name];
        copyfile(fromFileName,toFileName);
    end
end

for k=3:5
    path=sprintf('%sYZ\\*_%d.block',fromPath,k);
    list=dir(path);

    for i=1:size(list,1)
        fromFileName=[list(i).folder,'\',list(i).name];
        toFileName=[toPath,'YZ\',list(i).name];
        copyfile(fromFileName,toFileName);
    end
end

for k=3:5
    path=sprintf('%sZX\\*_%d.block',fromPath,k);
    list=dir(path);

    for i=1:size(list,1)
        fromFileName=[list(i).folder,'\',list(i).name];
        toFileName=[toPath,'ZX\',list(i).name];
        copyfile(fromFileName,toFileName);
    end
end