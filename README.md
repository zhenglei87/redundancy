# redundancy
4:1 redundancy
USAGE:"a.exe -s filename", 
  split filename to 5 files, with file name suffixed by "[0~4].s", each file has no meaningful value
USAGE:"a.exe -r filename", 
  recover filename.[0~4].s to filename.s, 4 files in 5 is needed
