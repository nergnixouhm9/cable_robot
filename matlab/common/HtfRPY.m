function mat = HtfRPY(angle)

c1 = cos(angle(1)); s1 = sin(angle(1));
c2 = cos(angle(2)); s2 = sin(angle(2));
mat = zeros(3);
mat(1,2) = -s1;
mat(1,3) = c1*c2;
mat(2,2) = c1;
mat(2,3) = s1*c2;
mat(3,1) = 1;
mat(3,3) = -s2;

end