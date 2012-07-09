****************************************************************

  pulse2pulse

  reads full waveform LIDAR data from a variety of proprietary
  formats and stores it to the PulseWaves format.
 
  For updates you can follow PulseWaves at

  http://pulsewaves.org
  http://twitter.com/laserpulsewaves

  Martin @laserpulsewaves

****************************************************************

example use:

>> pulse2pulse.exe -i lidar.pls -gui
>> pulse2pulse.exe -i lidar.laz -subseq 0 10000 -o lidar.pls 
>> pulse2pulse.exe -i lidar.laz -subseq 0 10000 -o lidar.pls -owvz
>> pulse2pulse.exe -i lidar.lgc -subseq 0 10000 -o lidar.pls -keep_descriptor 0
>> pulse2pulse.exe -i lidar.lgw -subseq 0 10000 -o lidar.pls -clip_first_x_above 630500

for more info:

C:\released_code\pulsetools\bin>pulse2pulse.exe -h
Supported Pulse Inputs
  -i lidar.pls
  -i lidar.plz
  -i lidar.las
  -i lidar.laz
  -i lidar.lgw
  -i lidar.lgc
  -i lidar.sdf  (slow!!!)
  -h
Supported PULSE Outputs
  -o pulse.pls
PULSEtools (by martin.isenburg@gmail.com) version 120614
usage:
pulse2pulse -i las13fwf.las -o out.pls
pulse2pulse -i pulsewaves.pls -o pulsewaves.pls
pulse2pulse -i lvis.lgw -o lvis.pls
pulse2pulse -i geolas.lgc -o geolas.pls
pulse2pulse -i riegl.sdf -o riegl.pls
pulse2pulse -h

---------------

if you find bugs let me (martin.isenburg@gmail.com) know.
