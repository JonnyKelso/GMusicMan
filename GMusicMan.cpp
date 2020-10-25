#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>

namespace fs = std::filesystem;
using std::string;
using std::wstring;
using std::cout;
using std::endl;

//std::vector<string> known_music_paths = { "F:\\iTunes_Music" };
struct Library
{
    std::vector<wstring> music_files;
    std::vector<wstring> missing_files;
    std::vector<wstring> ext_library;
    std::vector<wstring> found_files;
    std::vector<wstring> error_files;
};
string data_path = "";
std::string getMetaData(std::filesystem::path& path);
int process_args(int arg_count, char* arg_array[]);
void buildDictionaryOfExistingMusic(std::vector<wstring>& library);
void findFilenameInExtLibrary(std::unique_ptr<Library>& library);
void writeVectorToFile(std::string filename, Library& library);



int process_args(int arg_count, char* arg_array[])
{
    if(arg_count == 1)
    {
        cout << "Usage: \n";
        cout << "\t " << arg_array[0] << " <path>" << endl;
        return 1;
    }
    if(arg_count > 1)
    {
        data_path = arg_array[1];
        if (!fs::is_directory(fs::status(data_path)))
        {
            cout << "Path [ " << data_path << " ] is not a directory" << endl;
            cout << "Please supply a path to the music files." << endl;
            return 1; 
        }
    }
    return 0;
}
void buildDictionaryOfExistingMusic(std::vector<wstring>& library)
{
    std::wifstream ext_lib_file("sumo_mp3.txt");
    if(ext_lib_file.is_open())
    {
        wstring line;
        while(ext_lib_file.good())
        {
            std::getline(ext_lib_file, line);
            library.push_back(line);
        }
    }
    else
    {
        cout << "Couldn't read library file" << endl;
    }
}
void findFilenameInExtLibrary(std::unique_ptr<Library>& library)
{
   /* std::filesystem::path found_copies(data_path + "\found_copies");
    if(!std::filesystem::exists(found_copies))
    {
        if(!std::filesystem::create_directory(found_copies))
        {
            cout << "Couldn't create a new directory for fouind copies of files." << endl;
            return;
        }
    }*/
    int remaining = library->missing_files.size();
    for(auto look_for_str : library->missing_files)
    {   
        std::filesystem::path look_for_path(look_for_str);
        cout << "----------------------------------------------------" << endl;
        cout << remaining << " remaining." << endl;
        cout << "* " << getMetaData(look_for_path) << endl;
        //std::wcout << "* " << look_for_path.stem() << endl; 
        bool found_file = false;
        int file_count = 0;
        for(auto line : library->ext_library)
        {
            file_count++;
            std::filesystem::path path(line);
            if(path.filename().wstring().find(look_for_path.stem().wstring().substr(0,10)) != wstring::npos)
            {
                if(found_file == true)
                {
                    cout << "found file AGAIN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    std::cin.get();
                }
                std::wcout << "* " << look_for_path.stem() << endl; 
                std::wcout << "-> " << path.filename().wstring() << '\n';
                std::wcout << "-> (" << file_count << ") Found: " << line << '\n';
                
                string accept;
                cout << "Do you want to accept this match?";
                getline (std::cin, accept);
                if(accept == "Y" || accept == "y")
                {
                    library->found_files.push_back(line);
                    found_file = true;
                    std::filesystem::path new_path = look_for_path;
                    new_path += ".found";
                    //cout << "Renaming: " << look_for_path << " to " << new_path << endl; 
                    std::filesystem::rename(look_for_path, new_path);
                    if(std::filesystem::exists(new_path))
                    {
                        cout << "Renamed to " << new_path << endl;
                    }
                    else
                    {
                        cout << "Error: rename FAILED" << endl;
                    }
                    remaining--;
                    break;
                }
                else if(accept == "S" || accept == "s")
                {
                    // skip
                    break;
                }
                else if(accept == "D" || accept == "d")
                {
                    // don't care
                    std::filesystem::path new_path = look_for_path;
                    new_path += ".deleteme";
                    std::filesystem::rename(look_for_path, new_path);
                    if(std::filesystem::exists(new_path))
                    {
                        cout << "Renamed to " << new_path << endl;
                    }
                    else
                    {
                        cout << "Error: rename FAILED" << endl;
                    }
                    remaining--;
                    break;
                }
            }
        }
        if(!found_file)
        {
            library->error_files.push_back(look_for_str);
        }
    }
    cout << "Found " << library->found_files.size() << " files" << endl;
    cout << "Did not find " << library->error_files.size() << " files" << endl;

}
std::string getMetaData(std::filesystem::path& path)
{
    std::ifstream meta_file(path);
    std::string header;
    std::string data;
    if(meta_file.is_open())
    {
        std::getline(meta_file, header);
        std::getline(meta_file, data);
    }
    else
    {
        cout << "Couldn't open metadata file: " << path << endl;
    }
    return data;
}
void writeVectorToFile(std::string filename, Library& library)
{
    std::ofstream outFile(filename);
    if(outFile.is_open())
    {
        for(auto line : library.error_files)
        {
            std::wcout << line << '\n';
        }
    }
    else
    {
        cout << "Could not open outout file: " << filename << endl;
    }
}
int main(int argc, char* argv[])
{
    int args_result = process_args(argc, argv);
    if(args_result > 0)
    {
        return args_result;
    }
    cout << "OK" << '\n';

    auto myLibrary = std::make_unique<Library>();
    buildDictionaryOfExistingMusic(myLibrary->ext_library);
    int file_count = 0;
    int missing_file_count = 0;
    int music_file_count = 0;
    cout << "Looking in: " << data_path << '\n';
    for(const auto& entry : fs::directory_iterator(data_path))
    {
        file_count++;
        if(entry.path().extension() == ".csv")
        {
            myLibrary->missing_files.push_back(entry.path().wstring());
        }
        else if(entry.path().extension() == ".mp3")
        {
            myLibrary->music_files.push_back(entry.path().wstring());
        }
    }
    missing_file_count = myLibrary->missing_files.size();
    music_file_count = myLibrary->music_files.size();

    cout << "Found: " << file_count << " files.\n";
    cout << "Found: " << music_file_count << " music files.\n";
    cout << "Found: " << missing_file_count << " metadata files.\n";
    cout << "Found: " << music_file_count + missing_file_count << " total files.\n";
    if(file_count == (missing_file_count + music_file_count))
    {
        cout << "Read ALL files." << endl;
    }
    else
    {
        cout << "Some files not read!!!!!!!!!" << endl;
    }
    //findFilenameInExtLibrary(myLibrary->ext_library, myLibrary->missing_files, myLibrary->found_files);
    findFilenameInExtLibrary(myLibrary);
}
