CLEANFILES = iCS iCSD

install: all
all: iCS
iCS: ../src/iCS
	cp ../src/iCS ./`echo iCS | sed '$(transform)'`
	if [ ! -e iCS -a -e iCSD ]; then ln -s iCSD iCS; fi
