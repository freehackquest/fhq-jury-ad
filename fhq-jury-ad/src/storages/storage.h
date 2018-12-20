#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <flag.h>
#include <model_scoreboard.h>

class Storage {
public:
    // Storage(const std::string &sType);
    static std::string type() { return "unknown"; };

    virtual bool applyConfigFromFile(
        const std::string &sFilePath, 
        std::vector<ModelTeamConf> &vTeamsConf,
        std::vector<ModelServiceConf> &vServicesConf
    ) = 0;

    virtual void clean() = 0;

    // add new live flag
    virtual void addLiveFlag(const ModelTeamConf &teamConf, const ModelServiceConf &serviceConf, const Flag &sFlag) = 0;

    // list of flags with ended if server up and check another flag lost on down
    virtual std::vector<Flag> endedFlags(const ModelTeamConf &teamConf, const ModelServiceConf &service) = 0;

    // update flag status and update scoreboard table for team 
    virtual void updateFlag(const ModelTeamConf &teamConf, const ModelServiceConf &serviceConf, const Flag &sFlag) = 0;

    // force update scoreboard
    virtual void updateScoreboard(const ModelTeamConf &teamConf, const ModelServiceConf &serviceConf) = 0; 

    // find flag
    virtual bool findFlagByValue(const std::string &sFlag, Flag &resultFlag) = 0;

    // update team stole with check
    virtual bool updateTeamStole(const std::string &sFlag, const std::string &sTeamId) = 0;

    // remove flag
    virtual void removeFlag(Flag &flag) = 0;

    // move flag to archive
    virtual void moveToArchive(Flag &flag) = 0;
};

#endif // STORAGE_H