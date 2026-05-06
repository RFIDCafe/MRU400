/*
 * MRU400 RFID Tag Reader
 * Connects to local FEIG reader daemon on 127.0.0.1:10001
 * Outputs JSON tag events to stdout for piping to mosquitto_pub
 */

#include <Fedm.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <csignal>
#include <unistd.h>
#include <ctime>

using namespace std;
using namespace FEDM;

static volatile bool g_running = true;

void signalHandler(int) {
    g_running = false;
}

string getCurrentTimestamp() {
    time_t now = time(nullptr);
    struct tm *tm_info = gmtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", tm_info);
    return string(buf);
}

string makeJson(const string &epc, const string &tid, const string &timestamp) {
    ostringstream ss;
    ss << "{\"epc\":\"" << epc << "\","
       << "\"tid\":\"" << tid << "\","
       << "\"timestamp\":\"" << timestamp << "\"}";
    return ss.str();
}

int main(int argc, char *argv[]) {
    signal(SIGINT,  signalHandler);
    signal(SIGTERM, signalHandler);

    string ipAddr = "127.0.0.1";
    int port = 10001;

    ReaderModule *reader = new ReaderModule(RequestMode::UniDirectional);
    Connector cnn = Connector::createTcpConnector(ipAddr, port);

    int rc = reader->connect(cnn);
    if (rc != ErrorCode::Ok) {
        cerr << "Failed to connect to reader daemon: " << reader->lastErrorStatusText() << endl;
        delete reader;
        return 1;
    }

    rc = reader->tagEvent().initializeReaderBuffer();
    if (rc != ErrorCode::Ok) {
        cerr << "Failed to initialize reader buffer: " << reader->lastErrorStatusText() << endl;
        reader->disconnect();
        delete reader;
        return 1;
    }

    while (g_running) {
        rc = reader->tagEvent().readReaderBuffer(255);

        if (rc < ErrorCode::Ok) {
            cerr << "Read buffer error: " << reader->lastErrorStatusText() << endl;
            break;
        }

        if (rc == ReaderStatus::DataTableNoData) {
            sleep(1);
            continue;
        }

        reader->tagEvent().clearReaderBuffer();

        unique_ptr<const TagEventItem> item = reader->tagEvent().popItem();
        while (item) {
            if (!g_running) break;

            TagItem tag = item->tag();
            string epc = tag.epcC1G2_EpcToHexString();
            string tid = tag.epcC1G2_TidToHexString();
            string ts  = getCurrentTimestamp();

            if (!epc.empty()) {
                cout << makeJson(epc, tid, ts) << endl;
                cout.flush();
            }

            item = reader->tagEvent().popItem();
        }

        sleep(1);
    }

    reader->disconnect();
    delete reader;
    return 0;
}
