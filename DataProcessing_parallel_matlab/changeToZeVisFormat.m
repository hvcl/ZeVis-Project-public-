%% volume and label for XY and level 0

%label_base_path='X:/191228_60nm_label/';
%em_base_path='X:/180602_median2_160515_SWiFT_60nmpx_singles/';

label_base_path='X:/191218_60nm_label/';
em_base_path='X:/160515_SWiFT_60nmpx_singles/';


result_path='result_data_191224_60nm/';

labelFiles=dir([label_base_path,'*.raw']);
emFiles=dir([em_base_path,'*.png']);

  
maxX=19
maxY=17
maxZ=int32(ceil(16000/512))-1;
parfor (global_z=0:maxZ,20)
    for local_z=0:511
        i=global_z*512+local_z;
        if i>=16000
            break;
        end
        
        display(['i: ', num2str(i),' & num: ',num2str(num)]);

        fileID = fopen([label_base_path,labelFiles(i+1).name],'rb');
        Label=fread(fileID,'uint32');
        fclose(fileID);
        Label=reshape(Label,[10240,9216]);
        Label=uint32(Label);

        EM=imread([em_base_path,emFiles(i+1).name])';
        [w,h]=size(EM);

        z=global_z;
        for x=0:maxX
            for y=0:maxY
                t=sprintf('%sEM/XY/%d_%d_%d_0.block',result_path,z,y,x);
                fileID=fopen(t,'a');
                fwrite(fileID,EM(512*x+1:512*x+512,512*y+1:512*y+512),'uint8');
                fclose(fileID);

                t=sprintf('%sBrain/XY/%d_%d_%d_0.block',result_path,z,y,x);
                fileID=fopen(t,'a');
                fwrite(fileID,Label(512*x+1:512*x+512,512*y+1:512*y+512),'uint32');
                fclose(fileID);

            end
        end    
    end
end
% fill empty part
parfor (x=0:maxX,20)
    for y=0:maxY
            t=sprintf('%sEM/XY/31_%d_%d_0.block',result_path,y,x);
            fileID=fopen(t,'a');
            tt=zeros(512,512,128*3,'uint8')+250;
            fwrite(fileID,tt,'uint8');
            fclose(fileID);
            
            t=sprintf('%sBrain/XY/31_%d_%d_0.block',result_path,y,x);
            fileID=fopen(t,'a');
            fwrite(fileID,zeros(512,512,128*3,'uint32'),'uint32');
            fclose(fileID);
    end
end
%% remove empty label block

tFiles=dir([result_path,'Brain/XY/*.block']);
len=length(tFiles);
for i=1:len
    curFile=[result_path,'Brain/XY/',tFiles(i).name];
    fileID=fopen(curFile,'r');
    label=fread(fileID,[512*512 512],'uint32');
    fclose(fileID);
    if sum(label(:))==0
        delete(curFile);
        disp([curFile,' delete']);
    end
end

%% volume and label for XY and level 1~5

for level=1:5
    curY=int32(floor(double(maxY)/pow2(level)));
    curX=int32(floor(double(maxX)/pow2(level)));
    parfor (z=0:maxZ,20)
        for y=0:curY
            for x=0:curX
                em_new_file_name=sprintf('%sEM/XY/%d_%d_%d_%d.block',result_path,z,y,x,level);
                display(em_new_file_name);
                em_file_buffer=zeros(512,512,512,'uint8');
                fileID=fopen(em_new_file_name,'w');
                for prev_y=0:1
                    for prev_x=0:1
                        em_prev_file_name=sprintf('%sEM/XY/%d_%d_%d_%d.block',result_path,z,y*2+prev_y,x*2+prev_x,level-1);
                        fileID2=fopen(em_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        em_file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,:)=...
                            imresize3(reshape(uint8(fread(fileID2,[512*512 512],'uint8')),[512 512 512]),[256 256 512],'nearest');
                        fclose(fileID2);
                    end
                end
                fwrite(fileID,em_file_buffer,'uint8');
                fclose(fileID);


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

emPrevFiles=dir([result_path,'EM/XY/*0.block']);
len=length(emPrevFiles);

parfor (i=1:len,20)
    t=split(emPrevFiles(i).name,["_","."]);
    z=t{1};
    y=t{2};
    x=t{3};
    
    em_xy_file=[result_path,'EM/XY/',emPrevFiles(i).name];
    display(em_xy_file);

    fileID=fopen(em_xy_file,'r');                
    EM_prev=uint8(reshape(fread(fileID,'uint8'),[512 512 512]));
    fclose(fileID);
    
    em_yz_path=[result_path,'EM/YZ/',z,'_',y,'_',x,'_0.block'];
    fileID=fopen(em_yz_path,'w');
    fwrite(fileID,permute(EM_prev,[3,2,1]),'uint8');
    fclose(fileID);
    
    em_zx_path=[result_path,'EM/ZX/',z,'_',y,'_',x,'_0.block'];
    fileID=fopen(em_zx_path,'w');
    fwrite(fileID,permute(EM_prev,[1,3,2]),'uint8');
    fclose(fileID);
   
end

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
                em_new_file_name=sprintf('%sEM/YZ/%d_%d_%d_%d.block',result_path,z,y,x,level);
                display(em_new_file_name);
                em_file_buffer=zeros(512,512,512,'uint8');
                fileID=fopen(em_new_file_name,'w');
                for prev_y=0:1
                    for prev_z=0:1
                        em_prev_file_name=sprintf('%sEM/YZ/%d_%d_%d_%d.block',result_path,z*2+prev_z,y*2+prev_y,x,level-1);
                        fileID2=fopen(em_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        em_file_buffer(256*prev_z+1:256*prev_z+256,256*prev_y+1:256*prev_y+256,:)=...
                            imresize3(reshape(uint8(fread(fileID2,[512*512 512],'uint8')),[512 512 512]),[256 256 512],'nearest');
                        fclose(fileID2);
                    end
                end
                fwrite(fileID,em_file_buffer,'uint8');
                fclose(fileID);


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
                em_new_file_name=sprintf('%sEM/ZX/%d_%d_%d_%d.block',result_path,z,y,x,level);
                display(em_new_file_name);
                em_file_buffer=zeros(512,512,512,'uint8');
                fileID=fopen(em_new_file_name,'w');
                for prev_x=0:1
                    for prev_z=0:1
                        em_prev_file_name=sprintf('%sEM/ZX/%d_%d_%d_%d.block',result_path,z*2+prev_z,y,x*2+prev_x,level-1);
                        fileID2=fopen(em_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        em_file_buffer(256*prev_x+1:256*prev_x+256,256*prev_z+1:256*prev_z+256,:)=...
                            imresize3(reshape(uint8(fread(fileID2,[512*512 512],'uint8')),[512 512 512]),[256 256 512],'nearest');
                        fclose(fileID2);
                    end
                end
                fwrite(fileID,em_file_buffer,'uint8');
                fclose(fileID);


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
