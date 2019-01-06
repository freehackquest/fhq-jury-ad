#include "scoreboard.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <utils_logger.h>
#include <ts.h>

// ---------------------------------------------------------------------

Scoreboard::Scoreboard(bool bRandom,
        int nGameStartInSec,
        int nGameEndInSec,
        int nFlagTimeLiveInSec,
        const std::vector<Team> &vTeamsConf,
        const std::vector<Service> &vServicesConf,
        Storage *pStorage
    ) {
    TAG = "Scoreboard";
    m_bRandom = bRandom;
    m_pStorage = pStorage;
    std::srand(unsigned(std::time(0)));
    m_nGameStartInSec = nGameStartInSec;
    m_nGameEndInSec = nGameEndInSec;
    m_nFlagTimeLiveInSec = nFlagTimeLiveInSec;

    m_mapTeamsStatuses.clear(); // possible memory leak
    for (unsigned int iteam = 0; iteam < vTeamsConf.size(); iteam++) {
        Team teamConf = vTeamsConf[iteam];
        std::string sTeamId = teamConf.id();

        m_mapTeamsStatuses[sTeamId] = new TeamStatusRow(sTeamId, vServicesConf, nGameStartInSec, nGameEndInSec);
        m_mapTeamsStatuses[sTeamId]->setPlace(iteam+1);

        // random values of service for testing
        if (m_bRandom) {
            double nScore = (std::rand() % 10000)/10;
            m_mapTeamsStatuses[sTeamId]->setScore(nScore);
        }

        for (unsigned int iservice = 0; iservice < vServicesConf.size(); iservice++) {
            Service service = vServicesConf[iservice];
            m_mapTeamsStatuses[sTeamId]->setServiceStatus(service.id(), ServiceStatusCell::SERVICE_DOWN);

            // random states of service for testing 
            if (m_bRandom) {
                m_mapTeamsStatuses[sTeamId]->setServiceStatus(service.id(), randomServiceStatus()); 
                m_mapTeamsStatuses[sTeamId]->setTries(std::rand() % 1000);
            }
        }
    }

    // keep the list of the services ids
    for (unsigned int iservice = 0; iservice < vServicesConf.size(); iservice++) {
        m_vServices.push_back(vServicesConf[iservice]);
    }

    {
        m_jsonScoreboard.clear();
        for (unsigned int iteam = 0; iteam < vTeamsConf.size(); iteam++) {
            Team teamConf = vTeamsConf[iteam];
            std::string sTeamId = teamConf.id();
            // team.num();
            nlohmann::json teamData;
            teamData["place"] = m_mapTeamsStatuses[sTeamId]->place();
            teamData["score"] = m_mapTeamsStatuses[sTeamId]->score();
            nlohmann::json jsonServices;
            for (unsigned int iservice = 0; iservice < vServicesConf.size(); iservice++) {
                Service serviceConf = vServicesConf[iservice];
                nlohmann::json serviceData;
                serviceData["defence"] = 0;
                serviceData["attack"] = 0;
                serviceData["uptime"] = 0.0;
                serviceData["status"] = m_mapTeamsStatuses[sTeamId]->serviceStatus(serviceConf.id());
                jsonServices[serviceConf.id()] = serviceData;
            }
            teamData["services"] = jsonServices;
            m_jsonScoreboard[teamConf.id()] = teamData;
        }
    }

}

// ----------------------------------------------------------------------

std::string Scoreboard::randomServiceStatus() {
    std::string sResult = ServiceStatusCell::SERVICE_DOWN;
    int nState = std::rand() % 5;
    switch (nState) {
        case 0: sResult = ServiceStatusCell::SERVICE_UP; break;
        case 1: sResult = ServiceStatusCell::SERVICE_DOWN; break;
        case 2: sResult = ServiceStatusCell::SERVICE_MUMBLE; break;
        case 3: sResult = ServiceStatusCell::SERVICE_CORRUPT; break;
        case 4: sResult = ServiceStatusCell::SERVICE_SHIT; break;
    }
    return sResult;
}

// ----------------------------------------------------------------------

void Scoreboard::setServiceStatus(const std::string &sTeamId, const std::string &sServiceId, const std::string &sStatus) {
    std::lock_guard<std::mutex> lock(m_mutexJson);
    std::string sNewStatus = m_bRandom ? randomServiceStatus() : sStatus;

    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        if (it->second->serviceStatus(sServiceId) != sNewStatus) {
            it->second->setServiceStatus(sServiceId, sNewStatus);
            m_jsonScoreboard[sTeamId]["services"][sServiceId]["status"] = sNewStatus;
        }
    }
}

