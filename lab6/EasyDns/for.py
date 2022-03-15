
import subprocess

i=0
f = open("dnsrelay.txt")               # 返回一个文件对象 
line = f.readline()               # 调用文件的 readline()方法 
while line: 
    line2=line.split(" ")
    print(line2[0]+str(i))
    # if(line2[0]=="0.0.0.0"):
    #     i+=1
    #     line = f.readline() 
    #     continue
    print(line2[1])
    line2[1]=line2[1].strip()
    command='nslookup -port=6801 '+line2[1] +' 127.0.0.1'
    # command='nslookup -port=6801  '+line2[1] + ' 127.0.0.1'
    subprocess.call(command, shell=True)

    line = f.readline() 

    
 
f.close()  