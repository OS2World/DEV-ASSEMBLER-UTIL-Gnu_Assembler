
all: as.inf

as.inf: as.texinfo asdoc-config.texi
	texi2ipf as.texinfo >as.ipf
	ipfc /INF as.ipf

clean:
	-rm as.ipf as.inf


