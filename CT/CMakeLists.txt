cmake_minimum_required(VERSION 3.12)
project(CT)

set(CMAKE_CXX_STANDARD 11)

add_executable(CT main.cpp Parser.cpp Parser.h SparseTable.cpp SparseTable.h ScoreAnalyzer.cpp ScoreAnalyzer.h ClusterInitializer.cpp ClusterInitializer.h TreeStructure.h CompatibilityAnalyzer.cpp CompatibilityAnalyzer.h Linked_List_Builder.cpp Linked_List_Builder.h HDT_Structure.h SpanningForest.cpp SpanningForest.h Treap.cpp Treap.h HDT.cpp HDT.h FastCompatibilityTest.cpp FastCompatibilityTest.h InputGenerator.cpp InputGenerator.h)