#ifndef PARSER_H
#define PARSER_H

#include "network.h"

network parse_network_cfg(char *filename);
void load_weights(network *net, char *filename);
#endif
