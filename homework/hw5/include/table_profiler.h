#ifndef TABLE_PROFILER_H
#define TABLE_PROFILER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

void initReport(const char* filename, const char* title);
void addGroup(const char* filename, const char* groupname, const double values[][5], int num_rows);
void finaliseReport(const char* filename);

#endif // TABLE_PROFILER_H