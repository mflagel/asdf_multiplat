#include "stdafx.h"
#include "utilities.h"

#include <cstring>  //for std::strtok

#include <stdio.h>
#include <zlib.h>
#include <fcntl.h>    //for O_RDONLY AND O_WRONLY, used with tar_open()
#include <libtar.h>

// #include "reflected_struct.h"

// #include "rapidjson/rapidjson.h"
// #include "rapidjson/filewritestream.h"
// #include "rapidjson/writer.h"

#include "main/asdf_multiplat.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

DIAGNOSTIC_IGNORE(-Wcomment)

// using namespace rapidjson;

namespace stdfs = std::experimental::filesystem;
using path = stdfs::path;

namespace asdf {
    namespace util {

    void terminate_handler()
    {
        std::exception_ptr exptr = std::current_exception();
        ASSERT(exptr != nullptr, "");        

        try {
            std::rethrow_exception(exptr);
        }
        catch (const std::exception& e) {
            std::fprintf(stderr, "Terminated due to exception: %s\n", e.what());
        }
        catch (...)
        {
            LOG("Exception Fail...");
        }

        char buffer[1024];
        STRERROR(errno, buffer, 1023);
        LOG("Error: %s", buffer);
        std::abort();
    }

    // SIGINT handler
    void interrupt_handler(int x)
    {
        LOG("\nInterrupt Signal");

        ASDF_UNUSED(x);

        // FIXME for projects that use this file but not asdf_multiplat.cpp
        app.running = false;
    }

    struct file_open_exception : std::exception 
    {
        std::string filepath;
        file_open_exception(std::string const& _filepath)
        : filepath(_filepath)
        {}
        const char* what() const noexcept override
        {
            return ("Unable to open file: " + filepath).c_str();
        }
    };

    std::string read_text_file(std::string const& filepath) {
        if(!is_file(filepath))
        {
            throw file_open_exception(filepath);
        }

        std::string outputstring;
        std::ifstream ifs(filepath, std::ifstream::in);
        ifs.exceptions( std::ios::failbit );

        // ASSERT(ifs.good(), "Error loading text file %s", filepath.c_str());
        if(!ifs.good())
        {
            throw file_open_exception(filepath);
        }

        ifs.seekg(0, std::ios::end);		//seek to the end to get the size the output string should be
        outputstring.reserve(size_t(ifs.tellg()));	//reserve necessary memory up front
        ifs.seekg(0, std::ios::beg);		//seek back to the start

        outputstring.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        ifs.close();

        return outputstring;
    }

    void write_text_file(std::string const& filepath, std::string const& data)
    {
        std::ofstream os;
        os.open(filepath, std::ios::out | std::ios::trunc);

        if(!os.is_open())
        {
            throw file_open_exception(filepath); //todo: use a better exception
        }
        else
        {
            os << data;
            os.close();
        }
    }

