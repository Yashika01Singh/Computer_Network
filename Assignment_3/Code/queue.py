import matplotlib.pyplot as plt
fd=open("tcp-example.tr","r")

enque_time = {}
delay_time={}

for i in fd:

    line = i.split()
    
    time = float(line[1])    
    
  
    
    data = line[2]
    node = data[10]
    if(node!="1"):
    	     continue
    seq = line[36]
    seq = seq[4:]
    index = float(seq)
    if line[0]=='+':
    	enque_time[index] = time
    
    if line[0] == '-':
    	delay_time[time] = 1000*(time-enque_time[index] )
  
plt.xlabel("Dequeue Time seconds ")
plt.ylabel(" Delay Time ms ")
plt.title("Queuing Delay at Node 1")
plt.scatter(delay_time.keys(), delay_time.values())
plt.grid()
plt.show()

