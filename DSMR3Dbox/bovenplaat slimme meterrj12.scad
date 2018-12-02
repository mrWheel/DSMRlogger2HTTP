xmax=40.7;
ymax=66;
dikte=2;
$fn=25;
zmax=dikte;
gat_top=5.0;
hoeksupport=8;
difference()
{union()
{//translate([dikte*1.13,dikte*1.13,0])
cube([xmax,ymax,dikte]);
   // klein stukje uit de hoek
 translate([1.5,1.5,0])
   cylinder(r=2,h=zmax);
 translate([1.5,ymax-1.5,0])
   cylinder(r=2,h=zmax);
 translate([xmax-1.5,1.5,0])
   cylinder(r=2,h=zmax);
 translate([xmax-1.5,ymax-1.5,0])
   cylinder(r=2,h=zmax);       
}
translate([25,10,1])
    linear_extrude(height=2)
    rotate([0,0,90])
    text("  DSMR",size=5,halign=1,valign=1);
 translate([gat_top,gat_top,0]) 
     cylinder(h=zmax+1,d=3);
 translate([xmax-gat_top,6,0])  
     cylinder(h=zmax+1,d=3);
 translate([gat_top,ymax-gat_top,0])  
     cylinder(h=zmax+1,d=3);
 translate([xmax-gat_top,ymax-gat_top,0])  
     cylinder(h=zmax+1,d=3);
 
    // venstertje om led beter te zien
   translate([20,40,0.4])
   cube([6,6,3]);
     translate([10,10,1])
    linear_extrude(height=2)
    rotate([0,0,90])
    text("//ESP01-DSMR.local",size=3.5,halign=1,valign=1);
    translate([xmax-4,13,1])
   linear_extrude(height=2)
    rotate([0,0,90])
text("Slimme meter",size=5,halign=1,valign=1);  }
 

    
    
 