#include "global.h"
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>
#include <cstring>
//#include "TDMData.cpp"

class FileManager{

public:
    map<int, vector<int>> die_position;
    map<int, vector<int>> fpga_die;
    vector<pair<int,vector<int>>> source_sink;
    vector<vector<int>> wires;
    string filename = "./TestCase20231027/testcase";
    bool read(string testcase_Num){
        return readFile(testcase_Num)&&
        readFile1(testcase_Num)&&
        readFile2(testcase_Num)&&
        readFile3(testcase_Num);
    }
    bool readFile(string testcaseNum){
        string die_node_filename = filename+ testcaseNum + "/design.die.position";
            int fd = open(die_node_filename.c_str(), O_RDONLY);
            if (fd < 0) {
                cout << "OPEN FILE ERROR!" << endl;
                return false;
            }
            struct stat fileStat;
            if (fstat(fd, &fileStat) == -1) {
                cout << "无法获取文件大小" << endl;
                close(fd);
                return false;
            }

            int fileLength = fileStat.st_size;

            /* 建立映射 */
            char *buffer = (char *) mmap(NULL, fileLength, PROT_READ, MAP_SHARED, fd, 0);
            close(fd);
            if (buffer == MAP_FAILED) {
                cout << "无法映射文件到内存: " << strerror(errno) << endl;
                return false;
            }


            int index = 0;
            bool isleft = true;
            int leftIndex = 0;
            int currentId = 0;
            vector<int> gve;
            for (int i = 0; i < fileLength; i++) {
                if (isleft) {
                    if (buffer[i] >= '0' && buffer[i] <= '9') {
                        leftIndex = leftIndex * 10 + buffer[i] - '0';
                    } else if (buffer[i] == ':') {
                        isleft = false;
                    }
                } else {
                    if (buffer[i] == 'g') {
                        currentId = 0;
                    }
                    if (buffer[i] >= '0' && buffer[i] <= '9') {
                        currentId = currentId * 10 + buffer[i] - '0';
                    } else if (buffer[i] == '\n') {
                        die_position[leftIndex] = gve;
                        gve.clear();
                        isleft = true;
                        currentId = 0;
                        leftIndex = 0;
                    } else if (buffer[i] == ' ') {
                        gve.push_back(currentId);

                    } else {
                    }
                }
            }
        die_position[leftIndex] = gve;
        munmap(buffer, fileLength);
        for(auto pair:die_position){
            cout<<pair.first<<" ";
            const std::vector<int>& positions = pair.second;
            for (int position : positions) {
                cout << "g" << position << " ";
            }
            cout<<endl;
        }

        return true;
    }
    bool readFile1(string testcaseNum){
        string fpga_die_filename = filename+ testcaseNum+"/design.fpga.die";
        int fd = open(fpga_die_filename.c_str(), O_RDONLY);
        if (fd < 0) {
            cout << "OPEN FILE ERROR!" << endl;
            return false;
        }
        struct stat fileStat;
        if (fstat(fd, &fileStat) == -1) {
            cout << "无法获取文件大小" << endl;
            close(fd);
            return false;
        }
        int fileLength = fileStat.st_size;

        /* 建立映射 */
        char *buffer = (char *) mmap(NULL, fileLength, PROT_READ, MAP_SHARED, fd, 0);
        close(fd);
        if (buffer == MAP_FAILED) {
            cout << "无法映射文件到内存: " << strerror(errno) << endl;
            return false;
        }


        int index = 0;
        bool isleft = true;
        int leftIndex = 0;
        int currentId = 0;
        for (int i = 0; i < fileLength; i++) {
            if (isleft) {
                if (buffer[i] >= '0' && buffer[i] <= '9') {
                    leftIndex = leftIndex * 10 + buffer[i] - '0';
                } else if (buffer[i] == ':') {
                    isleft = false;
                }
            } else {
                if (buffer[i] == 'D') {
                    currentId = 0;
                }
                if (buffer[i] >= '0' && buffer[i] <= '9') {
                    currentId = currentId * 10 + buffer[i] - '0';
                } else if (buffer[i] == '\n') {
                    if(currentId!=0) fpga_die[leftIndex].push_back(currentId);
                    isleft = true;
                    currentId = 0;
                    leftIndex = 0;
                } else if (buffer[i] == ' ') {
                    fpga_die[leftIndex].push_back(currentId);
                    currentId = 0;
                }
                else {
                }
            }
        }
        if(currentId!=0) fpga_die[leftIndex].push_back(currentId);
        for(auto pair:fpga_die){
            cout<<pair.first<<" ";
            const std::vector<int>& positions = pair.second;
            for (int position : positions) {
                cout << "die" << position << " ";
            }
            cout<<endl;
        }
        munmap(buffer, fileLength);


        return true;
    }
    bool readFile2(string testcaseNum){
        string net_filename = filename +testcaseNum+"/design.net";
        int fd = open(net_filename.c_str(), O_RDONLY);
        if (fd < 0) {
            cout << "OPEN FILE ERROR!" << endl;
            return false;
        }
        struct stat fileStat;
        if (fstat(fd, &fileStat) == -1) {
            cout << "无法获取文件大小" << endl;
            close(fd);
            return false;
        }

        int fileLength = fileStat.st_size;

        /* 建立映射 */
        char *buffer = (char *) mmap(NULL, fileLength, PROT_READ, MAP_SHARED, fd, 0);
        close(fd);
        if (buffer == MAP_FAILED) {
            cout << "无法映射文件到内存: " << strerror(errno) << endl;
            return false;
        }


        int index = 0;

        int sink_index = 0;
        bool isDigit = false;
        bool isFirst = true;
        pair<int,vector<int>> ss;
        for (int i = 0; i < fileLength; i++) {
            if(isDigit){
                if (buffer[i] >= '0' && buffer[i] <= '9') {
                    index = index * 10 + buffer[i] - '0';
                }
                if(buffer[i]=='l'){
                    ss.second.push_back(index);
                    index = 0;
                    isDigit = false;
                }
                else if(buffer[i]=='s'){
                    if(!isFirst){
                        source_sink.push_back(ss);
                    }
                    ss.first = index;
                    ss.second.clear();
                    index = 0;
                    isDigit = false;
                    isFirst = false;
                }
            }
            else {
                if(buffer[i]=='g'||buffer[i]=='\0'){
                    isDigit = true;
                }
            }
        }
        if(ss.second.size()>0) source_sink.push_back(ss);

        for(int i=0;i<source_sink.size();i++){
            cout<<"Net"<<i;
            pair<int,vector<int>> pair1 = source_sink[i];
            cout<<" sourceNode"<<pair1.first;
            cout<<" sinkNode";
            for(auto a:pair1.second){
                cout<<a<<" ";
            }
            cout<<endl;
        }
        munmap(buffer, fileLength);

        return true;
    }
    bool readFile3(string testcaseNum){
        string die_network_filename = filename+testcaseNum+"/design.die.network";
        int fd = open(die_network_filename.c_str(), O_RDONLY);
        if (fd < 0) {
            cout << "OPEN FILE ERROR!" << endl;
            return false;
        }
        struct stat fileStat;
        if (fstat(fd, &fileStat) == -1) {
            cout << "无法获取文件大小" << endl;
            close(fd);
            return false;
        }
        int fileLength = fileStat.st_size;

        /* 建立映射 */
        char *buffer = (char *) mmap(NULL, fileLength, PROT_READ, MAP_SHARED, fd, 0);
        close(fd);
        if (buffer == MAP_FAILED) {
            cout << "无法映射文件到内存: " << strerror(errno) << endl;
            return false;
        }
        int limit = 0;
        vector<int> ve;
        for (int i = 0; i < fileLength; i++) {
            if(buffer[i]>='0'&&buffer[i]<='9'){
                limit = limit * 10 + buffer[i] - '0';
            }
            else if(buffer[i]=='\n'&&!ve.empty()){
                ve.push_back(limit);
                wires.push_back(ve);
                limit = 0;
                ve.clear();
            }
            else if(buffer[i]=='\n'&&ve.empty()){

            }
            else if(buffer[i]=='\t'||buffer[i]==' '){
                ve.push_back(limit);
                limit = 0;
            }
        }

        munmap(buffer, fileLength);
        string str;
        int row=0;
        int column = 0;
        for(auto line:wires){
            column = 0;
            for (int a:line) {
                cout<<"(";
                cout<<row ;
                cout<<",";
                cout<<column;
                cout<<")";
                cout<< a << " ";
                str += to_string(a)+" ";
                column++;
            }
            str+="\n";
            cout<<endl;
            row++;
        }
//        outputFile(str);
        return true;
    }
    bool outputFile(string data){
        const std::string outputPath = "TestCase20231027/output/output.txt";
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            std::cerr << "无法打开文件 " << outputPath << std::endl;
            return 1;
        }
        file << data; // 使用 std::string 直接写入
        file.close();
        return 0;
    }
};