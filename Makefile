#this works, good enough
D=OpenProjectCreator
C=BaseParser.cpp   ConditionEvaluator.cpp  MacroStore.cpp         ParserHelpers.cpp  Values.cpp               main.cpp \
BaseProject.cpp  ExtendedVPCParser.cpp   OperatorFunctions.cpp  VPCParser.cpp      VisualStudioProject.cpp
O=$(patsubst %.cpp,obj/%.o,$(C))


bin/opc: $(O)
	mkdir -p bin
	$(CXX) $(CCFLAGS) $(LDFLAGS) $^ -o $@

obj/%.o: $(D)/%.cpp
	mkdir -p obj
	$(CXX) $(CCFLAGS) -c $^ -o $@


