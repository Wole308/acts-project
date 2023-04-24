#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include "../include/common.h"

class algorithm {
public:
	algorithm();
	~algorithm();
	
	unsigned int vertex_initdata(unsigned int Algo, unsigned int index);
	unsigned int get_algorithm_id(std::string Algo);
private:
};
#endif








