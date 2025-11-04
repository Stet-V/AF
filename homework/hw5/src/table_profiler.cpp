#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "table_profiler.h"

int groupCounter = 0;

void initReport(const char* filename, const char* title) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        return;
    }

    fprintf(file, 
        "<!DOCTYPE html>\n"
        "<html lang=\"en-US\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>%s</title>\n"
        "    <style>\n"
        "        body { font-family: Arial; margin: 20px; background: #f4f4f4; }\n"
        "        h1 { text-align: center; color: #2c3e50; }\n"
        "        table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n"
        "        th, td { border: 1px solid #ddd; padding: 12px; text-align: center; }\n"
        "        th { background: #3498db; color: white; }\n"
        "        .group-0 { background: #e0f7fa; }\n"
        "        .group-1 { background: #e8f5e9; }\n"
        "        .group-2 { background: #fff9c4; }\n"
        "        .group-3 { background: #ffe0b2; }\n"
        "        .group-4 { background: #f3e5f5; }\n"
        "        .group-5 { background: #e1f5fe; }\n"
        "        .group-6 { background: #ffcdd2; }\n"
        "        .group-7 { background: #dcedc8; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>%s</h1>\n"
        "    <table>\n"
        "        <thead>\n"
        "            <tr>\n"
        "                <th>Group</th>\n"
        "                <th>Filling Factor</th>\n"
        "                <th>Avg Effort (found)</th>\n"
        "                <th>Max Effort (found)</th>\n"
        "                <th>Avg Effort (not found)</th>\n"
        "                <th>Max Effort (not found)</th>\n"
        "            </tr>\n"
        "        </thead>\n"
        "        <tbody>\n",
        title, 
        title
    );
    fclose(file);
}

void addGroup(const char* filename, const char* groupname, const double values[][5], int num_rows) {
    FILE* file = fopen(filename, "a");
    if (!file) {
        return;
    }

    for (int i = 0; i < num_rows; i++) {
        fprintf(file,
            "            <tr class=\"group-%d\">\n",
            groupCounter % 8
        );
        if (!i) {
            fprintf(file, 
                "                <td rowspan=\"%d\">%s</td>\n",
                num_rows,
                groupname
            );
        }
        fprintf(file,
            "                <td>%.2f</td>\n"
            "                <td>%.2f</td>\n"
            "                <td>%d</td>\n"
            "                <td>%.2f</td>\n"
            "                <td>%d</td>\n"
            "            </tr>\n",
            values[i][0],
            values[i][1],
            (int)(values[i][2]),
            values[i][3],
            (int)(values[i][4])
        );
    }
    groupCounter++;
    fclose(file);
}

void finaliseReport(const char* filename) {
    FILE* file = fopen(filename, "a");
    if (!file) {
        return;
    }
    fprintf(file,
        "       </tbody>\n"
        "   </table>\n"
        "</body>\n"
        "</html>\n"
    );
    fclose(file);
}