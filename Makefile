SRCS = src/main.cpp src/utils.cpp src/flags.cpp src/tui.cpp
OBJS = $(SRCS:src/%.cpp=obj/%.o)
PRNAME = lf

$(PRNAME): $(OBJS)
	clang++ $(OBJS) -o $(PRNAME) -Iinclude -lncurses -lmenu -lform -Wall -Wpedantic -O2

obj/%.o: src/%.cpp
	clang++ -c $< -o $@ -Iinclude

.PHONY: clean
clean: 
	rm -f $(OBJS)
