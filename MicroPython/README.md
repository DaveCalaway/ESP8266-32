# ESP32_MicroPython_example

**Install:** https://tinyurl.com/y2r8le63  

** Connect:**
```
screen /dev/tty.NAME 115200
```

## RSHELL
Link: https://github.com/dhylands/rshell

**Connection:**
```
rshell -p /dev/tty.NAME
```

**Copy file from pc to ESP32:**
```
cp lol.py /pyboard/
```

**ls file on the ESP32:**
```
ls /pyboard/
```  

**Exe file:**
```
repl
```
next type:
```
import lol.py
```  

**Board name:**
Create a file, called "board.py" and edit:  
```
name="esp32"
```  
now load the file on the board.  
Reboot the rshall.   


## upip
Dock: https://tinyurl.com/y49uhnj3  
Lib list: https://tinyurl.com/q3q6dnp  

Copy the example wifi.py on the board.  
Run it
```import wifi.py```  

**Install the packs you need:**  
```  
import upip
upip.install('micropython-umqtt.simple')
```  


### Useful commands
- CTRL + D ( reload repl )
- CTRL + X ( exit repl )
