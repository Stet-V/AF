#ifndef TABLEPROFILER_H
#define TABLEPROFILER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef struct Report {
    FILE* file;
    int groupCounter;
} Report;

Report* initReport(const char* filename, const char* title, const char** headers, const int num_headers) {
    Report* report = new Report;
    if (!report) {
        return nullptr;
    }

    report->file = fopen(filename, "w");
    if (!report->file) {
        delete report;
        return nullptr;
    }

    report->groupCounter = 0;

    fprintf(report->file,
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
        "            <tr>\n",
        title,
        title
    );

    fprintf(report->file,
        "                <th>Group</th>\n"
    );

    for (int i = 0; i < num_headers; i++) {
        fprintf(report->file, "                <th>%s</th>\n", headers[i]);
    }

    fprintf(report->file,
        "            </tr>\n"
        "        </thead>\n"
        "        <tbody>\n"
    );
    
    return report;
}

void addGroup(Report* report, const char* groupname, const float values[][5], const int num_rows) {
    if (!report || !report->file) {
        return;
    }

    for (int i = 0; i < num_rows; i++) {
        fprintf(report->file,
            "            <tr class=\"group-%d\">\n",
            report->groupCounter % 8
        );
        if (i == 0) {
            fprintf(report->file,
                "                <td rowspan=\"%d\">%s</td>\n",
                num_rows,
                groupname
            );
        }
        fprintf(report->file,
            "                <td>%.2f</td>\n"
            "                <td>%.2f</td>\n"
            "                <td>%.2f</td>\n"
            "                <td>%.2f</td>\n"
            "                <td>%.2f</td>\n"
            "            </tr>\n",
            values[i][0],
            values[i][1],
            values[i][2],
            values[i][3],
            values[i][4]
        );
    }
    report->groupCounter++;
}

void addGroup(Report* report, const char* groupname, const int values[][5], const int num_rows) {
    if (!report || !report->file) {
        return;
    }

    for (int i = 0; i < num_rows; i++) {
        fprintf(report->file,
            "            <tr class=\"group-%d\">\n",
            report->groupCounter % 8
        );
        if (i == 0) {
            fprintf(report->file,
                "                <td rowspan=\"%d\">%s</td>\n",
                num_rows,
                groupname
            );
        }
        fprintf(report->file,
            "                <td>%d</td>\n"
            "                <td>%d</td>\n"
            "                <td>%d</td>\n"
            "                <td>%d</td>\n"
            "                <td>%d</td>\n"
            "            </tr>\n",
            values[i][0],
            values[i][1],
            values[i][2],
            values[i][3],
            values[i][4]
        );
    }
    report->groupCounter++;
}

void closeReport(Report* report) {
    if (!report) {
        return;
    }

    if (report->file) {
        fprintf(report->file,
            "        </tbody>\n"
            "    </table>\n"
            "</body>\n"
            "</html>\n"
        );
        fclose(report->file);
    }
    delete report;
}

#endif // TABLE_PROFILER_H