
#--------------------------------------------------------------------

CC = gcc
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC
SOFLAGS = -shared
LDFLAGS = -lstdc++

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

#--------------------------------------------------------------------

LIBOBJS = spjsonutils.o spjsoncodec.o spjsonevent.o spjsonreader.o \
		spjsonparser.o spjsonnode.o spjsoniterator.o spjsondomparser.o \
		spjsonhandle.o spjsonrpc.o sppbfield.o sppbcodec.o

TARGET =  libspjson.so libspjson.a \
		testpull testdom testhandle testrpc testprotobuf

#--------------------------------------------------------------------

all: $(TARGET)

libspjson.so: $(LIBOBJS)
	$(LINKER) $(SOFLAGS) $^ -o $@

libspjson.a: $(LIBOBJS)
	$(AR) $@ $^

testpull: testpull.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspjson -o $@

testdom: testdom.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspjson -o $@

testhandle: testhandle.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspjson -o $@

testrpc: testrpc.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspjson -o $@

testprotobuf: testprotobuf.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspjson -o $@

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