// ----------------------------------------------------------------------

void Scoreboard::incrementTries(const std::string &sTeamId) {
    std::lock_guard<std::mutex> lock(m_mutexJson);
    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        it->second->setTries(it->second->tries() + 1);
        m_jsonScoreboard[sTeamId]["tries"] = it->second->tries();
    }
}

// ----------------------------------------------------------------------

void Scoreboard::initStateFromStorage() {
    std::map<std::string,TeamStatusRow *>::iterator it;
    for (it = m_mapTeamsStatuses.begin(); it != m_mapTeamsStatuses.end(); it++) {
        TeamStatusRow *pRow = it->second;

        int nTries = m_pStorage->numberOfFlagAttempts(pRow->teamId());
        pRow->setTries(nTries);
        m_jsonScoreboard[pRow->teamId()]["tries"] = nTries;

        for (unsigned int i = 0; i < m_vServices.size(); i++) {
            std::string sServiceID = m_vServices[i].id();
            
            // calculate defence
            int nDefence = m_pStorage->defenceValue(pRow->teamId(), sServiceID);
            pRow->setServiceDefence(sServiceID, nDefence);
            m_jsonScoreboard[pRow->teamId()]["services"][sServiceID]["defence"] = nDefence;

            // calculate attack
            int nAttack = m_pStorage->attackValue(pRow->teamId(), sServiceID);
            pRow->setServiceAttack(sServiceID, nAttack);
            m_jsonScoreboard[pRow->teamId()]["services"][sServiceID]["attack"] = nAttack;

            // calculate uptime
            int nFlagsSuccess = m_pStorage->numberOfFlagSuccessPutted(pRow->teamId(), sServiceID);
            pRow->setServiceFlagsPutted(sServiceID, nFlagsSuccess);
        }
    }
    sortPlaces();
}

// ----------------------------------------------------------------------

void Scoreboard::incrementAttackScore(const std::string &sTeamId, const std::string &sServiceId) {
    std::lock_guard<std::mutex> lock(m_mutexJson);
    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        TeamStatusRow *pRow = it->second; 
        m_jsonScoreboard[sTeamId]["services"][sServiceId]["attack"] = pRow->incrementAttack(sServiceId);
        m_jsonScoreboard[sTeamId]["score"] = pRow->score();
        sortPlaces();
    }
}

// ----------------------------------------------------------------------

void Scoreboard::incrementDefenceScore(const std::string &sTeamId, const std::string &sServiceId) {
    std::lock_guard<std::mutex> lock(m_mutexJson);
    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        TeamStatusRow *pRow = it->second; 
        m_jsonScoreboard[sTeamId]["services"][sServiceId]["defence"] = pRow->incrementDefence(sServiceId);
        m_jsonScoreboard[sTeamId]["score"] = pRow->score();
        sortPlaces();
    }
}

// ----------------------------------------------------------------------

void Scoreboard::incrementFlagsPutted(const std::string &sTeamId, const std::string &sServiceId) {
    std::lock_guard<std::mutex> lock(m_mutexJson);
    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        TeamStatusRow *pRow = it->second;
        pRow->incrementFlagsPutted(sServiceId);
        m_jsonScoreboard[sTeamId]["services"][sServiceId]["uptime"] = pRow->serviceUptime(sServiceId);
        m_jsonScoreboard[sTeamId]["score"] = pRow->score();
        sortPlaces();
    }
}

// ----------------------------------------------------------------------

void Scoreboard::updateScore(const std::string &sTeamId, const std::string &sServiceId) {
    std::lock_guard<std::mutex> lock(m_mutexJson);
    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        TeamStatusRow *pRow = it->second;
        pRow->updateScore(sServiceId);
        m_jsonScoreboard[sTeamId]["services"][sServiceId]["uptime"] = pRow->serviceUptime(sServiceId);
        m_jsonScoreboard[sTeamId]["score"] = pRow->score();
        sortPlaces();
    }
}

// ----------------------------------------------------------------------

std::string Scoreboard::serviceStatus(const std::string &sTeamId, const std::string &sServiceId) {
    std::map<std::string,TeamStatusRow *>::iterator it;
    it = m_mapTeamsStatuses.find(sTeamId);
    if (it != m_mapTeamsStatuses.end()) {
        return it->second->serviceStatus(sServiceId);
    }
    return "";
}

// ----------------------------------------------------------------------

static bool sort_using_greater_than(double u, double v) {
   return u > v;
}

// ----------------------------------------------------------------------

