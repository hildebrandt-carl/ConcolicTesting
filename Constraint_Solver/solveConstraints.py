import sys
from z3 import *
import re
import random


tests = []
a,b,c = Ints('a b c') 
d,e,f = Ints('d e f')
g,h,i = Ints('g h i')
j,k,l = Ints('j k l')

varRe = re.compile("\D\w*")

first_input = []
print("How many inputs values does this program take:")
numValues = input()
for i in range(int(numValues)):
    first_input.append(random.randint(-10000,10000))
print("Your values are:", first_input)

print("Input file name or 0 to stop:")
fileName = input()
constraintsTested = [[[['a', '>', '0'], ['a', '>', '5'], ['b', '!=', '0']]]] 

def flip_boolean_operator(operator):
    if operator == "==":
        return "!="
    elif operator == "!=":
        return "=="
    elif operator == "<":
        return ">="
    elif operator == "<=":
        return ">"
    elif operator == ">":
        return "<="
    elif operator == ">=":
        return "<"

s=Solver()
while fileName != "0":
    inFile = open(fileName)
    curVar = ord('a')
    varMap = dict()
    constraints = []
    operand1 = 0
    operand2 = 0
    for line in inFile:
        if "Constraint Hit:" in line:
            fullConstraint = line.split()
            operand1 = fullConstraint[2]
            operand2 = fullConstraint[4]

            if varRe.match(operand1):
                if operand1 in varMap:
                    operand1 = varMap[operand1]
                else:
                    varMap[operand1] = chr(curVar)
                    operand1 = chr(curVar)
                    curVar+=1
            if varRe.match(operand2):
                if operand2 in varMap:
                    operand2 = varMap[operand2]
                else:
                    varMap[operand2] = chr(curVar)
                    operand2 = chr(curVar)
                    curVar+=1

        if "Predicate Evaluation" in line:
            if "True" in line:
                constraint = [operand1,fullConstraint[3],operand2]
            else:
                constraint = [operand1,flip_boolean_operator(fullConstraint[3]),operand2]
            constraints.append(constraint)
    
    for myIter in range(len(constraints)):
        flipped_constraint = [constraints[len(constraints)-myIter-1][0],flip_boolean_operator(constraints[len(constraints)-myIter-1][1]),constraints[len(constraints)-myIter-1][2]] 
        new_constraints = []
        new_constraints.append(constraints[:len(constraints)-myIter-1] + [flipped_constraint] + constraints[len(constraints)-myIter:])
        print(constraints[:len(constraints)-myIter-1])
        print([flipped_constraint])
        print(constraints[len(constraints)-myIter:])
        print(new_constraints)
        if new_constraints not in constraintsTested:
            break
    for aConstraint in new_constraints[0]:
        print(aConstraint)
        print("".join(aConstraint))
        s.add(eval("".join(aConstraint)))

    if (s.check() == sat): 
        print(s.check())
        tests.append(s.model())
        constraintsTested.append(new_constraints)
    else:
        print(s.check())
        break
    print("New Input:", tests[len(tests)-1])
    print("Input file name or 0 to stop:")
    fileName = input()

print(constraintsTested)
print(tests)