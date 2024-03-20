#!/usr/bin/env python3

import subprocess
import re
import random
from itertools import permutations
import random


test_count = 1000000
input = "new\nit 1\nit 2\nit 3\nit 4\n"
for i in range(test_count):
    input += "shuffle\n"
input += "free\nquit\n"


command='./qtest -v 3'
clist = command.split()
completedProcess = subprocess.run(clist, capture_output=True, text=True, input=input)
s = completedProcess.stdout
startIdx = s.find("l = [1 2 3 4]") 
endIdx = s.find("l = NULL")
s = s[startIdx + 14 : endIdx]
Regex = re.compile(r'\d \d \d \d')
result = Regex.findall(s)

def permute(nums):
    nums=list(permutations(nums,len(nums)))
    return nums

def chiSquared(observation, expectation):
    return ((observation - expectation) ** 2) / expectation 

  
nums = []
for i in result:
    nums.append(i.split())


counterSet = {}
shuffle_array = ['1', '2', '3', '4']
s = permute(shuffle_array)


for i in range(len(s)):
    w = ''.join(s[i])
    counterSet[w] = 0


for num in nums:
    permutation = ''.join(num)
    counterSet[permutation] += 1
        

expectation = test_count // len(s)
c = counterSet.values()
chiSquaredSum = 0
for i in c:
    chiSquaredSum += chiSquared(i, expectation)
print("Expectation: ", expectation)
print("Observation: ", counterSet)
print("chi square sum: ", chiSquaredSum)


import matplotlib.pyplot as plt
import numpy as np
permutations = counterSet.keys()
counts = counterSet.values()

x = np.arange(len(counts))
plt.bar(x, counts)
plt.xticks(x, permutations)
plt.xlabel('permutations')
plt.ylabel('counts')
plt.title('Shuffle result')
plt.show()
