CXX          = c++
CXXFLAGS     = -std=c++17 -Wall -Wextra -Werror
TESTCXXFLAGS = -std=c++17 -Wall -Wextra
GCOVR        = gcovr --filter 'src/.*' --object-directory obj/cov -r .

NAME       = stone_analysis
NAME_TESTS = stone_analysis_tests

# --- Production sources (no main.cpp) ---
SRCS =	src/wav/WavReader.cpp    \
	src/wav/WavWriter.cpp    \
	src/dft/Fft.cpp          \
	src/dft/Dft.cpp          \
	src/dft/Idft.cpp         \
	src/steg/CharMap.cpp     \
	src/steg/Cypher.cpp      \
	src/steg/Decypher.cpp    \
	src/analyze/Analyzer.cpp

MAIN_SRC = src/main.cpp

# --- Test sources ---
TEST_SRCS =	tests/test_complex.cpp  \
		tests/test_dft.cpp      \
		tests/test_charmap.cpp  \
		tests/test_cypher.cpp   \
		tests/test_wav.cpp      \
		tests/test_analyze.cpp

# --- Object lists ---
OBJS      = $(SRCS:src/%.cpp=obj/%.o)
MAIN_OBJ  = obj/main.o
COV_OBJS  = $(SRCS:src/%.cpp=obj/cov/%.o)
TEST_OBJS = $(TEST_SRCS:tests/%.cpp=obj/tests/%.o)

COVFLAGS = --coverage

# ---- Main binary ----
all: $(NAME)

$(NAME): $(OBJS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -Isrc -c $< -o $@

# ---- Test binary (coverage-instrumented) ----
$(NAME_TESTS): $(COV_OBJS) $(TEST_OBJS)
	$(CXX) $(TESTCXXFLAGS) $(COVFLAGS) -o $@ $^ -lcriterion

obj/cov/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -Isrc -c $< -o $@

obj/tests/%.o: tests/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(TESTCXXFLAGS) $(COVFLAGS) -Isrc -c $< -o $@

# ---- Tests + coverage ----
tests_run:  $(NAME_TESTS)
	./$(NAME_TESTS)
	@printf '\n--- Line coverage ---\n'
	$(GCOVR) --txt-metric line
	@printf '\n--- Branch coverage ---\n'
	$(GCOVR) --txt-metric branch --print-summary

functional_tests: $(NAME)
	bash tests/functional.sh

docs:
	doxygen Doxyfile

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME) $(NAME_TESTS)
	rm -rf docs/doxygen

re: fclean all

.PHONY: all clean fclean re tests_run functional_tests docs
