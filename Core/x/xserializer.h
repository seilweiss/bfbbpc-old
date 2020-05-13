#ifndef XSERIALIZER_H
#define XSERIALIZER_H

struct st_SERIAL_PERCID_SIZE
{
    unsigned int idtag;
    int needsize;
};

struct st_SERIAL_CLIENTINFO
{
	unsigned int idtag;
	int *membuf;
	int trueoff;
	int actsize;
};

struct xSerial
{
	unsigned int idtag;
	int baseoff;
	st_SERIAL_CLIENTINFO *ctxtdata;
	int warned;
	int curele;
	int bitidx;
	int bittally;

	int Write_b1(int bits);
	int Read_b1(int *bits);
};

typedef int(*xSerialTraverseCallBack)(unsigned int clientID, xSerial *xser);

int xSerialStartup(int count, st_SERIAL_PERCID_SIZE *sizeinfo);
int xSerialShutdown();
void xSerialTraverse(xSerialTraverseCallBack func);
void xSerialWipeMainBuffer();

#endif