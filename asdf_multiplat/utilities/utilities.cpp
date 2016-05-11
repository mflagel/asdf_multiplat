#include "stdafx.h"
#include "utilities.h"

#include <cstring>

// #include "reflected_struct.h"

// #include "rapidjson/rapidjson.h"
// #include "rapidjson/filewritestream.h"
// #include "rapidjson/writer.h"

#ifdef _MSC_VER
//#include <Winbase.h>
#include <windows.h>
#else
#include <sys/stat.h>
#endif

// using namespace rapidjson;

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

        // FIXME commenting this out so that gurps_track will work
        // Will set it up to use global app struct later
        // app.running = false;
    }

    struct file_open_exception : std::exception 
    {
        std::string filepath;
        file_open_exception(std::string const& _filepath)
        : filepath(_filepath)
        {}
        virtual const char* what() const noexcept
        {
            return ("Unable to open file: " + filepath).c_str();
        }
    };

    std::string read_text_file(std::string const& filepath) {
        if(!file_exists(filepath))
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

    bool file_exists(std::string const& filepath)
    {
#ifdef _MSC_VER
        /*
        if (FILE *file = fopen(filepath.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }
        */
        //return PathFileExists(filepath.c_str()) > 0;

        DWORD dwAttrib = GetFileAttributes(filepath.c_str());

        return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat buffer;   
        return (stat (filepath.c_str(), &buffer) == 0); 
#endif
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

        return std::move(cwd);
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

    std::vector<std::string> tokenize(char* const str, char* const delimiters)
    {
        std::vector<std::string> tokens;

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

        return tokens;
    }

    /// FIXME move somewhere specific to SDL or something
    /// otherwise non-sdl projects cant use this file
    // void checkSDLError(int line/*=-1*/){
    // #ifndef NDEBUG
	   //  const char *error = SDL_GetError();
	   //  if (*error != '\0')
	   //  {
		  //   printf("SDL Error: %s\n", error);
		  //   if (line != -1)
			 //    printf(" + line: %i\n", line);
		  //   SDL_ClearError();
	   //  }
    // #endif
    // }

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