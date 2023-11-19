#include <fstream>

#define BUFF_SIZE 5012

int read(std::string path, char *buffer, std::size_t * bytes = 0) {
    /*
    Reads raw contents of file. And stores length.
    path: location/filename of file.
    buffer: destination buffer for data.
    bytes: (optional) address of size_t object to store length of file-content. 
    */

    //open file as fstream object, read mode
    std::fstream file (path, std::fstream::in);
    
    if (file.is_open()) {

        //Get length of file
        file.seekg(0, std::ios_base::end);
        std::size_t length = file.tellg();
        file.seekg(0, std::ios_base::beg);

        //output num of bytes in file to storage val
        if (bytes != 0) {
            *bytes = length;
        }
        
        //fill buffer array with clean memory
        std::fill(buffer, buffer + length, NULL);

        //read file-data to buffer
        file.read(buffer, length);
    }
    //close file
    file.close();

    return 0;
}

int read(std::string file_name, std::string directory, char *buffer, std::size_t * bytes = 0) {
    /*
    Reads raw contents of file. And stores length.
    file_name: name of file.
    directory: directory of file. Relative or absolute.
    buffer: destination buffer for data. 
    */

    //construct path to file
    std::string path_str = directory + file_name; 

    std::cout << path_str << std::endl;

    //open file as fstream object, read mode
    std::fstream file (path_str, std::fstream::in);
    if (file.is_open()) {

        //Get length of file
        file.seekg(0, std::ios_base::end);
        std::size_t length = file.tellg();
        file.seekg(0, std::ios_base::beg);

        //output num of bytes in file to storage val
        if (bytes != 0) {
            *bytes = length;
        }
        
        //fill buffer array with clean memory
        std::fill(buffer, buffer + length, NULL);

        //read file-data to buffer
        file.read(buffer, length);
    }
    //close file
    file.close();

    return 0;
}

//looks for file "file_name" in target directory "directory"
//Param "directory" = path to folder to search, relative or complete.
//Returns true if found, false if not. 
//Returns val of GetLastError in other cases.
int find_file(std::string file_name, std::string directory = "") {
    WIN32_FIND_DATAA fileData;
    HANDLE hFind;

    //Construct path to file with func inputs
    std::string path_str = directory + file_name; 

    char path[path_str.length() + 1];
    strcpy(path, path_str.c_str());

    //std::cout << path << std::endl;
    
    //look for file given by constructed "path"
    hFind = FindFirstFileA(path, &fileData); 
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return (true);
    }

    else if (GetLastError() == ERROR_FILE_NOT_FOUND) {
        FindClose(hFind);
        return (false);
    }

    else {
        FindClose(hFind);
        return GetLastError();
    }
}

int img_read(std::string file_name, std::string directory, char *buffer, std::size_t * bytes = 0) {
    /*
    Reads raw contents of file. And stores length.
    path: location/filename of file.
    buffer: destination buffer for data.
    bytes: (optional) address of size_t object to store length of file-content. 
    */
    std::cout << "IMG READ" << std::endl;
    std::string path = directory + file_name; 

    //open file as fstream object, read mode
    std::ifstream file (path, std::ifstream::binary | std::ifstream::in);
    
    if (file.is_open()) {

        //Get length of file
        file.seekg(0, std::ios_base::end);
        std::size_t length = file.tellg();
        file.seekg(0, std::ios_base::beg);

        //output num of bytes in file to storage val
        if (bytes != 0) {
            *bytes = length;
        }
        
        //fill buffer array with clean memory
        std::fill(buffer, buffer + BUFF_SIZE, NULL);

        //read file-data to buffer
        file.read(buffer, BUFF_SIZE);
    }
    //close file
    file.close();

    return 0;
}

/* EXPERIMENTAL
int extract(char * file_name, char * buffer) {

    //open file as fstream object, read mode
    std::fstream file (file_name, std::fstream::in);

    //extract contents of file
    while (file) {
        file >> buffer;
        std::cout << buffer;
    }

    //close file
    file.close();

    return 0;
}
*/

  
