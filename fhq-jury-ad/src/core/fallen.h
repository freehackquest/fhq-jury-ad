#ifndef FALLEN_H
#define FALLEN_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <json.hpp>
#include <deque>

class Fallen {
    public:
        static bool writeFile(const std::string &sFilename, const std::string &sContent);
        static bool writeFile(const std::string &sFilename, const char *pBuffer, const int nBufferSize);

        static unsigned long convertVoid2Int(void *p);
        static std::string getPointerAsHex(void *p);
};

class WSJCppCore {
    public:
        static bool init(
            int argc, char** argv, 
            const std::string &sApplicationName,
            const std::string &sApplicationVersion,
            const std::string &sApplicationAuthor,
            const std::string &sLibraryNameForExports
        );
};

// 
// ---------------------------------------------------------------------

enum FallenHelpParseArgType {
    PARAMETER,
    SINGLE_OPTION
};

// ---------------------------------------------------------------------

class FallenHelpParseArg {
    public:
        // TODO redesign to alias list {"1", "2"}
        FallenHelpParseArg(const std::string &sName, const std::string &sAlias, FallenHelpParseArgType nType, const std::string &sDescription);
        FallenHelpParseArg();
        std::string name();
        std::string alias();
        std::string description();
        bool isOption();

    private:
        std::string m_sName;
        std::string m_sAlias;
        std::string m_sDescription;
        bool m_bIsOption;
};

// ---------------------------------------------------------------------

class FallenHelpParseArgs {
    public:
        FallenHelpParseArgs(int argc, char** argv);
        void setAppName(const std::string &sAppName);
        void setAppVersion(const std::string &sAppVersion);
        bool has(const std::string &sName);
        std::string option(const std::string &sName);
        void addHelp(const std::string &sName, const std::string &sAlias, FallenHelpParseArgType nType, const std::string &sDescription);
        void printHelp();
        bool checkArgs(std::string &sArgsErrors);

    private:
        std::vector<std::string> m_vArgs;
        std::vector<FallenHelpParseArg *> m_vHelpArgs;
        std::string m_sAppName;
        std::string m_sAppVersion;
};

// ---------------------------------------------------------------------

#endif // FALLEN_H
