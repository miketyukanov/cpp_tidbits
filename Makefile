CCPP=g++
CCPPFLAGS=-Wall -std=c++17
CLEAN=rm -f
EXEC=cpp_tidbits 
OBJECT_FILES=*.o
SOURCE_FILES=main.cpp cont_string.cpp templates.cpp member_calls_ctor.cpp lambdas.cpp 

$(EXEC): $(OBJECT_FILES)
	$(CCPP) $(CCPPFLAGS) -o $@ $^

$(OBJECT_FILES): $(SOURCE_FILES)
	$(CCPP) $(CCPPFLAGS) -c $^

clean: 
	$(CLEAN)  *.o $(EXEC)


