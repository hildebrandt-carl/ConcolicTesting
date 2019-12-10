import sys
from z3 import *
import re
import random

tests = []

#We create symbolic ints for Z3
#They must match, so we generate
#16. Ideally, this will be enough
#symbolic variables. This can be 
#extended to floats
a,b,c = Ints('a b c') 
d,e,f = Ints('d e f')
g,h,i = Ints('g h i')
j,k,l = Ints('j k l')

#This regex checks if the constraint is a variable
varRe = re.compile("\D\w*")

#We generate random input to start this process
first_input = []
print("How many inputs values does this program take:")
numValues = input()
for i in range(int(numValues)):
    first_input.append(random.randint(-10000,10000))
print("Your values are:", first_input)

print("Input file name or 0 to stop:")
fileName = input()
constraintsTested = [] 

# Changes constraints in string representation to reflect negation of constraint
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

#While we still have input files do the following
s=Solver()
while fileName != "0":
    inFile = open(fileName)

    #Maps the variable to a corresponding symbolic variable
    curVar = ord('a')
    varMap = dict()
    constraints = []
    operand1 = 0
    operand2 = 0

    for line in inFile:
    	#If the line is a constraint line, create the constraint
        if "Constraint Hit:" in line:
            fullConstraint = line.split()
            operand1 = fullConstraint[2]
            operand2 = fullConstraint[4]

            #If the variable is a input variable, map it to a constraint
            #If the variable has not been used before, map it to a constraint
            #If it hasn't, map it to a new constraint
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

        #If the predicate was true, keep it the operator. Otherwise, flip it
        if "Predicate Evaluation" in line:
            if "True" in line:
                constraint = [operand1,fullConstraint[3],operand2]
            else:
                constraint = [operand1,flip_boolean_operator(fullConstraint[3]),operand2]
            constraints.append(constraint)
    
    #For each constraint (starting at the last one, flip the operator. If this set of constraints haven't been evaluated,
    #evaluate them. Otherwise, try flipping the next one
    for myIter in range(len(constraints)):
        flipped_constraint = [constraints[len(constraints)-myIter-1][0],flip_boolean_operator(constraints[len(constraints)-myIter-1][1]),constraints[len(constraints)-myIter-1][2]] 
        new_constraints = []
        new_constraints.append(constraints[:len(constraints)-myIter-1] + [flipped_constraint] + constraints[len(constraints)-myIter:])
        if new_constraints not in constraintsTested:
            break

    #Join the portions of the constraint and add them not the solver
    for aConstraint in new_constraints[0]:
        s.add(eval("".join(aConstraint)))

    #Check if the constrains are solvable, if they are append them to solved constraints and
    #produce a model. Then ask for input to restart process
    print(s.check())
    tests.append(s.model())
    constraintsTested.append(new_constraints)
    print("New Input:", tests[len(tests)-1])
    print("Input file name or 0 to stop:")
    fileName = input()

#Prints the tests run at the end
numTest=1
for test in tests:
    print("Test", numTest, test)
    numTest+=1
