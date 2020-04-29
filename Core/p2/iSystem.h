#ifndef ISYSTEM_H
#define ISYSTEM_H

struct RwMemoryFunctions;

RwMemoryFunctions *psGetMemoryFunctions();

void iSystemInit(unsigned int options);

#endif