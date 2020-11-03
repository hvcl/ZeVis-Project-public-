%% volume and label for XY and level 0


result_path='P:/Zevis_lsd_data/';

  
maxX=19
maxY=17
maxZ=int32(ceil(16000/512))-1;

maxLevel=5;


%% volume and label for XY and level 1~5

for level=1:5
    curY=int32(floor(double(maxY)/pow2(level)));
    curX=int32(floor(double(maxX)/pow2(level)));
    parfor (z=0:maxZ,20)
        for y=0:curY
            for x=0:curX
                label_new_file_name=sprintf('%sBrain/XY/%d_%d_%d_%d.block',result_path,z,y,x,level);
                label_file_buffer=zeros(512,512,512,'uint32');
                check=0;
                for prev_y=0:1
                    for prev_x=0:1
                        label_prev_file_name=sprintf('%sBrain/XY/%d_%d_%d_%d.block',result_path,z,y*2+prev_y,x*2+prev_x,level-1);
                        fileID2=fopen(label_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        check=1;
                        label_file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,:)=...
                            imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                        fclose(fileID2);
                    end
                end
                if check==1
                    label_new_file_name=sprintf('%sBrain/XY/%d_%d_%d_%d.block',result_path,z,y,x,level);
                    display(label_new_file_name);

                    fileID=fopen(label_new_file_name,'w');                
                    fwrite(fileID,label_file_buffer,'uint32');
                    fclose(fileID);
                end
            end
        end
    end
end

%% Rotate XYZ to ZYX, XZY (level 0)

labelPrevFiles=dir([result_path,'Brain/XY/*0.block']);
len=length(labelPrevFiles);

parfor (i=1:len,20)
    t=split(labelPrevFiles(i).name,["_","."]);
    z=t{1};
    y=t{2};
    x=t{3};
    
    label_xy_file=[result_path,'Brain/XY/',labelPrevFiles(i).name];
    display(label_xy_file);

    fileID=fopen(label_xy_file,'r');                
    Label_prev=uint32(reshape(fread(fileID,'uint32'),[512 512 512]));
    fclose(fileID);
    
    label_yz_path=[result_path,'Brain/YZ/',z,'_',y,'_',x,'_0.block'];
    fileID=fopen(label_yz_path,'w');
    fwrite(fileID,permute(Label_prev,[3,2,1]),'uint32');
    fclose(fileID);
    
    label_zx_path=[result_path,'Brain/ZX/',z,'_',y,'_',x,'_0.block'];
    fileID=fopen(label_zx_path,'w');
    fwrite(fileID,permute(Label_prev,[1,3,2]),'uint32');
    fclose(fileID);  
end


%% volume and label for YZ and level 1~5 zyx
maxX=19
maxY=17
maxZ=31;

for level=1:5
    curY=int32(floor(double(maxY)/pow2(level)));
    curZ=int32(floor(double(maxZ)/pow2(level)));
    parfor (x=0:maxX,20)
        for y=0:curY
            for z=0:curZ

                label_file_buffer=zeros(512,512,512,'uint32');
                check=0;
                for prev_y=0:1
                    for prev_z=0:1
                        label_prev_file_name=sprintf('%sBrain/YZ/%d_%d_%d_%d.block',result_path,z*2+prev_z,y*2+prev_y,x,level-1);
                        fileID2=fopen(label_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        check=1;
                        label_file_buffer(256*prev_z+1:256*prev_z+256,256*prev_y+1:256*prev_y+256,:)=...
                            imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                        fclose(fileID2);
                    end
                end
                if check==1
                    label_new_file_name=sprintf('%sBrain/YZ/%d_%d_%d_%d.block',result_path,z,y,x,level);
                    display(label_new_file_name);

                    fileID=fopen(label_new_file_name,'w');                
                    fwrite(fileID,label_file_buffer,'uint32');
                    fclose(fileID);
                end
            end
        end
    end
end

%% volume and label for ZX and level 1~5 xzy
maxX=19
maxY=17
maxZ=31;

for level=1:5
    curX=int32(floor(double(maxX)/pow2(level)));
    curZ=int32(floor(double(maxZ)/pow2(level)));
    parfor (y=0:maxY,20)
        for x=0:curX
            for z=0:curZ
                label_file_buffer=zeros(512,512,512,'uint32');
                check=0;
                for prev_x=0:1
                    for prev_z=0:1
                        label_prev_file_name=sprintf('%sBrain/ZX/%d_%d_%d_%d.block',result_path,z*2+prev_z,y,x*2+prev_x,level-1);
                        fileID2=fopen(label_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        check=1;
                        label_file_buffer(256*prev_x+1:256*prev_x+256,256*prev_z+1:256*prev_z+256,:)=...
                            imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                        fclose(fileID2);
                    end
                end
                if check==1
                    label_new_file_name=sprintf('%sBrain/ZX/%d_%d_%d_%d.block',result_path,z,y,x,level);
                    display(label_new_file_name);

                    fileID=fopen(label_new_file_name,'w');                
                    fwrite(fileID,label_file_buffer,'uint32');
                    fclose(fileID);
                end
            end
        end
    end
end
