all: pagerank

pagerank: pagerank.cc
	clang++ pagerank.cc -o pagerank -O3 --std=c++17
