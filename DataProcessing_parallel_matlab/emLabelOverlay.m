%label_base_path='191218_60nm_label/';
%em_base_path='60nm_volume/';

label_base_path='X:/191228_60nm_label/';
em_base_path='X:/180602_median2_160515_SWiFT_60nmpx_singles/';

result_path='result_median_60nm_191230/';
num=2013;
colors=uint8(colormap(jet) * 0.5 * 255);
colors=[colors;0,0,0];

num=2708

for i=689:10000
    if i < 10
        t=sprintf('%s0000%d.raw',label_base_path,i);
    elseif i<100
        t=sprintf('%s000%d.raw',label_base_path,i);
    elseif i<1000
        t=sprintf('%s00%d.raw',label_base_path,i);
    elseif i<10000
        t=sprintf('%s0%d.raw',label_base_path,i);
    else
        t=sprintf('%s%d.raw',label_base_path,i);        
    end
    
    
    if num < 10000
        t2=sprintf('%s0%d.png',em_base_path,num);
    else 
        t2=sprintf('%s%d.png',em_base_path,num);
    end
    while ~isfile(t2)
        num=num+1;
        if num < 10000
            t2=sprintf('%s0%d.png',em_base_path,num);
        else 
            t2=sprintf('%s%d.png',em_base_path,num);
        end
    end
    
    fileID = fopen(t,'rb');
    if fileID==-1
        i
        num=num+1;
        continue;
    end
    Label=fread(fileID,'uint32');
    fclose(fileID);
    Label=reshape(Label,[10240,9216])';
    Label=uint32(Label);
    
    EM=imread(t2);
    
    [w,h]=size(EM);
    
    B=zeros(w,h,3,'uint8');
    weight=ones(w,h,'uint8');
    weight(Label~=0)=2;
    tB=EM./weight;
    tL=mod(Label,64)+1;
    tL(Label==0)=65;
    B(:,:,1)=tB + reshape(colors(tL,1),[w,h]);
    B(:,:,2)=tB + reshape(colors(tL,2),[w,h]);
    B(:,:,3)=tB + reshape(colors(tL,3),[w,h]);
%    B = labeloverlay(EM,Label);
    
    t=sprintf('%s%d.png',result_path,i);
    imwrite(imresize(B,0.5),t);
    
    num=num+1;

    
end