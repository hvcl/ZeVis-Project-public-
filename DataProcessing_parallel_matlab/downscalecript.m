BasePath='E:/Zevis/';
ResultPath='V:/Zevis_240nm_data/';

DownScaleZeVisData('E:/Zevis/BrainLabel_60nm/','V:/Zevis_120nm_data/Brain/',19,17,31,'uint32',4);
%DownScaleZeVisData('E:/Zevis/EM_60nm/','V:/Zevis_120nm_data/EM/',19,17,31,'uint8',4);
%DownScaleZeVisData('E:/Zevis/Subregion_60nm/Brainmask/','V:/Zevis_120nm_data/Subregion/Brainmask/',19,17,31,'uint8',4);
%DownScaleZeVisData('E:/Zevis/Subregion_60nm/DiencephalonHabenulaLeft/','V:/Zevis_120nm_data/Subregion/DiencephalonHabenulaLeft/',19,17,31,'uint8',4);
%DownScaleZeVisData('E:/Zevis/Subregion_60nm/DiencephalonHabenulaRight/','V:/Zevis_120nm_data/Subregion/DiencephalonHabenulaRight/',19,17,31,'uint8',4);
%DownScaleZeVisData('E:/Zevis/Subregion_60nm/MesencephalonTecumNeuropil/','V:/Zevis_120nm_data/Subregion/MesencephalonTecumNeuropil/',19,17,31,'uint8',4);

DownScaleZeVisCellData('E:/Zevis/BrainLabel_60nm/','V:/Zevis_120nm_data/Brain/',0.5);

DownScaleZeVisData('V:/Zevis_120nm_data/Brain/','V:/Zevis_240nm_data/Brain/',9,8,15,'uint32',3);
DownScaleZeVisData('V:/Zevis_120nm_data/EM/','V:/Zevis_240nm_data/EM/',9,8,15,'uint8',3);
DownScaleZeVisData('V:/Zevis_120nm_data/Subregion/Brainmask/','V:/Zevis_240nm_data/Subregion/Brainmask/',9,8,15,'uint8',3);
DownScaleZeVisData('V:/Zevis_120nm_data/Subregion/DiencephalonHabenulaLeft/','V:/Zevis_240nm_data/Subregion/DiencephalonHabenulaLeft/',9,8,15,'uint8',3);
DownScaleZeVisData('V:/Zevis_120nm_data/Subregion/DiencephalonHabenulaRight/','V:/Zevis_240nm_data/Subregion/DiencephalonHabenulaRight/',9,8,15,'uint8',3);
DownScaleZeVisData('V:/Zevis_120nm_data/Subregion/MesencephalonTecumNeuropil/','V:/Zevis_240nm_data/Subregion/MesencephalonTecumNeuropil/',9,8,15,'uint8',3);

DownScaleZeVisCellData('V:/Zevis_120nm_data/Brain/','V:/Zevis_240nm_data/Brain/',0.5);


DownScaleZeVisData('V:/Zevis_240nm_data/Brain/','V:/Zevis_480nm_data/Brain/',4,4,7,'uint32',2);
DownScaleZeVisData('V:/Zevis_240nm_data/EM/','V:/Zevis_480nm_data/EM/',4,4,7,'uint8',2);
DownScaleZeVisData('V:/Zevis_240nm_data/Subregion/Brainmask/','V:/Zevis_480nm_data/Subregion/Brainmask/',4,4,7,'uint8',2);
DownScaleZeVisData('V:/Zevis_240nm_data/Subregion/DiencephalonHabenulaLeft/','V:/Zevis_480nm_data/Subregion/DiencephalonHabenulaLeft/',4,4,7,'uint8',2);
DownScaleZeVisData('V:/Zevis_240nm_data/Subregion/DiencephalonHabenulaRight/','V:/Zevis_480nm_data/Subregion/DiencephalonHabenulaRight/',4,4,7,'uint8',2);
DownScaleZeVisData('V:/Zevis_240nm_data/Subregion/MesencephalonTecumNeuropil/','V:/Zevis_480nm_data/Subregion/MesencephalonTecumNeuropil/',4,4,7,'uint8',2);

DownScaleZeVisCellData('V:/Zevis_240nm_data/Brain/','V:/Zevis_480nm_data/Brain/',0.5);


labelClear('V:/Zevis_240nm_data/Brain/');
labelClear('V:/Zevis_480nm_data/Brain/');

cellClear2('E:/Zevis/BrainLabel_60nm/');
cellClear2('V:/Zevis_240nm_data/Brain/');
cellClear2('V:/Zevis_480nm_data/Brain/');
