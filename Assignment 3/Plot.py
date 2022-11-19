from matplotlib import pyplot as plt

fd = open('tcp-example3.tr', 'r')
lines = fd.readlines()

coordinates = []
enqueue = []
dequeue = []

for i in range (len(lines)-1):
    
    lst = lines[i].split(' ')

    if (lst[2] == '/NodeList/1/DeviceList/1/$ns3::PointToPointNetDevice/TxQueue/Enqueue'):
        enq = float(lst[1])
        enqueue.append(enq)
        
    if (lst[2] == '/NodeList/1/DeviceList/1/$ns3::PointToPointNetDevice/TxQueue/Dequeue'):
        dequeue.append(float(lst[1]))


for i in range(len(enqueue)-1):
    diff = float(dequeue[i] - enqueue[i])
    lst = []
    
    lst.append(dequeue[i])
    lst.append(diff)
    coordinates.append(lst)


x, y = zip(*coordinates)

plt.plot(x, y)
plt.title('Queuing Delay vs Time')
plt.xlabel('Time')
plt.ylabel('Queuing Delay')
plt.show()

