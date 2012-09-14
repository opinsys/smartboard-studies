import os
 
# Fork 15 processes and run lsusb in a loop in each one of them
 
for _ in range(15):
    if not os.fork():
        for _ in range(1000):
            os.system("lsusb -v")
        break
