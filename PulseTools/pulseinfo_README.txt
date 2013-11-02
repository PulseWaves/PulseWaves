****************************************************************

  pulseinfo

  reads full waveform LIDAR data from PulseWaves format and also
  a variety of other proprietary formats and outputs a textual
  description of their content.
 
  For updates you can follow PulseWaves at

  http://pulsewaves.org
  http://twitter.com/laserpulsewaves

  Martin @laserpulsewaves

****************************************************************

example use:

>> pulseinfo.exe -version
>> pulseinfo.exe -i lidar.pls -gui
>> pulseinfo.exe -i lidar.laz
>> pulseinfo.exe -i lidar.csd
>> pulseinfo.exe -i lidar.lgc -o lidar.txt
>> pulseinfo.exe -i lidar.sdf -clip_first_x_above 630500
>> pulseinfo.exe -i lidar.lgw -no_var -clip_first_x_above 630500

for more info:

C:\pulsetools\bin>pulseinfo -h
Filter pulses based on first and last sample.
  -clip_tile 631000 4834000 1000 (ll_x ll_y size)
  -clip_circle 630250.00 4834750.00 100 (x y radius)
  -clip_box 620000 4830000 100 621000 4831000 200 (min_x min_y min_z max_x max_y max_z)
  -clip 630000 4834000 631000 4836000 (min_x min_y max_x max_y)
  -clip_x_below 630000.50 (min_x)
  -clip_y_below 4834500.25 (min_y)
  -clip_x_above 630500.50 (max_x)
  -clip_y_above 4836000.75 (max_y)
  -clip_z 11.125 130.725 (min_z, max_z)
  -clip_z_below 11.125 (min_z)
  -clip_z_above 130.725 (max_z)
Filter pulses based on first sample.
  -clip_first 630000 4834000 631000 4836000 (min_x min_y max_x max_y)
  -clip_first_x_below 630000.50 (min_x)
  -clip_first_y_below 4834500.25 (min_y)
  -clip_first_x_above 630500.50 (max_x)
  -clip_first_y_above 4836000.75 (max_y)
  -clip_first_z 11.125 130.725 (min_z, max_z)
  -clip_first_z_below 11.125 (min_z)
  -clip_first_z_above 130.725 (max_z)
Filter pulses based on last sample.
  -clip_last 630000 4834000 631000 4836000 (min_x min_y max_x max_y)
  -clip_last_x_below 630000.50 (min_x)
  -clip_last_y_below 4834500.25 (min_y)
  -clip_last_x_above 630500.50 (max_x)
  -clip_last_y_above 4836000.75 (max_y)
  -clip_last_z 11.125 130.725 (min_z, max_z)
  -clip_last_z_below 11.125 (min_z)
  -clip_last_z_above 130.725 (max_z)
Filter pulses based on anchor point.
  -clip_anchor 630000 4834000 631000 4836000 (min_x min_y max_x max_y)
  -clip_anchor_x_below 630000.50 (min_x)
  -clip_anchor_y_below 4834500.25 (min_y)
  -clip_anchor_x_above 630500.50 (max_x)
  -clip_anchor_y_above 4836000.75 (max_y)
  -clip_anchor_z 11.125 130.725 (min_z, max_z)
  -clip_anchor_z_below 11.125 (min_z)
  -clip_anchor_z_above 130.725 (max_z)
Filter pulses based on the scanline flags.
  -drop_scan_direction 0
  -scan_direction_change_only
  -edge_of_scan_line_only
Filter pulses based on their time stamp.
  -keep_time 11.125 130.725
  -drop_time_below 11.125
  -drop_time_above 130.725
  -drop_time_between 22.0 48.0
Filter pulses based on their pulse descriptor.
  -keep_descriptor 1 2
  -drop_descriptor 0
Filter pulses based on scan direction or mirror facet.
  -keep_scan_direction 1
  -drop_scan_direction 0
  -keep_facet 1 3
  -drop_facet 0
Filter pulses with simple thinning.
  -keep_every_nth 2
  -keep_random_fraction 0.1
  -thin_with_grid 1.0
Transform coordinates.
  -translate_x -2.5
  -translate_z 42.1
  -translate_xyz 0.5 0.5 0.01
  -switch_x_y
  -switch_y_z
  -switch_x_z
Transform raw XYZ integers.
  -translate_raw_Z 20
  -translate_raw_XYZ 2 2 0
Transform intensity.
  -scale_intensity 2.5
  -translate_intensity 50
  -translate_then_scale_intensity 0.5 3.1
Modify the classification.
  -set_classification_to 2
  -change_classification_from_to 2 4
Modify the pulse source ID.
  -set_pulse_source_to 500
  -change_pulse_source_from_to 1023 1024
Supported Pulse Inputs
  -i lidar.pls
  -i lidar.plz
  -i lidar.lgw
  -i lidar.lgc
  -i lidar.sdf
  -i lidar.las
  -i lidar.laz
  -h
PULSEtools (by martin.isenburg@rapidlasso.com) version 0.3 rev 8 (131102)
usage:
pulseinfo -i pulsewaves.pls -no_check
pulseinfo -i las13fwf.las -o lidar_info.txt
pulseinfo -i lvis.lgw -no_header -no_vlrs
pulseinfo -i geolas.lgc -stdout
pulseinfo -i riegl.sdf
pulseinfo -i *.pls -repair

---------------

if you find bugs let me (martin.isenburg@rapidlasso.com) know.
