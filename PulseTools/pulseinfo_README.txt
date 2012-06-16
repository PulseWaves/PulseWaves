****************************************************************

  pulseinfo

  reads full waveform LIDAR data from a variety of proprietary
  formats and outputs a textual description of their content.
 
  For updates you can follow PulseWaves at

  http://pulsewaves.org
  http://twitter.com/laserpulsewaves

  Martin @laserpulsewaves

****************************************************************

for more info:

C:\released_code\pulsetools\bin>pulseinfo -h
Supported Pulse Inputs
  -i lidar.pls
  -i lidar.plz
  -i lidar.las
  -i lidar.laz
  -i lidar.lgw
  -i lidar.lgc
  -i lidar.sdf
  -h
PULSEtools (by martin.isenburg@gmail.com) version 120614
usage:
pulseinfo -i pulsewaves.pls -no_check
pulseinfo -i las13fwf.las -o lidar_info.txt
pulseinfo -i lvis.lgw -no_header -no_vlrs
pulseinfo -i geolas.lgc -stdout
pulseinfo -i riegl.sdf
pulseinfo -i *.pls -repair

---------------

if you find bugs let me (martin.isenburg@gmail.com) know.
