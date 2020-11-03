%% 1. Fix cell info

emPath='E:/Zevis/EM_60nm/';
labelPath='V:/lsd_segmentation/';
fixedBlockPath='E:/Zevis/mauthner_cell_fixed_blocks/12_4_10_0.raw';
resultPath='E:/Zevis/mauthner_cell_fixed_data_for_lsd/';


cellPos1=[5525 2345 6606]; %x y z
Index1=floor(cellPos1/512); %x y z
localPos1=cellPos1-Index1*512;
cellLabel1=4438119; %4446031
Type=-1;%-1: new, 1: modify


%% fixed cell median

f=fopen(fixedBlockPath,'r','b');
data=fread(f,512*512*512,'uint16');
data=reshape(uint16(data),512,512,512);
fclose(f);

left=zeros(512,512,512,'uint16');
left(2:512,1:512,1:512)=data(1:511,1:512,1:512);

right=zeros(512,512,512,'uint16');
right(1:511,1:512,1:512)=data(2:512,1:512,1:512);

data(left==1 & right==1)=1;

left=zeros(512,512,512,'uint16');
left(2:512,1:512,1:512)=data(1:511,1:512,1:512);

right=zeros(512,512,512,'uint16');
right(1:511,1:512,1:512)=data(2:512,1:512,1:512);

data(left==0 & right==0)=0;


%

down=zeros(512,512,512,'uint16');
down(1:512,2:512,1:512)=data(1:512,1:511,1:512);

up=zeros(512,512,512,'uint16');
up(1:512,1:511,1:512)=data(1:512,2:512,1:512);

data(down==1 & up==1)=1;

down=zeros(512,512,512,'uint16');
down(1:512,2:512,1:512)=data(1:512,1:511,1:512);

up=zeros(512,512,512,'uint16');
up(1:512,1:511,1:512)=data(1:512,2:512,1:512);

data(down==0 & up==0)=0;


%

front=zeros(512,512,512,'uint16');
front(1:512,1:512,2:512)=data(1:512,1:512,1:511);

back=zeros(512,512,512,'uint16');
back(1:512,1:512,1:511)=data(1:512,1:512,2:512);

data(front==1 & back==1)=1;

front=zeros(512,512,512,'uint16');
front(1:512,1:512,2:512)=data(1:512,1:512,1:511);

back=zeros(512,512,512,'uint16');
back(1:512,1:512,1:511)=data(1:512,1:512,2:512);

data(front==0 & back==0)=0;

f=fopen([fixedBlockPath,'_'],'w');
fwrite(f,data,'uint16');
fclose(f);


%% data format change

labelPath='E:/Zevis/BrainLabel_60nm/';
%labelPath='V:/lsd_segmentation/';
cellLabel1=217218; %4446031

bPath=[labelPath,'XY/12_4_10_0.block'];

f=fopen(bPath,'r','l');
o_data=fread(f,512*512*512,'uint32');
o_data=reshape(uint32(o_data),512,512,512);
fclose(f);

resPath='E:/Zevis/Manual_cell_segmentation_result/12_4_10_0.raw';
resPath2='E:/Zevis/Manual_cell_segmentation_result/12_4_10_0.block';

f=fopen(resPath,'r','b');
f_data=fread(f,512*512*512,'uint16');
f_data=reshape(uint16(f_data),512,512,512);
fclose(f);

o_data(o_data==cellLabel1)=0;
o_data(f_data==1)=cellLabel1;

f=fopen(resPath2,'w');
fwrite(f,o_data,'uint32');
fclose(f);




bPath=[labelPath,'XY/12_4_11_0.block'];

f=fopen(bPath,'r','l');
o_data=fread(f,512*512*512,'uint32');
o_data=reshape(uint32(o_data),512,512,512);
fclose(f);

resPath='E:/Zevis/Manual_cell_segmentation_result/12_4_11_0.raw';
resPath2='E:/Zevis/Manual_cell_segmentation_result/12_4_11_0.block';

f=fopen(resPath,'r','b');
f_data=fread(f,512*512*512,'uint16');
f_data=reshape(uint16(f_data),512,512,512);
fclose(f);

o_data(o_data==cellLabel1)=0;
o_data(f_data==1)=cellLabel1;

