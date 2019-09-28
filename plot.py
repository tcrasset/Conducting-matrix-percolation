import pandas as pd 
from matplotlib import pyplot as plt

df = pd.read_csv("output_parallel_8.csv",delimiter=' ', header=0)

# print(df.columns)


# Plot probability as a function of density
legend = []
for index,i in enumerate(df.N.unique()):
    subdf = df[df['N'] == i]
    plt.plot(subdf.d, subdf.Probability)
    legend.append('N = {}'.format(df.N.unique()[index]))
plt.ylabel("Probability of conduction")
plt.xlabel("Density of fibers")
plt.legend(legend, loc  ='lower right')
plt.show()


# Plot average time as a function of density
legend = []
for index,i in enumerate(df.N.unique()):
    subdf = df[df['N'] == i]
    plt.plot(subdf.d, subdf.AvgTime)
    legend.append('N = {}'.format(df.N.unique()[index]))
plt.ylabel("Runtime (ms)")
plt.xlabel("Density of fibers")
plt.legend(legend, loc  ='lower right')
plt.show()


# Plot average time as a function of density
meanTime = []
for index,i in enumerate(df.N.unique()):
    meanTime.append(df[df['N'] == i].AvgTime.mean())
# plt.scatter(df.N.unique(), meanTime, marker='x')
plt.plot(df.N.unique(), meanTime, marker='x',markersize=10)
plt.ylabel("Runtime (ms)")
plt.xlabel("Grid width/height (N)")
plt.show()


# print(df)


