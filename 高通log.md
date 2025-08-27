``` bash
adb  pull /sdcard/Logs 
adb  pull /data/anr 
adb  pull /data/tombstones 
adb  pull /data/vendor/tombstones  
adb  pull /data/system/dropbox  
adb  shell pm list packages -f > packageslist.txt

adb  pull /data/vendor/wifi/wpa  
adb  pull /data/local/tmp  
adb  pull /data/vendor/ramdump  
adb  pull /data/misc/radio  radio   
adb  shell dumpsys audio > audio.txt
adb  shell dumpsys media.audio_flinger > media.audio_flinger.txt
adb  shell dumpsys media.audio_policy > media.audio_policy.txt

adb  shell getprop > sysprop.txt
adb  shell cat /proc/cmdline > cmdline.txt
adb  shell dumpsys dropbox --print > dropbox.txt
adb  shell dumpsys window > dumpwindow.txt
adb  shell dumpsys meminfo > meminfodump.txt
adb  shell cat /proc/meminfo >meminfo.txt
adb   bugreport  bugreport.zip
```