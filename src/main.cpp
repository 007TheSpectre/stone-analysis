#include "analyze/Analyzer.hpp"
#include "steg/Cypher.hpp"
#include "steg/Decypher.hpp"
#include "wav/WavReader.hpp"
#include "wav/WavWriter.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

static void printUsage(const char* prog) {
    std::cout << "USAGE\n"
              << "    " << prog << " [--analyze | -a] IN_FILE N\n"
              << "                     [--cypher  | -c] IN_FILE OUT_FILE MESSAGE\n"
              << "                     [--decypher | -d] IN_FILE\n\n"
              << "DESCRIPTION\n"
              << "    IN_FILE     An audio file to be analyzed\n"
              << "    OUT_FILE    Output audio file of the cypher mode\n"
              << "    MESSAGE     The message to hide in the audio file\n"
              << "    N           Number of top frequencies to display\n";
}

static int runAnalyze(const char* inFile, const char* nArg) {
    int topN = std::atoi(nArg);
    if (topN <= 0) {
        std::cerr << "N must be a positive integer\n";
        return 84;
    }
    WavReader reader;
    reader.open(inFile);
    Analyzer analyzer;
    analyzer.analyze(reader.samples(), topN, 48000);
    return 0;
}

static int runCypher(const char* inFile, const char* outFile, const char* message) {
    WavReader reader;
    reader.open(inFile);
    Cypher cypher;
    std::vector<int16_t> encoded = cypher.encode(reader.samples(), message);
    WavWriter writer;
    writer.write(outFile, reader.rawHeader(), encoded);
    return 0;
}

static int runDecypher(const char* inFile) {
    WavReader reader;
    reader.open(inFile);
    Decypher decypher;
    std::string msg = decypher.decode(reader.samples());
    std::cout << msg << "\n";
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 84;
    }

    std::string mode = argv[1];

    try {
        if (mode == "--help" || mode == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        if (mode == "--analyze" || mode == "-a") {
            if (argc != 4) {
                std::cerr << "Usage: " << argv[0] << " --analyze IN_FILE N\n";
                return 84;
            }
            return runAnalyze(argv[2], argv[3]);
        }
        if (mode == "--cypher" || mode == "-c") {
            if (argc != 5) {
                std::cerr << "Usage: " << argv[0] << " --cypher IN_FILE OUT_FILE MESSAGE\n";
                return 84;
            }
            return runCypher(argv[2], argv[3], argv[4]);
        }
        if (mode == "--decypher" || mode == "-d") {
            if (argc != 3) {
                std::cerr << "Usage: " << argv[0] << " --decypher IN_FILE\n";
                return 84;
            }
            return runDecypher(argv[2]);
        }
        std::cerr << "Unknown mode: " << mode << "\n";
        printUsage(argv[0]);
        return 84;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 84;
    }
}
