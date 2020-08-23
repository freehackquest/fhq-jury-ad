#ifndef SERVICE_COSTS_AND_STATISTICS_H
#define SERVICE_COSTS_AND_STATISTICS_H

#include <string>

class ServiceCostsAndStatistics {
    public:
        ServiceCostsAndStatistics(const std::string &sServiceId);
        int allStolenFlagsForService();
        void incrementStolenFlagsForService();
        void setStolenFlagsForService(int nStolenFlags);
        double updateProportionalStolenFlagsForService(int m_nAllStolenFlags);
        double updateCostStolenFlagForService(int nStolenPoints, double nSumOfReverseProportionalStolenFlags);
        double costStolenFlag();

        int allDefenceFlagsForService();
        void incrementDefenceFlagsForService();
        void setDefenceFlagsForService(int nDdefenceFlags);
        double updateProportionalDefenceFlagsForService(int m_nAllDefenceFlags);
        double updateCostDefenceFlagForService(int nDefencePoints, double nSumOfReverseProportionalDefenceFlags);
        double costDefenceFlag();

        std::string getFirstBloodTeamId();
        void setFirstBloodTeamId(const std::string &sFirstBlood);

    private:
        std::string TAG;
        std::string m_sServiceId;
        std::string m_sFirstBlood;

        int m_nAllStolenFlagsForService;
        double m_nReverseProportionalStolenFlags;
        double m_nCostStolenFlag;

        int m_nAllDefenceFlagsForService;
        double m_nReverseProportionalDefenceFlags;
        double m_nCostDefenceFlag;
};

#endif // SERVICE_COSTS_AND_STATISTICS_H