void Scoreboard::sortPlaces() {
    // std::lock_guard<std::mutex> lock(m_mutexJson);
    // sort places
    {
        std::vector<double> vScores;
        std::map<std::string,TeamStatusRow *>::iterator it1;
        for (it1 = m_mapTeamsStatuses.begin(); it1 != m_mapTeamsStatuses.end(); it1++) {
            if(std::find(vScores.begin(), vScores.end(), it1->second->score()) == vScores.end()) {
                vScores.push_back(it1->second->score());
            }
        }
        std::sort(vScores.begin(), vScores.end(), sort_using_greater_than);
        for (it1 = m_mapTeamsStatuses.begin(); it1 != m_mapTeamsStatuses.end(); it1++) {
            double nScore = it1->second->score();
            ptrdiff_t pos = std::find(vScores.begin(), vScores.end(), nScore) - vScores.begin();
            it1->second->setPlace(pos + 1); // TODO fix: same scores will be same place
        }
    }

    // update json
    {
        std::map<std::string,TeamStatusRow *>::iterator it1;
        for (it1 = m_mapTeamsStatuses.begin(); it1 != m_mapTeamsStatuses.end(); it1++) {
            TeamStatusRow *pTeamStatus = it1->second;
            std::string sTeamId_ = pTeamStatus->teamId();

            // std::cout << sTeamNum << ": result: score: " << pTeamStatus->score() << ", place: " << pTeamStatus->place() << "\n";
            m_jsonScoreboard[sTeamId_]["score"] = pTeamStatus->score();
            m_jsonScoreboard[sTeamId_]["place"] = pTeamStatus->place();
            m_jsonScoreboard[sTeamId_]["tries"] = pTeamStatus->tries();
        }
    }
}

// ----------------------------------------------------------------------

void Scoreboard::addFlagLive(const Flag &flag) {
    std::lock_guard<std::mutex> lock(m_mutexFlagsLive);
    std::map<std::string,Flag>::iterator it;
    it = m_mapFlagsLive.find(flag.value());
    if (it != m_mapFlagsLive.end()) {
        Log::warn(TAG, flag.value() + " - flag already exists");
    } else {
        m_mapFlagsLive[flag.value()] = flag;
        m_pStorage->insertFlagLive(flag);
    }
}

// ----------------------------------------------------------------------

std::vector<Flag> Scoreboard::outdatedFlagsLive(const std::string &sTeamId, const std::string &sServiceId) {
    std::lock_guard<std::mutex> lock(m_mutexFlagsLive);
    std::vector<Flag> vResult;
    long nCurrentTime = TS::currentTime_milliseconds() - m_nFlagTimeLiveInSec*1000;
    std::map<std::string,Flag>::iterator it;
    for (it = m_mapFlagsLive.begin(); it != m_mapFlagsLive.end(); it++) {
        Flag flag = it->second;
        if (flag.teamId() == sTeamId 
            && flag.serviceId() == sServiceId
            && flag.timeEnd() < nCurrentTime
        ) {
            vResult.push_back(flag);
        }
    }
    return vResult;
}

// ----------------------------------------------------------------------

bool Scoreboard::findFlagLive(const std::string &sFlagValue, Flag &flag) {
    std::lock_guard<std::mutex> lock(m_mutexFlagsLive);
    std::map<std::string,Flag>::iterator it = m_mapFlagsLive.find(sFlagValue);
    if (it != m_mapFlagsLive.end()) {
        flag.copyFrom(it->second);
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------

void Scoreboard::removeFlagLive(const Flag &flag) {
    std::lock_guard<std::mutex> lock(m_mutexFlagsLive);
    std::map<std::string,Flag>::iterator it;
    it = m_mapFlagsLive.find(flag.value());
    if (it != m_mapFlagsLive.end()) {
        m_mapFlagsLive.erase(it);
        m_pStorage->deleteFlagLive(flag);
    } else {
        Log::warn(TAG, flag.value() + " - flag did not exists");
    }
}

// ----------------------------------------------------------------------

std::string Scoreboard::toString(){
    // TODO mutex
    std::string sResult = ""; 
    std::map<std::string,TeamStatusRow *>::iterator it;
    for (it = m_mapTeamsStatuses.begin(); it != m_mapTeamsStatuses.end(); ++it){
        sResult += it->first + ": \n"
            "\tscore: " + std::to_string(it->second->score()) + "\n"
            + it->second->servicesToString() + "\n";
    }

    return sResult;
}

// ----------------------------------------------------------------------

const nlohmann::json &Scoreboard::toJson(){
    std::lock_guard<std::mutex> lock(m_mutexJson);
    return m_jsonScoreboard;
}

// ----------------------------------------------------------------------
