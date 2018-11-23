import pandas as pd
import plotly
import plotly.offline as py
import plotly.graph_objs as go
import csv

py.init_notebook_mode(connected=True)
file_name = '/Users/douglaslee/Desktop/abs1n50.csv'
data = []  # data means the orginal data,but you have to turn .dat format into .csv format
for line in open(file_name):
    line = line.split()
    data.append(line)
csv_file = csv.reader(open('/Users/douglaslee/Desktop/highcost_H3.abs1n50.csv', 'r'))
deliver = []  ##deliver means the result approach
for a in csv_file:
    deliver.append(a)
for a in data:
    print a
print '...'
x = []
y = []
kucun = []
produce = []
for i in range(1, len(data)):
    x.append(float(data[i][1]))
    y.append(float(data[i][2]))
    if (i == 1):
        produce.append(int(data[i][4]))
        kucun.append(int(data[i][3]))
    else:
        kucun.append(int(data[i][3]))
        produce.append(-int(data[i][6]))
print (x)
print y
print kucun
print produce
period = int(data[0][1])
print 'period=', period
print '...'
putdown = [0] * len(x)
for i in range(period):
    x1 = []
    y1 = []
    deli = []
    for j in range(len(x)):
        kucun[j] = kucun[j] + produce[j]
        putdown[j] = 2
    for k in range(1, len(deliver[3 * i + 1]) - 1):
        num = int(deliver[3 * i + 1][k])
        if (num == 0):
            kucun[num] = kucun[num] - float(deliver[3 * i + 2][k])
        else:
            kucun[num] = kucun[num] + float(deliver[3 * i + 2][k])
            putdown[num] = putdown[num] + float(deliver[3 * i + 2][k])
            deli.append(float(deliver[3 * i + 2][k]))
        x1.append(x[num])
        y1.append(y[num])
    print kucun
    x1.append(x[0])
    y1.append(y[0])
    txt = []
    for i in range(len(x)):
        txtItem = []
        txtItem.append(i)
        txtItem.append(kucun[i])
        txt.append(txtItem)
    tmp = kucun[0]
    kucun[0] = 70
    putdown[0] = 'blue'
    print '.....', i
    node = go.Scatter(
        x=x,
        y=y,
        # text=[[1,2],1,2,3,4,5],
        text=txt,
        mode='markers'
    )
    storage = go.Scatter(
        x=x,
        y=y,
        marker=dict(
            size=kucun,
            color=putdown,
            showscale=True,
            cauto=True,
            opacity=0.5  # transparent
        ),
        mode='markers'
    )

    if (len(x1) != 0):
        route = go.Scatter(
            x=x1,
            y=y1,
            text=deli
        )
        py.plot([node, storage, route])
    else:
        py.plot([node, storage])
    kucun[0] = tmp