    [[deprecated("use std::filesystem")]]
    bool is_directory(std::string const& filepath)
    {
        #ifdef _MSC_VER
        DWORD dwAttrib = GetFileAttributes(filepath.c_str());

        return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) > 0);
#else
        struct stat path_stat;
        if(stat(filepath.c_str(), &path_stat) == 0)
            return S_ISDIR(path_stat.st_mode);
        return false;
#endif
    }

    [[deprecated("use std::filesystem")]]
    bool is_file(std::string const& filepath)
    {
#ifdef _MSC_VER
        DWORD dwAttrib = GetFileAttributes(filepath.c_str());

        return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat path_stat;
        if(stat(filepath.c_str(), &path_stat) == 0)
            return S_ISREG(path_stat.st_mode);
        return false;
#endif
    }

    std::string find_folder(std::string const& name, size_t max_search_dist)
    {
        auto path = name;
        size_t search_dist = 0;

        for(;;)
        {
            if(search_dist == max_search_dist)
            {
                return "";
            }

            //search upwards
            if(is_directory(path))
            {
                return path;
            }
            else
            {
                path.insert(0, "../");
                ++search_dist;
            }
        }
    }

    [[deprecated("use std::filesystem")]]
    void create_dir(std::string const& path)
    {
        ASSERT(!is_directory(path), "Directory already exists at %s", path.c_str());
        ASSERT(!is_file(path), "Cannot create directory, File already exists at %s", path.c_str());


#ifdef MSVC
        ASSERT(path.length() < 248, "Windows CreateDirectory() does not support paths longer than 248 chars. Tell a programmer to use the Unicode version");
        bool status = CreateDirectory(path.c_str());

        /*
        if(status == ERROR_ALREADY_EXISTS)
            todo: throw exception?
        else if(status == ERROR_PATH_NOT_FOUND)
            todo: throw exception?
        */
#else
        EXPLODE("todo: posix version of create_dir()");
#endif
    }


    /*
                       /                            
                   a       x                        
              b                y                    
          c         d             z                 
      e     f     g    h                            


    make E relative to G

    con_e = /a/b/c/e
    con_g = /a/b/d/g

    common = /a/b/
    uncommon_E = c/e
    uncommon_G = d/g

    replace common path with "../" some number of times
    number of "../" is equal to number of elements in uncommon_G


    E relative to G = ../../c/e
    */
    path relative(path const& a, path const& b)
    {
        if(a == b)
            return path();

        using namespace stdfs;

        path con_a = canonical(a);

        // remove the filename otherwise an exception is thrown in UNIX
        // because the file doesn't exist yet. Works fine in window
        // I think
        path con_b = b;
        con_b.remove_filename();
        con_b = canonical(con_b);

        if(con_a == con_b)
            return path();


        if(con_a.root_path() != con_b.root_path())
            return con_a; //no relative path, only absolute

        //split into vector of components so I can access easier
        std::vector<path> components_a;
        std::vector<path> components_b;

        for(auto const& c_a : con_a)
            components_a.push_back(c_a);
        for(auto const& c_b : con_b)
            components_b.push_back(c_b);

//#ifdef _MSC_VER
//        components_a.pop_back();
//        components_b.pop_back();
//#endif

        size_t num_common_components = 0;

        for(size_t i = 0; 
            i < components_a.size() 
         && i < components_b.size()
         && components_a[i] == components_b[i];
            ++i)
        {
            ++num_common_components;
        }

        path result;

        size_t num_upwards = components_b.size() - num_common_components;
        for(size_t i = 0; i < num_upwards; ++i)
            result /= "..";

        for(size_t i = num_common_components; i < components_a.size(); ++i)
        {
            result /= components_a[i];
        }


        return result;
    }

    // enum file_find_direction_e
    // {
    //       file_search_upwards
    //     , file_search_downward
    //     , file_search_outwards ///ie: both up and down
    // };

    path find_file(path const& filename, path const& start_point)
    {
        ASSERT(!filename.empty(), "cannot search for empty filename");

        if(stdfs::exists(filename))
            return filename;

        ASSERT(stdfs::exists(start_point), "invalid starting point for search [%s]", start_point.c_str());
        ASSERT(stdfs::is_directory(start_point), "start point is not a directory [%s]", start_point.c_str());

        for(auto const& p : stdfs::recursive_directory_iterator(start_point))
        {
            if(p.path().filename() == filename)
                return p;
        }

        return path();
    }


//     Document read_json_file(std::string const& filepath)
//     {
//         std::string json = read_text_file(filepath);
//         Document doc;
//         doc.Parse(json.c_str());

//         return std::move(doc);
//     }

//     void write_json_file(rapidjson::Document& d, std::string const& filepath)
//     {
//         //todo: verify file at filepath exists?

