#pragma once
#include <string>
#include <cstdlib>



class FileSystem
{
private:
    typedef std::string(*Builder) (const std::string& path);

public:
    static std::string getPath(const std::string& path)
    {
        static std::string(*pathBuilder)(std::string const&) = getPathBuilder();
        return (*pathBuilder)(path);
    }

private:
    static std::string const& getRoot()
    {
        char* envRoot = getenv("LOGL_ROOT_PATH");
    
        if (envRoot != nullptr) {
            static std::string envPath = std::string(envRoot);
            return envPath;
        }
        // Macro defined in CMakeLists.txt
        #ifdef ROOT_DIR
            static std::string root = ROOT_DIR;
            return root;
        #else
            static std::string root = "";
            return root;
        #endif
    }

    //static std::string(*foo (std::string const &)) getPathBuilder()
    static Builder getPathBuilder()
    {
        if (getRoot() != "")
            return &FileSystem::getPathRelativeRoot;
        else
            return &FileSystem::getPathRelativeBinary;
    }

    static std::string getPathRelativeRoot(const std::string& path)
    {
        return getRoot() + std::string("/") + path;
    }

    static std::string getPathRelativeBinary(const std::string& path)
    {
        return path;
    }


};
