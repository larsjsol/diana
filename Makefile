#--------------------------------
# Toplevel Makefile for DIANA
#--------------------------------

# includefile contains Compiler definitions
include ../conf/${OSTYPE}.mk

SRCDIR=src
LIBDIR=lib$(PLT)
OBJDIR=obj$(PLT)
BINDIR=bin$(PLT)
INCDIR=../include
LOCALINC=$(LOCALDIR)/include
LANGDIR=lang

DEPENDSFILE=make.depends
MOCFILE=make.moc
DEFINES+=-DMETNOFIELDFILE  -DMETNOPRODDB -DMETNOOBS \
	$(NETCDFDEF) $(BUFROBSDEF) $(LOGGDEF) $(CORBADEF)

ifdef WEATHERDB
DEFINES += -DWEATHERDB
WEATHERDB_LIB=-ldiWeatherDB
WEATHERDB_EXTRA_LIB=-lboost_thread \
                    $(shell pkg-config --libs libpqxx) \
                    -L$(shell pg_config --libdir)  -lpq \
                    -lboost_date_time
endif

PROFETLIBS=-lpropoly -lprofet
ifdef OMNIORB_INST
PROFETLIBS+= -lpods
endif

INCLUDE= -I. \
	 -I$(INCDIR) \
	 -I$(LOCALINC)/propoly \
	 -I$(LOCALINC)/profet \
	 -I$(LOCALINC)/profetSQL \
	 -I$(LOCALINC)/proFunctions \
	 -I$(LOCALINC)/qUtilities-coclient2 \
	 -I$(LOCALINC)/puDatatypes \
	 -I$(LOCALINC)/glp \
	 -I$(LOCALINC)/glText \
	 -I$(LOCALINC)/robs \
	 -I$(LOCALINC)/diMItiff \
	 -I$(LOCALINC)/diField \
	 -I$(LOCALINC)/milib \
	 -I$(LOCALINC)/diSQL \
	 -I$(LOCALINC)/puSQL \
	 -I$(LOCALINC)/puTools \
	 -I$(LOCALINC)/puCtools \
	 -I$(LOCALINC)          \
	 $(PNGINCLUDE) \
	 $(QTINCLUDE) \
	 $(GLINCLUDE) \
	 $(MYSQLINCLUDE) \
	 $(TIFFINCLUDE) \
	 $(FTGLINCLUDE) \
	 $(FTINCLUDE) \
	 $(SHAPEINCLUDE) \
	 $(XINCLUDE) \
	 $(OMNI_INCLUDE)


# Note: PNG library included in the Qt library (also used in batch version)

# WARNING: library sequence may be very important due to path (-L) sequence

LINKS = -L$(LOCALDIR)/$(LIBDIR) $(PROFETLIBS) \
	-lqUtilities-coclient2 -lpuDatatypes \
	-lGLP -lglText -lrobs -ldiMItiff -ldiField -lprofet -lprofetSQL -lproFunctions -lmic -ldiSQL -lpuSQL \
	$(WEATHERDB_LIB) \
	-lpuTools \
	-L$(QTDIR)/lib $(QT_LIBS) \
	$(GLLIBDIR) -lGL -lGLU $(GLXTRALIBS) \
	$(MYSQLLIBDIR) -lmysqlclient \
	$(TIFFLIBDIR) -ltiff \
	$(FTGLLIBDIR) -lftgl $(FTLIBDIR) -lfreetype \
	$(SHAPELIBDIR) -lshp \
	$(XLIBDIR) -lXext -lXmu -lXt -lX11 \
	-L$(LOCALDIR)/$(LIBDIR) -lmi \
	-L$(EMOSLIBDIR) -lemos \
	$(LOGGLIBS) \
	$(F2CLIB) -lm \
	$(UDUNITSLIB) \
	$(NETCDFLIB) \
	$(WEATHERDB_EXTRA_LIB) \
	$(OMNI_LIBS)


BLINKS= $(LINKS)

OPTIONS="CXX=${CXX}" "CCFLAGS=${CXXFLAGS} ${DEFINES}" \
	"CC=${CC}" "CFLAGS=${CFLAGS} ${DEFINES}" \
	"LDFLAGS=${CXXLDFLAGS}" "INCLUDE=${INCLUDE}" \
	"DEPENDSFILE=${DEPENDSFILE}" "BINDIR=../${BINDIR}" \
	"LOCALDIR=${LOCALDIR}" "INCDIR=${INCDIR}" \
	"LINKS=${LINKS}" "BLINKS=${BLINKS}" \
	"QTDIR=${QTDIR}" "MOCFILE=${MOCFILE}" "LANGDIR=../${LANGDIR}"


all: directories mocs depends mark diana bmark bdiana


nodep: mark diana bmark bdiana

directories:
	if [ ! -d $(OBJDIR) ] ; then mkdir $(OBJDIR) ; fi
	if [ ! -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	cd $(OBJDIR) ; ln -sf ../$(SRCDIR)/*.cc .
	cd $(OBJDIR) ; ln -sf ../$(SRCDIR)/*.c .
	cd $(OBJDIR) ; ln -sf ../$(SRCDIR)/Makefile .
	if [ ! -f $(OBJDIR)/$(DEPENDSFILE) ] ; \
	then touch $(OBJDIR)/$(DEPENDSFILE) ; fi
	if [ ! -f $(OBJDIR)/$(MOCFILE) ] ; \
	then touch $(OBJDIR)/$(MOCFILE) ; fi
	cd $(OBJDIR); make $(OPTIONS) build

mocs:
	cd $(OBJDIR); make $(OPTIONS) mocs

languages:
	if [ ! -d $(LANGDIR) ] ; then mkdir $(LANGDIR) ; fi
	cd $(SRCDIR); make $(OPTIONS) languages

depends:
	cd $(OBJDIR); make $(OPTIONS) depends

mark:
	@echo  '[1;31mMaking diana ----------[0m'

diana:
	cd $(OBJDIR); make $(OPTIONS) mkdiana

bmark:
	@echo '[1;31mMaking bdiana -----------[0m'

bdiana:
	cd $(OBJDIR); make $(OPTIONS) mkbdiana

pretty:
	find . \( -name 'core' -o -name '*~' \) -exec rm -f {} \;

binclean:
	rm $(BINDIR)/diana
	rm $(BINDIR)/bdiana

clean:
	@make pretty
	cd $(OBJDIR); rm -f *.o

veryclean:
#	@make pretty
	rm -rf $(OBJDIR)

# install
DESTNAME=diana
COPYFILES=bin/diana bin/bdiana diana.png diana.sh dianaTEST.sh
COPYDIRS=doc
COPYTREES=fonts images etc lang
BINLINKS=diana.sh bin/diana bin/bdiana
TESTDESTNAME=dianaTEST
TESTBINLINKS=dianaTEST.sh 

include ../conf/install.mk















