****************************************************************

  pulseview

  reads full waveform LIDAR data from a variety of proprietary
  formats and visualizes them with a simple 3D viewer.
 
  For updates you can follow PulseWaves at

  http://pulsewaves.org
  http://twitter.com/laserpulsewaves

  Martin @laserpulsewaves

****************************************************************

Note that for Optech's *.csd files the corresponding *.ndf and
*.idx files need to be renamed to have the same base name as the
CSD file or the tools will not be able to find them.

* use the right mouse button to make a menu pop up
* switch between <f>irst and <l>ast samples with keys 'f' and 'l'
* press 'W' to turn the display of a 50 pulses on and off.
* use the key 'i' to select a particular sample 

for more info:

C:\released_code\pulsetools\bin>pulseview -h
Supported Pulse Inputs
  -i lidar.pls
  -i lidar.plz
  -i lidar.las
  -i lidar.laz
  -i lidar.lgw
  -i lidar.lgc
  -i lidar.csd
  -i lidar.sdf  (slow!!!)
  -h
PULSEtools (by martin.isenburg@gmail.com) version 120614
usage:
pulseview -i pulsewaves.pls -pulses 200000
pulseview -i las13fwf.las -win 1600 1200
pulseview -i lvis.lgw
pulseview -i geolas.lgc
pulseview -i riegl.sdf
pulseview -h

---------------

if you find bugs let me (martin.isenburg@gmail.com) know.