f=fopen(resPath2,'w');
fwrite(f,o_data,'uint32');
fclose(f);


%% feature calculation

Index1=[10 4 12]; %x y z
cellLabel1=4438119; %4446031

leftPath='E:/Zevis/mauthner_cell_fixed_data_for_lsd/12_4_10_0.block';
rightPath='E:/Zevis/mauthner_cell_fixed_data_for_lsd/12_4_11_0.block';

leftEMPath=[emPath,'XY/12_4_10_0.block'];
rightEMPath=[emPath,'XY/12_4_11_0.block'];

f=fopen(leftPath,'r','l');
left_data=fread(f,512*512*512,'uint32');
left_data=reshape(uint32(left_data),512,512,512);
fclose(f);

f=fopen(rightPath,'r','l');
right_data=fread(f,512*512*512,'uint32');
right_data=reshape(uint32(right_data),512,512,512);
fclose(f);

label=zeros(1024,512,512,'uint32');
label(1:512,:,:)=left_data;
label(513:1024,:,:)=right_data;


f=fopen(leftEMPath,'r','l');
left_data=fread(f,512*512*512,'uint8');
left_data=reshape(uint8(left_data),512,512,512);
fclose(f);

f=fopen(rightEMPath,'r','l');
right_data=fread(f,512*512*512,'uint8');
right_data=reshape(uint8(right_data),512,512,512);
fclose(f);

em=zeros(1024,512,512,'uint8');
em(1:512,:,:)=left_data;
em(513:1024,:,:)=right_data;

label2=zeros(1024,512,512,'uint32');
label2(label==cellLabel1)=1;
stats = regionprops3(label2,em,'BoundingBox','Centroid','PrincipalAxisLength','SurfaceArea','Volume','MeanIntensity');

startX=Index1(1)*512;
startY=Index1(2)*512;
startZ=Index1(3)*512;

features(1)=stats.MeanIntensity;
features(2)=stats.Centroid(2)+startX;
features(3)=stats.Centroid(1)+startY;
features(4)=stats.Centroid(3)+startZ;
features(5)=stats.SurfaceArea;
features(6)=stats.Volume;
features(7)=sqrt(1-(stats.PrincipalAxisLength(3)*stats.PrincipalAxisLength(3))/(stats.PrincipalAxisLength(1)*stats.PrincipalAxisLength(1)));
features(8)=4*pi*power((3*stats.Volume)/(4*pi),2.0/3.0) /  stats.SurfaceArea;

features(9)=stats.BoundingBox(2)+startX;
features(10)=stats.BoundingBox(1)+startY;
features(11)=stats.BoundingBox(3)+startZ;
features(12)=stats.BoundingBox(5)+features(9);
features(13)=stats.BoundingBox(4)+features(10);
features(14)=stats.BoundingBox(6)+features(11);

%% update blocks


result_path='E:/Zevis/mauthner_cell_fixed_data_for_lsd/';
  
maxX=19
maxY=17
maxZ=int32(ceil(16000/512))-1;

maxLevel=5;

