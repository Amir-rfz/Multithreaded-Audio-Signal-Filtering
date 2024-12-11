CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS = -lsndfile -lpthread

SRCDIR = src
INCDIR = include
OBJDIR = build
BINDIR = .

TARGET = $(BINDIR)/VoiceFilters

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEADERS = $(wildcard $(INCDIR)/*.h)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
