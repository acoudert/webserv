NAME          = webserv

DIR_SRC       = src/
DIR_MAIN      = $(DIR_SRC)main/
DIR_SERVER    = $(DIR_SRC)server/
DIR_HANDLER   = $(DIR_SRC)handler/
DIR_REQUEST   = $(DIR_SRC)request/
DIR_RESPONSE  = $(DIR_SRC)response/
DIR_INTERFACE = $(DIR_RESPONSE)interface/
DIR_WORKER    = $(DIR_SRC)worker/
DIR_UTILS     = $(DIR_SRC)utils/
DIR_PATHS     = $(DIR_UTILS)paths/
DIR_ERROR     = $(DIR_SRC)error/
DIR_SIG       = $(DIR_SRC)signal/
DIR_MIMES     = $(DIR_UTILS)mime/

CXX           = clang++
CXXINCLUDES   = -I $(DIR_MAIN)
CXXINCLUDES  += -I $(DIR_SERVER)
CXXINCLUDES  += -I $(DIR_REQUEST)
CXXINCLUDES  += -I $(DIR_WORKER)
CXXINCLUDES  += -I $(DIR_UTILS)
CXXINCLUDES  += -I $(DIR_PATHS)
CXXINCLUDES  += -I $(DIR_ERROR)
CXXINCLUDES  += -I $(DIR_SIG)
CXXINCLUDES  += -I $(DIR_HANDLER)
CXXINCLUDES  += -I $(DIR_REQUEST)
CXXINCLUDES  += -I $(DIR_RESPONSE)
CXXINCLUDES  += -I $(DIR_INTERFACE)
CXXINCLUDES  += -I $(DIR_SIG)
CXXINCLUDES  += -I $(DIR_MIMES)
CXXFLAGS      = -Wall -Wextra -Werror -std=c++98 $(CXXINCLUDES) -D_GLIBCXX_DEBUG -g
RM            = rm -f

SRCS          = src/server/ServerGroup.cpp \
                src/server/Location.cpp \
                src/server/CGI.cpp \
                src/server/Server.cpp \
                src/server/Methods.cpp \
                src/error/error.cpp \
                src/main/Data.cpp \
                src/main/main.cpp \
                src/request/Request.cpp \
                src/request/RequestLine.cpp \
                src/worker/worker.cpp \
                src/signal/sig.cpp \
                src/utils/utils.cpp \
                src/utils/paths/pathUtils.cpp \
                src/utils/mime/mimes.cpp \
                src/utils/libft.cpp \
                src/handler/Buffer.cpp \
                src/handler/Handler.cpp \
                src/response/Response.cpp \
                src/response/ResponseHeader.cpp \
                src/response/interface/AutoIndexGenerator.cpp \
                src/response/interface/CGIGenerator.cpp \
                src/response/interface/EmptyGenerator.cpp \
                src/response/interface/ErrorGenerator.cpp \
                src/response/interface/ErrorResponse.cpp \
                src/response/interface/FDGenerator.cpp \
                src/response/interface/GETResponse.cpp \
                src/response/interface/HEADResponse.cpp \
                src/response/interface/IGenerator.cpp \
                src/response/interface/PUTResponse.cpp


OBJS          = $(SRCS:.cpp=.o)
TEST_OBJS     = $(SRCS:.cpp=.test.o)

SEMB = assets/400.html \
       assets/401.html \
       assets/403.html \
       assets/404.html \
       assets/405.html \
       assets/408.html \
       assets/413.html \
       assets/418.html \
       assets/500.html \
       assets/501.html \
       assets/505.html \
       assets/301.html \
       assets/200.html \
       assets/201.html

OEMB = $(SEMB:.html=.o)

DEBUG_FLAGS = -g3 -Og

all : $(NAME)

$(NAME) : $(OBJS) $(OEMB) assets/mime.types.o
	$(CXX) $^ -o $@ -lpthread -std=c++98
	@echo "\033[0;32m"
	@./$(NAME) --help
	@echo "\033[0m"
	@echo "Segmentation fault (core dumped)"
	@echo "exit 11"

$(OBJS): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

assets/mime.types:;

clean :
	$(RM) $(OBJS) $(TEST_OBJS) $(OEMB) assets/mime.types.o

fclean : clean
	$(RM) $(NAME) $(DEBUG)

re : fclean all

assets/mime.types.o : assets/mime.types
	sort assets/mime.types -o assets/mime.types
	$(LD) -r -b binary assets/mime.types -o assets/mime.types.o -g

$(OEMB) : %.o : %.html
	$(LD) -r -b binary $< -o $@

.PHONY : all clean fclean re