for level=1:5
    curY=int32(floor(double(maxY)/pow2(level)));
    curX=int32(floor(double(maxX)/pow2(level)));
    parfor (z=0:maxZ,20)
        for y=0:curY
            for x=0:curX
                label_file_buffer=zeros(512,512,512,'uint32');
                check=0;
                
                for prev_y=0:1
                    for prev_x=0:1
                        label_prev_file_name=sprintf('%sXY/%d_%d_%d_%d.block',result_path,z,y*2+prev_y,x*2+prev_x,level-1);
                        fileID2=fopen(label_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        check=1;
                    end
                end

                if check==1
                    for prev_y=0:1
                        for prev_x=0:1
                            label_prev_file_name=sprintf('%sXY/%d_%d_%d_%d.block',result_path,z,y*2+prev_y,x*2+prev_x,level-1);
                            label_prev_file_name2=sprintf('%sXY/%d_%d_%d_%d.block',labelPath,z,y*2+prev_y,x*2+prev_x,level-1);

                            fileID2=fopen(label_prev_file_name,'r');
                            if fileID2==-1
                                fileID2=fopen(label_prev_file_name2,'r');
                            end
                            if fileID2~=-1
                                label_file_buffer(256*prev_x+1:256*prev_x+256,256*prev_y+1:256*prev_y+256,:)=...
                                    imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                    label_new_file_name=sprintf('%sXY/%d_%d_%d_%d.block',result_path,z,y,x,level);
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

labelPrevFiles=dir([result_path,'XY/*0.block']);
len=length(labelPrevFiles);

parfor (i=1:len,20)
    t=split(labelPrevFiles(i).name,["_","."]);
    z=t{1};
    y=t{2};
    x=t{3};
    
    label_xy_file=[result_path,'XY/',labelPrevFiles(i).name];
    display(label_xy_file);

    fileID=fopen(label_xy_file,'r');                
    Label_prev=uint32(reshape(fread(fileID,'uint32'),[512 512 512]));
    fclose(fileID);
    
    label_yz_path=[result_path,'YZ/',z,'_',y,'_',x,'_0.block'];
    fileID=fopen(label_yz_path,'w');
    fwrite(fileID,permute(Label_prev,[3,2,1]),'uint32');
    fclose(fileID);
    
    label_zx_path=[result_path,'ZX/',z,'_',y,'_',x,'_0.block'];
    fileID=fopen(label_zx_path,'w');
    fwrite(fileID,permute(Label_prev,[1,3,2]),'uint32');
    fclose(fileID);  
end


%% volume and label for YZ and level 1~5 zyx

maxX=19
maxY=17
maxZ=int32(ceil(16000/512))-1;

maxLevel=5;

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
                        label_prev_file_name=sprintf('%sYZ/%d_%d_%d_%d.block',result_path,z*2+prev_z,y*2+prev_y,x,level-1);
                        fileID2=fopen(label_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        check=1;
                    end
                end

                if check==1
                    for prev_y=0:1
                        for prev_z=0:1
                            label_prev_file_name=sprintf('%sYZ/%d_%d_%d_%d.block',result_path,z*2+prev_z,y*2+prev_y,x,level-1);
                            label_prev_file_name2=sprintf('%sYZ/%d_%d_%d_%d.block',labelPath,z*2+prev_z,y*2+prev_y,x,level-1);

                            fileID2=fopen(label_prev_file_name,'r');
                            if fileID2==-1
                                fileID2=fopen(label_prev_file_name2,'r');
                            end
                            if fileID2~=-1
                                label_file_buffer(256*prev_z+1:256*prev_z+256,256*prev_y+1:256*prev_y+256,:)=...
                                    imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                    label_new_file_name=sprintf('%sYZ/%d_%d_%d_%d.block',result_path,z,y,x,level);
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
maxZ=int32(ceil(16000/512))-1;

maxLevel=5;

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
                        label_prev_file_name=sprintf('%sZX/%d_%d_%d_%d.block',result_path,z*2+prev_z,y,x*2+prev_x,level-1);
                        fileID2=fopen(label_prev_file_name,'r');
                        if fileID2==-1
                            continue;
                        end
                        check=1;
                    end
                end

                if check==1
                    for prev_x=0:1
                        for prev_z=0:1
                            label_prev_file_name=sprintf('%sZX/%d_%d_%d_%d.block',result_path,z*2+prev_z,y,x*2+prev_x,level-1);
                            label_prev_file_name2=sprintf('%sZX/%d_%d_%d_%d.block',labelPath,z*2+prev_z,y,x*2+prev_x,level-1);

                            fileID2=fopen(label_prev_file_name,'r');
                            if fileID2==-1
                                fileID2=fopen(label_prev_file_name2,'r');
                            end
                            if fileID2~=-1
                                label_file_buffer(256*prev_x+1:256*prev_x+256,256*prev_z+1:256*prev_z+256,:)=...
                                    imresize3(reshape(uint32(fread(fileID2,[512*512 512],'uint32')),[512 512 512]),[256 256 512],'nearest');
                                fclose(fileID2);
                            end
                        end
                    end
                    label_new_file_name=sprintf('%sZX/%d_%d_%d_%d.block',result_path,z,y,x,level);
                    display(label_new_file_name);

                    fileID=fopen(label_new_file_name,'w');                
                    fwrite(fileID,label_file_buffer,'uint32');
                    fclose(fileID);
                end
            end
        end
    end
end