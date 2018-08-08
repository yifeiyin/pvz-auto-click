#ifndef EXECCOMMAND_H
#define EXECCOMMAND_H

std::string exec(std::string cmd) {

    std::string data;
    std::FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != nullptr)
                data.append(buffer);
        pclose(stream);
    }
    return data;
}

#endif // EXECCOMMAND_H
