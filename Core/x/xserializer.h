#ifndef XSERIALIZER_H
#define XSERIALIZER_H

struct st_SERIAL_PERCID_SIZE
{
	unsigned int idtag;
	int needsize;
};

int xSerialStartup(int count, st_SERIAL_PERCID_SIZE *sizeinfo);
int xSerialShutdown();

#endif