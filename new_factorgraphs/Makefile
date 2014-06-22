#  ===============================================================
#
#  Makefile for the Bayesian optimization Algorithm (sBOA) with
#  decision graphs
#
#  author:        Martin Pelikan
#
#  last modified: August 2000
#
#  ===============================================================
#
#  usage:
#         make fast      - compile what needed only 
#
#         make clean     - cleans object files
#
#         make all       - the same as make optimized
#
#         make tar.Z     - create a .tar.Z archive of the files 
#                          for transfering the sources
#
# -----------------------------------------------------------------

# ----------------------------------------------------------------------
# use the name of your C++ compiler instead of gcc in the following line
# ----------------------------------------------------------------------

CC = g++

# ------------------------------------------------------------------
# chose optimization level you want to have your code optimized with
# ------------------------------------------------------------------

OPTIMIZE  = -O3

OPT = -O2
EXTRA_LIB_FLAGS = -larmadillo
LIB_FLAGS = $(EXTRA_LIB_FLAGS)
CXXFLAGS = $(OPT)

# --------------------------------
# no need to go beyond these lines
# --------------------------------

FLAG     =  
LFLAG    = -lm 

CPP   = args.cc                   \
        bayesian.cc               \
        boa.cc                    \
	WindFarmLayout.cpp	  \
	decisionGraph.cc          \
        fitness.cc                \
	frequencyDecisionGraph.cc \
        getFileArgs.cc            \
        graph.cc                  \
        header.cc                 \
        help.cc                   \
	labeledTreeNode.cc        \
        main.cc                   \
        mymath.cc                 \
	operator.cc               \
        population.cc             \
        random.cc                 \
        replace.cc                \
        select.cc                 \
        stack.cc                  \
        startUp.cc                \
        statistics.cc             \
        utils.cc

OBJS  = args.o                   \
        bayesian.o               \
        boa.o                    \
	WindFarmLayout.o	 \
	decisionGraph.o          \
        fitness.o                \
	frequencyDecisionGraph.o \
        getFileArgs.o            \
        graph.o                  \
        header.o                 \
        help.o                   \
	labeledTreeNode.o        \
        main.o                   \
        mymath.o                 \
	operator.o               \
        population.o             \
        random.o                 \
        replace.o                \
        select.o                 \
        stack.o                  \
        startUp.o                \
        statistics.o             \
        utils.o

#
# make boa creates an executable file called 'boa'
#
boa: clean
	$(CC) -o boa $(CPP) $(LFLAG) $(OPTIMIZE) $(CXXFLAGS) $(LIB_FLAGS)

fast: $(OBJS)
	$(CC) -o boa $(OBJS) $(LFLAG) $(OPTIMIZE)

all: clean
	make boa

K2.o: K2.cc
	$(CC) $(FLAG) K2.cc

args.o: args.cc
	$(CC) $(FLAG) args.cc

bayesian.o: bayesian.cc
	$(CC) $(FLAG) bayesian.cc

boa.o: boa.cc
	$(CC) $(FLAG) boa.cc

WindFarmLayout.o: WindFarmLayout.cpp
	$(CC) $(FLAG) WindFarmLayout.cc

checkCycles.o: checkCycles.cc
	$(CC) $(FLAG) checkCycles.cc

computeCounts.o: computeCounts.cc
	$(CC) $(FLAG) computeCounts.cc

fitness.o: fitness.cc
	$(CC) $(FLAG) fitness.cc

getFileArgs.o: getFileArgs.cc
	$(CC) $(FLAG) getFileArgs.cc

graph.o: graph.cc
	$(CC) $(FLAG) graph.cc

header.o: header.cc
	$(CC) $(FLAG) header.cc

help.o: help.cc
	$(CC) $(FLAG) help.cc

main.o: main.cc
	$(CC) $(FLAG) main.cc

mymath.o: mymath.cc
	$(CC) $(FLAG) mymath.cc

population.o: population.cc
	$(CC) $(FLAG) population.cc

random.o: random.cc
	$(CC) $(FLAG) random.cc

recomputeGains.o: recomputeGains.cc
	$(CC) $(FLAG) recomputeGains.cc

replace.o: replace.cc
	$(CC) $(FLAG) replace.cc

select.o: select.cc
	$(CC) $(FLAG) select.cc

stack.o: stack.cc
	$(CC) $(FLAG) stack.cc

startUp.o: startUp.cc
	$(CC) $(FLAG) startUp.cc

statistics.o: statistics.cc
	$(CC) $(FLAG) statistics.cc

utils.o: utils.cc
	$(CC) $(FLAG) utils.cc

#
# make clean removes all .o files
#

clean: 
	@rm -f $(OBJS)

#
# make compressed tar file containing the source code
#

tar.Z:
	@tar cvf sBOA.tar *.cc *.h examples/input.* examples/output.* Makefile README COPYRIGHT ;
	@compress sBOA.tar
