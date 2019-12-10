# Compilers Final Project - Concolic Testing Engine

This project contains the code used in Will and I's final compilers project. The project uses an LLVM transformation pass to instrument LLVM bit code. The resultant new program outputs the programs symbolic constraints as well as their runtime evaluation. We use these constraints as input to a constraint solver, which computes a test input that will drive new path exploration in the instrumented program. This process can be repeated to explore whole programs efficiently.

## Project Sections

* [Constraint Solver](./Constraint_Solver/) - Contains the constraint solver which was used in this project.
* [LLVM Passes](./LLVM_Passes/) - Contains both the static and dynamic LLVM passes, which were used to analyze and instrument code, respectively.
* [Test Programs](./Test_Programs) - Contains the test programs and the results which were used to test this project.
* [Report](./Report) - Contains the final report for this project.

## Video

Coming Soon

## Authors

* **Carl Hildebrandt** - *Initial work* - [hildebrandt-carl](https://github.com/hildebrandt-carl)
* **Will Leeson** - *Initial work* - [Platapusbear](https://github.com/PlatapusBear)
