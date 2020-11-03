function DownScaleZeVisData(fromPath,toPath,maxX,maxY,maxZ,format,maxLevel)

%    fromPath='E:/Zevis/EM_60nm/';
%    toPath='V:/Zevis_120nm_data/EM/';

%    maxX=19;
%    maxY=17;
%    maxZ=31;

%% level 0
    curX=int32(floor(double(maxX)/2));
    curY=int32(floor(double(maxY)/2));
    curZ=int32(floor(double(maxZ)/2));

    parfor (x=0:curX,20)
        for y=0:curY
            for z=0:curZ
                file_buffer=zeros(512,512,512,format);
                for prev_z=0:1
                    for prev_y=0:1
                        for prev_x=0:1
                            prev_file_name=sprintf('%sXY/%d_%d_%d_0.block',fromPath,z*2+prev_z,y*2+prev_y,x*2+prev_x);
                            fileID2=fopen(prev_file_name,'r');
                            if fileID2==-1
                                continue;
                            end
                            if strcmp(format,'uint32')
                                file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,256*prev_z+1:256*prev_z+256)=...
                                imresize3(reshape(uint32(fread(fileID2,[512*512 512],format)),[512 512 512]),[256 256 256],'nearest');
                                fclose(fileID2);
                            elseif strcmp(format,'uint8')
                                file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,256*prev_z+1:256*prev_z+256)=...
                                imresize3(reshape(uint8(fread(fileID2,[512*512 512],format)),[512 512 512]),[256 256 256],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                end
                if sum(file_buffer(:))~=0
                    new_file_name=sprintf('%sXY/%d_%d_%d_0.block',toPath,z,y,x);
                    display(new_file_name);
                    fileID=fopen(new_file_name,'w');                
                    fwrite(fileID,file_buffer,format);
                    fclose(fileID);
                end
            end
        end
    end
    
%% level 1~maxLevel
    for level=1:maxLevel
        curZ=int32(floor(double(maxZ)/2));
        curY=int32(floor(double(maxY)/pow2(level+1)));
        curX=int32(floor(double(maxX)/pow2(level+1)));
        parfor (z=0:curZ,20)
            for y=0:curY
                for x=0:curX
                    file_buffer=zeros(512,512,512,format);
                    for prev_y=0:1
                        for prev_x=0:1
                            prev_file_name=sprintf('%sXY/%d_%d_%d_%d.block',toPath,z,y*2+prev_y,x*2+prev_x,level-1);
                            fileID2=fopen(prev_file_name,'r');
                            if fileID2==-1
                                continue;
                            end
                            if strcmp(format,'uint8')
                                file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,:)=...
                                imresize3(reshape(uint8(fread(fileID2,[512*512 512],format)),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            elseif strcmp(format,'uint32')
                                file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,:)=...
                                    imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                    if sum(file_buffer(:))~=0
                        new_file_name=sprintf('%sXY/%d_%d_%d_%d.block',toPath,z,y,x,level);
                        display(new_file_name);
                        fileID=fopen(new_file_name,'w');                
                        fwrite(fileID,file_buffer,format);
                        fclose(fileID);
                    end
                end
            end
        end
    end
   
%% Rotate XYZ to ZYX, XZY (level 0)

    PrevFiles=dir([toPath,'XY/*0.block']);
    len=length(PrevFiles);

    for i=1:len
        t=split(PrevFiles(i).name,["_","."]);
        z=t{1};
        y=t{2};
        x=t{3};

        xy_file=[toPath,'XY/',PrevFiles(i).name];
        display(xy_file);

        fileID=fopen(xy_file,'r');                
        if strcmp(format,'uint8')
            prev=uint8(reshape(fread(fileID,format),[512 512 512]));
        elseif strcmp(format,'uint32')
            prev=uint32(reshape(fread(fileID,format),[512 512 512]));
        end
        fclose(fileID);

        yz_path=[toPath,'YZ/',z,'_',y,'_',x,'_0.block'];
        fileID=fopen(yz_path,'w');
        fwrite(fileID,permute(prev,[3,2,1]),format);
        fclose(fileID);

        zx_path=[toPath,'ZX/',z,'_',y,'_',x,'_0.block'];
        fileID=fopen(zx_path,'w');
        fwrite(fileID,permute(prev,[1,3,2]),format);
        fclose(fileID);  
    end
    
%% level 1~maxLevel for YZ
    for level=1:maxLevel
        curY=int32(floor(double(maxY)/pow2(level)));
        curZ=int32(floor(double(maxZ)/pow2(level)));
        curX=int32(floor(double(maxX)/2));
        parfor (x=0:curX,20)
            for y=0:curY
                for z=0:curZ
                    file_buffer=zeros(512,512,512,format);
                    for prev_y=0:1
                        for prev_z=0:1
                            prev_file_name=sprintf('%sYZ/%d_%d_%d_%d.block',toPath,z*2+prev_z,y*2+prev_y,x,level-1);
                            fileID2=fopen(prev_file_name,'r');
                            if fileID2==-1
                                continue;
                            end
                            if strcmp(format,'uint8')
                                file_buffer(256*prev_z+1:256*prev_z+256,256*prev_y+1:256*prev_y+256,:)=...
                                imresize3(reshape(uint8(fread(fileID2,[512*512 512],format)),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            elseif strcmp(format,'uint32')
                                file_buffer(256*prev_z+1:256*prev_z+256,256*prev_y+1:256*prev_y+256,:)=...
                                    imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                    if sum(file_buffer(:))~=0
                        new_file_name=sprintf('%sYZ/%d_%d_%d_%d.block',toPath,z,y,x,level);
                        display(new_file_name);
                        fileID=fopen(new_file_name,'w');                
                        fwrite(fileID,file_buffer,format);
                        fclose(fileID);
                    end
                end
            end
        end
    end

    
%% level 1~maxLevel for ZX
    for level=1:maxLevel
        curX=int32(floor(double(maxX)/pow2(level)));
        curZ=int32(floor(double(maxZ)/pow2(level)));
        curY=int32(floor(double(maxY)/2));
        parfor (y=0:curY,20)
            for x=0:curX
                for z=0:curZ
                    file_buffer=zeros(512,512,512,format);
                    for prev_x=0:1
                        for prev_z=0:1
                            prev_file_name=sprintf('%sZX/%d_%d_%d_%d.block',toPath,z*2+prev_z,y,x*2+prev_x,level-1);
                            fileID2=fopen(prev_file_name,'r');
                            if fileID2==-1
                                continue;
                            end
                            if strcmp(format,'uint8')
                                file_buffer(256*prev_x+1:256*prev_x+256,256*prev_z+1:256*prev_z+256,:)=...
                                imresize3(reshape(uint8(fread(fileID2,[512*512 512],format)),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            elseif strcmp(format,'uint32')
                                file_buffer(256*prev_x+1:256*prev_x+256,256*prev_z+1:256*prev_z+256,:)=...
                                    imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                    if sum(file_buffer(:))~=0
                        new_file_name=sprintf('%sZX/%d_%d_%d_%d.block',toPath,z,y,x,level);
                        display(new_file_name);
                        fileID=fopen(new_file_name,'w');                
                        fwrite(fileID,file_buffer,format);
                        fclose(fileID);
                    end
                end
            end
        end
    end
end
