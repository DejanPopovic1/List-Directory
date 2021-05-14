#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>
using namespace std;

struct node{
  string name;
  string type;
  unsigned long long size;
  unsigned long long time;
};

bool compareNode(const node &a, const node &b){
   int res = a.name.compare(b.name);
   if(res < 0)
     return true;
   else
     return false;
}

bool isFile(struct stat statbuf){
    return !S_ISDIR(statbuf.st_mode);
}

bool isDir(struct stat statbuf){
    return S_ISDIR(statbuf.st_mode);
}

void listdir(const char *pathToDir, vector<node> &result, bool lastSlash){
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    struct tm *tm;
    time_t rawtime;
    if (!(dir = opendir(pathToDir))){
        cout << "Could not open the file or directory" << pathToDir << "\n";
        return;
    }
    if (!(entry = readdir(dir))){
        cout << "Could not read the file or directory" << pathToDir << "\n";
        return;
    }
    do{
        string slash = "";
        if(!lastSlash){
          slash = "/";
        }
        string parent(pathToDir);
        string file(entry->d_name);
        string final = parent + slash + file;
        if(stat(final.c_str(), &statbuf) == -1){
            cout << "Couldn't get the stat info of file or dir: " << final << "\n";
            return;
        }
        tm = localtime(&statbuf.st_mtime);
        unsigned long long time = mktime(tm);
        if (isDir(statbuf)){
            //skip the . and .. directory
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
            struct node tmp;
            tmp.name = final;
            tmp.type = "D";
            tmp.size = 0;
            tmp.time = time;
            result.push_back(tmp);
            listdir(final.c_str(), result, false);
        }
        else if(isFile(statbuf)){
            struct node tmp;
            tmp.name = final;
            tmp.type = "F";
            tmp.size = statbuf.st_size;
            tmp.time = time;
            result.push_back(tmp);
        }
    } while (entry = readdir(dir));
    closedir(dir);
}

int main(int argc, char *argv[]){
    if(argc != 3){
      cout << "Invalid arguments. Valid use of arguments includes the following pattern: \n";
      cout << "./ListDirectory.exe <Directory> <isSorted>\n";
      cout << "Directory: Path to any dir or file\n";
      cout << "isSorted: 0 = normal output | 1 = sorted output\n";
      cout << "Output of program includes <PathToDir>,<File or Directory>,<If file, size of file>,<UNIX format time of creation>";
      return 0;
    }
    vector<node> result;
    bool lastSlash = false;
    if(argv[1][strlen(argv[1]) - 1] == '/'){
      lastSlash = true;
    }
    listdir(argv[1], result, lastSlash);
    if(string(argv[2]).compare(string("1")) == 0){
        sort(result.begin(), result.end(), compareNode);
    }
    for(int i = 0; i<result.size(); i++){
      cout << result[i].name << "," << result[i].type << "," << result[i].size << "," << result[i].time << "\n";
    }
    return 0;
}
