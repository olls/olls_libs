all: demo

demo: *.cpp *.h
	$(CXX) -g $< -o $@