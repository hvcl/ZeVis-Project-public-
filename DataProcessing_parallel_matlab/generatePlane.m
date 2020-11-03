P1=[4769 3484 12045];
P2=[4859 2129 12045];
P3=[4722 874 5000];
P4=[4769 2302 5000];


normal1 = cross(P1-P2, P1-P3);
syms x y z
P = [x,y,z];
planefunction1 = dot(normal, P-P1);

normal2 = cross(P4-P2, P4-P1);
syms x y z
P = [x,y,z];
planefunction2 = dot(normal, P-P4);

normal3 = cross(P4-P3, P4-P1);
syms x y z
P = [x,y,z];
planefunction3 = dot(normal, P-P4);

normal4 = cross(P4-P3, P4-P2);
syms x y z
P = [x,y,z];
planefunction4 = dot(normal, P-P4);

normal1=-normal1/norm(normal1);
normal2=normal2/norm(normal2);
normal3=normal3/norm(normal3);
normal4=normal4/norm(normal4);

normal=(normal1+normal2+normal3+normal4)/4;
P=(P1+P2+P3+P4)/4;


normal5 = cross(P1-(P3+P4)/2, P1-P2);
normal5=normal5/norm(normal5)


normal6 = cross(P3-(P1+P2)/2, P3-P4);
normal6=normal6/norm(normal6)


P1=[4750 1472 4000];
P2=[4773 1745 8000];
P3=[4768 2587 12000];

normal1 = cross(P1-P2, P1-P3);
normal1=-normal1/norm(normal1);
syms x y z
P = [x,y,z];
planefunction1 = dot(normal, P-P1);