// #ifdef _MSC_VER
//         FILE* fp;
//         auto err = fopen_s(&fp, filepath.c_str(), "wb");
//         ASSERT(err == 0, "fopen_s fail!");
// #else
//         FILE* fp = fopen("output.json", "w");
// #endif
//         char writeBuffer[65536];
//         FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
//         Writer<FileWriteStream> writer(os);
//         d.Accept(writer);
//         fclose(fp);
//     }

    std::string get_current_working_directory()
    {
        std::string cwd(256, '\0'); //todo: use a max_filepath macro
        GetCurrentDir(&cwd[0], cwd.length()); //suposedly grabbing the address of std::string[0] works in c++11 onwards
        size_t new_size = 0;
        for (; cwd[new_size] != '\0'; ++new_size);
        cwd.resize(new_size);

        return cwd;
    }





    /// http://www.zlib.net/zlib_how.html
    #if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
    #  include <fcntl.h>
    #  include <io.h>
    #  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
    #else
    #  define SET_BINARY_MODE(file)
    #endif


    // constexpr size_t zlib_chunk_size = 262144; /// 2^20 = 256K
    constexpr size_t zlib_chunk_size = 1048576; /// 2^20 = 256K


    int compress_file(std::experimental::filesystem::path const& src_filepath
                    , std::experimental::filesystem::path const& dest_filepath
                    , int compression_level) noexcept
    {
        ASSERT(stdfs::exists(src_filepath), "file does not exist [%s]", src_filepath.c_str());

        FILE* source = fopen(src_filepath.c_str(),  "r");
        FILE* dest   = fopen(dest_filepath.c_str(), "w");

        int compress_result = compress_file(source, dest, compression_level);

        fclose(source);
        fclose(dest);

        return compress_result;
    }

    /// http://www.zlib.net/zlib_how.html
    int compress_file(FILE* source, FILE* dest, int compression_level) noexcept
    {
        using byte_t = std::byte;

        z_stream strm;
        byte_t in[zlib_chunk_size];
        byte_t out[zlib_chunk_size];

        strm.zalloc = nullptr;
        strm.zfree  = nullptr;
        strm.opaque = nullptr;

        int deflate_status = deflateInit(&strm, compression_level);
        if(deflate_status != Z_OK)
            return deflate_status;

        
        int flush = Z_NO_FLUSH;
        do
        {
            strm.avail_in = fread(in, 1, zlib_chunk_size, source);
            if (ferror(source)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
            flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
            strm.next_in = reinterpret_cast<Bytef*>(in);

            /* run deflate() on input until output buffer not full, finish
               compression if all of source has been read in */
            do
            {
                strm.avail_out = zlib_chunk_size;
                strm.next_out = reinterpret_cast<Bytef*>(out);

                deflate_status = deflate(&strm, flush);
                ASSERT(deflate_status != Z_STREAM_ERROR, "stream error during zlib deflate"); /* state not clobbered */

                size_t n_to_write = zlib_chunk_size - strm.avail_out;
                size_t n_written = fwrite(out, 1, n_to_write, dest);

                if (n_written != n_to_write || ferror(dest)) {
                    (void)deflateEnd(&strm);
                    return Z_ERRNO;
                }
            }
            while(strm.avail_out == 0);
            ASSERT(strm.avail_in == 0, "Not all input was deflated");
        }
        while(flush != Z_FINISH);
        ASSERT(deflate_status == Z_STREAM_END, "zlib stream not finished");

        /* clean up and return */
        (void)deflateEnd(&strm);
        return Z_OK;
    }


    int decompress_file(std::experimental::filesystem::path const& src_filepath
                      , std::experimental::filesystem::path const& dest_filepath) noexcept
    {
        ASSERT(stdfs::exists(src_filepath), "file does not exist [%s]", src_filepath.c_str());

        FILE* source = fopen(src_filepath.c_str(),  "r");
        FILE* dest   = fopen(dest_filepath.c_str(), "w");

        return decompress_file(source, dest);
    }

    /// http://www.zlib.net/zlib_how.html
    int decompress_file(FILE* source, FILE* dest) noexcept
    {
        using byte_t = std::byte;

        z_stream strm;
        byte_t in[zlib_chunk_size];
        byte_t out[zlib_chunk_size];

        strm.zalloc = nullptr;
        strm.zfree  = nullptr;
        strm.opaque = nullptr;

        strm.avail_in = 0;
        strm.next_in = nullptr;

        int inflate_status = inflateInit(&strm);
        if (inflate_status != Z_OK)
            return inflate_status;

        do
        {
            strm.avail_in = fread(in, 1, zlib_chunk_size, source);
            if (ferror(source)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
            if (strm.avail_in == 0)
                break;
            strm.next_in = reinterpret_cast<Bytef*>(in);

            /* run inflate() on input until output buffer not full */
            do
            {
                strm.avail_out = zlib_chunk_size;
                strm.next_out = reinterpret_cast<Bytef*>(out);

                inflate_status = inflate(&strm, Z_NO_FLUSH);
                ASSERT(inflate_status != Z_STREAM_ERROR, "zlib inflate error");

                switch (inflate_status)
                {
                    case Z_NEED_DICT:
                        inflate_status = Z_DATA_ERROR;
                        ///FALLTHROUGH
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        (void)inflateEnd(&strm);
                        return inflate_status;
                }

                size_t n_to_write = zlib_chunk_size - strm.avail_out;
                size_t n_written = fwrite(out, 1, n_to_write, dest);

                if (n_written != n_to_write || ferror(dest)) {
                    (void)deflateEnd(&strm);
                    return Z_ERRNO;
                }
            }
            while(strm.avail_out == 0);
        }
        while(inflate_status != Z_STREAM_END);

        /* clean up and return */
        (void)inflateEnd(&strm);
        return inflate_status == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
    }

    int archive_files(std::vector<stdfs::path> const& filepaths, stdfs::path const& combined_filepath)
    {
        /// if the file doesn't exist yet, create an empty file
        /// for tar to write into
        if(!stdfs::exists(combined_filepath))
        {
            FILE* f = fopen(combined_filepath.c_str(), "w");
            fclose(f);
        }
        else if(!stdfs::is_regular_file(combined_filepath))
        {
            return 1;
        }

        TAR *t;
        int tar_status = tar_open(&t
                            , combined_filepath.c_str()
                            , nullptr   //null tartype uses default file IO functions
                            , O_WRONLY  //must be O_RDONLY or O_WRONLY
                            , 0         // mode?
                            , TAR_NOOVERWRITE | TAR_VERBOSE
                            );

        if(tar_status != 0)
            return tar_status;


        for(auto const& p : filepaths)
        {
            ASSERT(stdfs::exists(p), "archiving a file that does not exist [%s]", p.c_str());

            tar_status = tar_append_file(t, p.c_str(), p.c_str());
            if(tar_status != 0)
                return tar_status;
        }


        tar_status = tar_close(t);
        return tar_status;
    }

    int unarchive_files(stdfs::path const& tar_path, stdfs::path const& extract_dir)
    {
        ASSERT(stdfs::exists(tar_path), "archive does not exist");
        ASSERT(stdfs::is_regular_file(tar_path), "archive is not a valid file");

        ASSERT(stdfs::exists(extract_dir), "extract_dir does not exist");
        ASSERT(stdfs::is_directory(extract_dir), "extract_dir is not a directory");

        TAR *t;
        int tar_status = tar_open(&t
                            , tar_path.c_str()
                            , nullptr   //null tartype uses default file IO functions
                            , O_RDONLY  //must be O_RDONLY or O_WRONLY
                            , 0         // mode?
                            , TAR_VERBOSE
                            );
        if(tar_status != 0)
            return tar_status;

        auto prefix = relative(tar_path.parent_path(), extract_dir);

        /// const_cast required because libtar never takes const char*
        /// as an argument for anything
        tar_status = tar_extract_all(t, const_cast<char*>(prefix.c_str()));
        if(tar_status != 0)
            return tar_status;

        tar_status = tar_close(t);
        return tar_status;
    }



    bool CheckBounds(int x, int y, int minX, int maxX, int minY, int maxY){
	    if(x > maxX || x < minX)
		    return false;
	    if(y > maxY || y < minY)
		    return false;

	    return true;
    }
    bool CheckBounds(int x, int y, int z, int minX, int maxX, int minY, int maxY, int minZ, int maxZ){
	    if(maxY - minY == 0) //if no floors
		    return false;
	    if(x > maxX || x < minX)
		    return false;
	    if(y > maxY || y < minY)
		    return false;
	    if(z > maxZ || z < minZ)
		    return false;

	    return true;
    }

    /// TODO: replace this with a c++11 version
    std::vector<std::string> tokenize(const char* _str, const char* delimiters)
    {
        std::vector<std::string> tokens;
        char* str = strdup(_str);

#ifdef _MSC_VER
        char* next_token = nullptr;

        char* token = strtok_s(str, delimiters, &next_token);
        while (token != nullptr) {
            tokens.push_back(std::string(token));
            token = strtok_s(nullptr, delimiters, &next_token);
        }
#else
        char* token = std::strtok(str, delimiters);
        while (token != nullptr) {
            tokens.push_back(std::string(token));
            token = std::strtok(nullptr, delimiters);
        }
#endif

        free(str);
        return tokens;
    }

    /// FIXME move somewhere specific to SDL or something
    /// otherwise non-sdl projects cant use this file
     void checkSDLError(int line/*=-1*/){
     #ifndef NDEBUG
	     const char *error = SDL_GetError();
	     if (*error != '\0')
	     {
		     printf("SDL Error: %s\n", error);
		     if (line != -1)
			     printf(" + line: %i\n", line);
		     SDL_ClearError();
	     }
     #endif
    }


    void replace(std::string& str, std::string const& to_replace, std::string const& replacement)
    {
        for(size_t i = str.find(to_replace, 0); i != std::string::npos; i = str.find(to_replace, i))
        {
            str.replace(i, to_replace.size(), replacement);
            i += to_replace.size(); // prevents find from catching on something inside of to_replace
        } 
    }


    /************************************************************************/
    /* GetB2FixtureVerts
    /* Returns a vector containing the vertices of a fixture
    /************************************************************************/
#if ASDF_BOX2D == 1
    std::vector<b2Vec2> GetB2FixtureVerts(b2Fixture* fixture){

	    std::vector<b2Vec2> vertices;
	    b2Shape::Type shapeType = fixture->GetType();

	    if ( shapeType == b2Shape::e_circle ){
		    b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
		    for(size_t vertNum = 0; vertNum < size_t(circleShape->GetVertexCount()); vertNum++)
			    vertices.push_back(circleShape->GetVertex(vertNum));
	    }
	    else if ( shapeType == b2Shape::e_polygon )	{
		    b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
		    for(size_t vertNum = 0; vertNum < polygonShape->GetVertexCount(); vertNum++)
			    vertices.push_back(polygonShape->GetVertex(vertNum));
	    }

	    return vertices;
    }


    size_t FindClosestVertexIndex(std::vector<b2Vec2> vertices, b2Vec2 positon){
	    int closestIndex = 0;
	    b2Vec2 closest = vertices[0];
	    for(size_t vertNum = 1; vertNum < vertices.size(); vertNum++){
		    //closest horizontally
		    float xDistClosest = abs(closest.x - positon.x);
		    float xDistNew = abs(vertices[vertNum].x - positon.x);

		    if(xDistNew < xDistClosest){
			    closest = vertices[vertNum];
			    closestIndex = vertNum;
		    }
		    else if (xDistNew == xDistClosest){ //if equal
			    //use vertical distance as tiebreaker
			    float yDistClosest = abs(closest.y - positon.y);
			    float yDistNew = abs(vertices[vertNum].y - positon.y);

			    if(yDistNew < yDistClosest){
				    closest = vertices[vertNum];
				    closestIndex = vertNum;
			    }
		    }
	    }

	    return closestIndex;
    }


    uint64_t binomial_coefficient(uint64_t n, uint64_t k)
    {
        if (k > n)
            return 0;

        if (n - k < k)
            k = n - k;

        uint64_t r = 1;

        for (uint64_t d = 1; d <= k; d++)
        {
            uint64_t mult = n;

            bool divided = true;

            if (mult % d == 0)
                mult /= d;
            else if (r % d == 0)
                r /= d;
            else
                divided = false;

            const uint64_t r_mult = r * mult;
            if (r_mult / mult != r)
                throw std::overflow_error("Overflow");

            r = r_mult;

            if (!divided)
                r /= d;

            n--;
        }

        return r;
    }

#endif
    }
}
